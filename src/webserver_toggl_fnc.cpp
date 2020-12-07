#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "datastore.h"
#include "webserver_toggl_fnc.h"

WebServer *WebTimeTogglSrv = nullptr;

void getTogglAPIKey(void);
void setTogglAPIKey(void);

void Webserver_Toggl_FunctionsRegister(WebServer *server)
{
  WebTimeTogglSrv = server;
  if (WebTimeTogglSrv == nullptr) {
    abort();
  }
  WebTimeTogglSrv->on("/APIKEY.dat", HTTP_GET, getTogglAPIKey);
  WebTimeTogglSrv->on("/APIKEY.dat", HTTP_POST, setTogglAPIKey); /* needs to process date and time */
}

void getTogglAPIKey(void)
{
  DynamicJsonDocument root(350);
  String response = "";

  String APIKEY = read_toggle_apikey();

  root["APIKEY"] = APIKEY;
  serializeJson(root, response);
  WebTimeTogglSrv->send(200, "text/plain", response);
}

void setTogglAPIKey(void)
{
  if (WebTimeTogglSrv->hasArg("APIKEY")){
    if (WebTimeTogglSrv->arg("APIKEY") == NULL){
      write_toggl_apikey(""); 
    } else {
      write_toggl_apikey(WebTimeTogglSrv->arg("APIKEY"));
    }
  }
  WebTimeTogglSrv->send(200);
}
