var settings;

var statusField = document.getElementById("status");
var statusMsgField = document.getElementById("statusmsg");
var updateMsgField = document.getElementById("updatemsg");
var settingsDlg = document.getElementById("settingsDlg");
var uploadDlg = document.getElementById("uploadDlg");

var saveButton = document.getElementById("save");
var rebootButton = document.getElementById("reboot");
var factoryresetButton = document.getElementById("factoryreset");
var fwupdateButton = document.getElementById("fwupdate");
var settingsButton = document.getElementById("settings");
var uploadButton = document.getElementById("upload");

saveButton.onclick = function () {
  statusMsg();
  jsonStr = formToJSON();
  saveSettings(jsonStr);
};
rebootButton.onclick = function () {
  statusMsg();
  reboot();
};
factoryresetButton.onclick = function () {
  statusMsg();
  var r = confirm("Do you really want to erase all settings?");
  if (r == true) {
    factoryReset();
  }
}
fwupdateButton.onclick = function () {
  settingsDlg.style.display = "none";
  uploadDlg.style.display = "block";
}
settingsButton.onclick = function () {
  settingsDlg.style.display = "block";
  uploadDlg.style.display = "none";
}
uploadButton.onclick = function () {
  //updateMsg();
  sendUpdate();
}

function formToJSON() {
  var form = document.getElementById("settingsForm");
  var formData = new FormData(form);
  var object = {};
  for (var pair of formData.entries()) {
    console.log("key: " + pair[0] + " value: " + pair[1]);
  }
  object["wifi_opmode"] = parseInt(formData.get("wifi_opmode"));
  object["static_ip"] = (formData.get("static_ip") === "true");
  object["wifi_ssid"] = formData.get("wifi_ssid");
  object["wifi_secret"] = formData.get("wifi_secret");
  object["wifi_hostname"] = formData.get("wifi_hostname");
  object["wifi_powersave"] = (formData.get("wifi_powersave") === "true");
  object["wifi_ap_fallback"] = (formData.get("wifi_ap_fallback") === "true");
  object["ip_addr"] = formData.get("ip_addr");
  object["ip_gw"] = formData.get("ip_gw");
  object["ip_netmask"] = formData.get("ip_netmask");
  object["ip_dns"] = formData.get("ip_dns");
  object["snmp_contact"] = formData.get("snmp_contact");
  object["snmp_location"] = formData.get("snmp_location");
  object["snmp_ro_community"] = formData.get("snmp_ro_community");
  object["snmp_rw_community"] = formData.get("snmp_rw_community");

  result = JSON.stringify(object);
  console.log(result);
  return result;
}

function JSONToForm(form, json) {
  settings = json;
  console.log(JSON.stringify(json));
  statusField.innerHTML = "Version: " + json.version;
  switch (json.wifi_opmode) {
    case 0: document.getElementById("ap").checked = true; break;
    case 1: document.getElementById("sta").checked = true; break;
    case 2: document.getElementById("eth").checked = true; break;
  }
  switch (json.static_ip) {
    case false: document.getElementById("static_ip_off").checked = true; break;
    case true: document.getElementById("static_ip_on").checked = true; break;
  }
  document.getElementsByName("wifi_ssid")[0].value = json.wifi_ssid;
  document.getElementsByName("wifi_secret")[0].value = json.wifi_secret;
  document.getElementsByName("wifi_hostname")[0].value = json.wifi_hostname;
  document.getElementsByName("wifi_powersave")[0].checked = json.wifi_powersave;
  document.getElementsByName("wifi_ap_fallback")[0].checked = json.wifi_ap_fallback;
  document.getElementsByName("ip_addr")[0].value = json.ip_addr;
  document.getElementsByName("ip_gw")[0].value = json.ip_gw;
  document.getElementsByName("ip_netmask")[0].value = json.ip_netmask;
  document.getElementsByName("ip_dns")[0].value = json.ip_dns;
  document.getElementsByName("snmp_contact")[0].value = json.snmp_contact;
  document.getElementsByName("snmp_location")[0].value = json.snmp_location;
  document.getElementsByName("snmp_ro_community")[0].value = json.snmp_ro_community;
  document.getElementsByName("snmp_rw_community")[0].value = json.snmp_rw_community;
}

function getSettings() {
  let xhr = new XMLHttpRequest();

  xhr.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      settings = JSON.parse(this.responseText);
      JSONToForm("settingsForm", settings);
    }
  };
  xhr.open("GET", "settings.json", true);
  xhr.send();
}

function saveSettings(json) {
  let xhr = new XMLHttpRequest();

  xhr.onreadystatechange = function () {
    if (this.readyState === 4) {
      if (this.status === 200) {
        settings = JSON.parse(this.responseText);
        reboot();
      }
      statusMsg("saveSettings(): " + this.status);
    }
  };
  xhr.open("POST", "settings.json", true);
  xhr.setRequestHeader("Content-Type", "application/json");
  xhr.send(json);
}

function reboot() {
  let xhr = new XMLHttpRequest();

  xhr.onreadystatechange = function () {
    if (this.readyState == 4) {
      statusMsg("reboot(): " + this.status);
    }
  };
  xhr.open("GET", "reboot", true);
  xhr.send();
}

function factoryReset() {
  let xhr = new XMLHttpRequest();

  xhr.onreadystatechange = function () {
    if (this.readyState == 4) {
      statusMsg("factoryReset(): " + this.status);
    }
  };
  xhr.open("GET", "factoryreset", true);
  xhr.send();
}

function sendUpdate() {
  var fileObj = document.getElementById("filename");
  var fileData = new FormData();
  let xhr = new XMLHttpRequest();

  fileData.append("file", fileObj.files[0]);

  function errorAction(event) {
    progressInfo(0);
    updateMsg("upload failed: " + event.type);
  }

  xhr.onreadystatechange = function () {
    if (this.readyState === 4) {
      updateMsg("sendUpdate(): " + this.status);
      if (this.status === 200) {
        updateMsg("update ok");
      } else {
        updateMsg("update failed");
      }
    }
  };
  xhr.upload.addEventListener('progress', (event) => {
    if (event.lengthComputable) {
      var percent = event.loaded / event.total;
      progressInfo(Math.round(percent * 100));
    }
  });
  xhr.upload.addEventListener("error", errorAction);
  xhr.upload.addEventListener("abort", errorAction);
  xhr.upload.addEventListener("timeout", errorAction);

  xhr.open("POST", "update", true);
  xhr.send(fileData);
}

function progressInfo(value) {
  prg.innerHTML = "Progress: " + value + "%";
  bar.style.width = value + "%";
}

function statusMsg(msg) {
  if (msg) {
    statusMsgField.innerHTML += msg + "<br/>";
  } else {
    statusMsgField.innerHTML = "";
  }
}

function updateMsg(msg) {
  if (msg) {
    updateMsgField.innerHTML += msg + "<br/>";
  } else {
    updateMsgField.innerHTML = "";
  }
}

function startEvents(){
  var es = new EventSource('/events');
  es.onopen = function(e) {
    updateMsg("Events Opened");
  };
  es.onerror = function(e) {
    if (e.target.readyState != EventSource.OPEN) {
      updateMsg("Events Closed");
    }
  };
  es.onmessage = function(e) {
    updateMsg("Event: " + e.data);
  };
  es.addEventListener('ota', function(e) {
    updateMsg("Event[ota]: " + e.data);
  }, false);
}

getSettings();
startEvents();
