#!/bin/sh

#curl -X PUT http://192.168.6.1/strategy?name=normal --data-binary @normal.lua
#curl -X PUT http://192.168.6.1/strategy?name=fuite --data-binary @fuite.lua
#curl -X PUT http://192.168.6.1/strategy?name=fonctionair --data-binary @fonctionnaire.lua
curl -X PUT http://192.168.6.1/strategy --data-binary @debug_strat.lua
