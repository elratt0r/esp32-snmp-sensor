#!/bin/sh
xxd -i -n "inc.index.html" index.html ../src/inc.index.html.h
xxd -i -n "inc.script.js" script.js ../src/inc.script.js.h
