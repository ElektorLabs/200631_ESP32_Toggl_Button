#include <ArduinoJson.h>
#include "datastore.h"

#include "webserver_base.h"
#include "webserver_toggl_fnc.h"

WebServer* WebTimeTogglSrv=nullptr;

void getTogglAPIKey( void );
void setTogglAPIKey ( void );

//As we have a kind of time service with NTP arround we also need to reimplement the functions to hanlde it
void Webserver_Toggl_FunctionsRegister(WebServer* server){
  WebTimeTogglSrv=server;
  if(WebTimeTogglSrv==nullptr){
    abort();
  }
  WebTimeTogglSrv->on("/APIKEY.dat", HTTP_GET, getTogglAPIKey);
  WebTimeTogglSrv->on("/APIKEY.dat", HTTP_POST, setTogglAPIKey); /* needs to process date and time */
  
}




void getTogglAPIKey( void ){
  DynamicJsonDocument root(350);
  String response="";
  
  String APIKEY = read_toggle_apikey();
    
  root["APIKEY"] = APIKEY;
  serializeJson(root,response);
  WebTimeTogglSrv->send(200, "text/plain", response);

}

void setTogglAPIKey ( void ){
//void write_toggl_apikey( String apikey);
    if( ! WebTimeTogglSrv->hasArg("APIKEY")  ) { // If the POST request doesn't have username and password data
        /* we are missong something here */
    } else {
    
        if( WebTimeTogglSrv->arg("APIKEY") == NULL){
            Serial.printf("Erase APIKEY\n\r");
            write_toggl_apikey( "" );
        } else {
            Serial.printf("New APIKEY: %s\n\r",WebTimeTogglSrv->arg("APIKEY").c_str());
            write_toggl_apikey( WebTimeTogglSrv->arg("APIKEY") );
        }
    }
    WebTimeTogglSrv->send(200);   

}
