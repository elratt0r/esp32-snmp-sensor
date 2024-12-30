#include "inc.index.html.h"
#include "inc.script.js.h"
#include "main.h"

static AsyncWebServer server(80);
static AsyncEventSource events("/events");

static void handleNotFound(AsyncWebServerRequest* request)
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += request->url();
    message += "\nMethod: ";
    message += (request->method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += request->args();
    message += "\n";
    for (uint8_t i = 0; i < request->args(); i++) {
        message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
    }
    request->send(404, "text/plain", message);
}

void webserver_setup()
{
	Serial.println("[WEB] setup begin");

    // GET /
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        AsyncWebServerResponse* response = request->beginResponse_P(
            200, "text/html", inc_index_html, inc_index_html_len);
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });

    // GET /script.js
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        AsyncWebServerResponse* response = request->beginResponse_P(
            200, "application/javascript", inc_script_js, inc_script_js_len);
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });

    // GET /settings.json
    server.on("/settings.json", HTTP_GET, [](AsyncWebServerRequest* request) {
        String output = get_settings();
        AsyncWebServerResponse* response = request->beginResponse(200, "application/json", output);
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
    });

    // OPTIONS /settings.json
    server.on("/settings.json", HTTP_OPTIONS, [](AsyncWebServerRequest* request) {
        AsyncWebServerResponse* response = request->beginResponse(204, "text/html");
        response->addHeader("Access-Control-Allow-Origin", "*");
        response->addHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
        response->addHeader("Access-Control-Allow-Credentials", "false");
        response->addHeader("Access-control-Allow-Headers", "x-requested-with");
        response->addHeader("Access-Control-Allow-Headers", "X-PINGOTHER, Content-Type");
        request->send(response);
    });

    // POST /settings.json
    server.on("/settings.json", HTTP_POST, [](AsyncWebServerRequest* request) {}, NULL, [](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
		JsonDocument json;
		DeserializationError error = deserializeJson(json, data);
		Serial.printf("[WEB] /settings.json: post settings (%d)\r\n", error);
		if (error || !parse_settings(json)) {
		request->send(501, "text/plain", "deserializeJson failed");
		} else {
		String output = get_settings();
		AsyncWebServerResponse *response =
			request->beginResponse(200, "application/json", output);
		response->addHeader("Access-Control-Allow-Origin", "*");
		request->send(response);
		}
		Serial.printf("[WEB] /settings.json: post settings done\r\n"); });

    // POST /update
    server.on("/update", HTTP_POST, [](AsyncWebServerRequest* request) {}, [](AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final) {
		// debug
		//Serial.printf("[WEB] index %u, len %u, final %d\r\n", index, len, final);

		// begin
		if (!index) {
			Serial.printf("[WEB] upload start: %s\r\n", filename.c_str());
			events.send("upload start", "ota");
			if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
				Serial.print("[WEB] update.begin: error: ");
				Serial.println(Update.errorString());
				events.send("update.begin: error", "ota");
				events.send(Update.errorString(), "ota");
				request->send(501, "text/plain", "update.begin failed\r\n");          
			}
		}
		// loop
		if (Update.isRunning()) {
			if (Update.write(data, len) != len) {
				Serial.print("[WEB] update.write: error: ");
				Serial.println(Update.errorString());
				events.send("update.write: error", "ota");
				events.send(Update.errorString(), "ota");
				request->send(501, "text/plain", "update.write failed\r\n");
			}
		}
		// final
		if (final) {
			Serial.printf("[WEB] upload end: %u\r\n", index + len);
			events.send("upload end", "ota");
			char p[32];
			snprintf(p, sizeof(p), "upload bytes: %u", index + len);
			events.send(p, "ota");
			delay(1000);
			if (!Update.end(true)) {
				Serial.print("[WEB] update.end: error: ");
				Serial.println(Update.errorString());
				events.send("update.end: error", "ota");
				events.send(Update.errorString(), "ota");
				request->send(501, "text/plain", "update.end failed\r\n");
			} else {
				Serial.println("[WEB] update: success");
				events.send("update: success", "ota");
				request->send(200, "text/plain", "update success\r\n");
				// reboot after disconnect
				request->onDisconnect([](){
					esp_reboot("web_update");
				});
			}
		}
	});

    // GET /reboot
    server.on("/reboot", [](AsyncWebServerRequest* request) {
        AsyncWebServerResponse* response = request->beginResponse(200, "text/plain", "OK\r\n");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
        // reboot after disconnect
        request->onDisconnect([]() {
            esp_reboot("web_reboot");
        });
    });

    // GET /factoryreset
    server.on("/factoryreset", [](AsyncWebServerRequest* request) {
        AsyncWebServerResponse* response = request->beginResponse(200, "text/plain", "OK\r\n");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
        // reset after disconnect
        request->onDisconnect([]() {
            factory_reset("web_factoryreset");
        });
    });

    // events
    events.onConnect([](AsyncEventSourceClient* client) {
        client->send("hello!", NULL, millis(), 1000);
    });
    server.addHandler(&events);

    // notfound
    server.onNotFound(handleNotFound);

    server.begin();

	Serial.println("[WEB] setup done");
}