
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <base64.h>
#include <ArduinoJson.h>
#include "../TimeCore/timecore.h"
#include "togglClient.h"

#ifndef LEAP_YEAR
    #define LEAP_YEAR(Y) ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )
#endif

#define TOGGL_DEBUG 255

WiFiClientSecure client;
HTTPClient httpclient;

//This one is not working!
const char* root_ca = \
"-----BEGIN CERTIFICATE-----\n"\
"MIIF2DCCA8CgAwIBAgIQTKr5yttjb+Af907YWwOGnTANBgkqhkiG9w0BAQwFADCB\n"\
"hTELMAkGA1UEBhMCR0IxGzAZBgNVBAgTEkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4G\n"\
"A1UEBxMHU2FsZm9yZDEaMBgGA1UEChMRQ09NT0RPIENBIExpbWl0ZWQxKzApBgNV\n"\
"BAMTIkNPTU9ETyBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAwMTE5\n"\
"MDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBhTELMAkGA1UEBhMCR0IxGzAZBgNVBAgT\n"\
"EkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4GA1UEBxMHU2FsZm9yZDEaMBgGA1UEChMR\n"\
"Q09NT0RPIENBIExpbWl0ZWQxKzApBgNVBAMTIkNPTU9ETyBSU0EgQ2VydGlmaWNh\n"\
"dGlvbiBBdXRob3JpdHkwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIKAoICAQCR\n"\
"6FSS0gpWsawNJN3Fz0RndJkrN6N9I3AAcbxT38T6KhKPS38QVr2fcHK3YX/JSw8X\n"\
"pz3jsARh7v8Rl8f0hj4K+j5c+ZPmNHrZFGvnnLOFoIJ6dq9xkNfs/Q36nGz637CC\n"\
"9BR++b7Epi9Pf5l/tfxnQ3K9DADWietrLNPtj5gcFKt+5eNu/Nio5JIk2kNrYrhV\n"\
"/erBvGy2i/MOjZrkm2xpmfh4SDBF1a3hDTxFYPwyllEnvGfDyi62a+pGx8cgoLEf\n"\
"Zd5ICLqkTqnyg0Y3hOvozIFIQ2dOciqbXL1MGyiKXCJ7tKuY2e7gUYPDCUZObT6Z\n"\
"+pUX2nwzV0E8jVHtC7ZcryxjGt9XyD+86V3Em69FmeKjWiS0uqlWPc9vqv9JWL7w\n"\
"qP/0uK3pN/u6uPQLOvnoQ0IeidiEyxPx2bvhiWC4jChWrBQdnArncevPDt09qZah\n"\
"SL0896+1DSJMwBGB7FY79tOi4lu3sgQiUpWAk2nojkxl8ZEDLXB0AuqLZxUpaVIC\n"\
"u9ffUGpVRr+goyhhf3DQw6KqLCGqR84onAZFdr+CGCe01a60y1Dma/RMhnEw6abf\n"\
"Fobg2P9A3fvQQoh/ozM6LlweQRGBY84YcWsr7KaKtzFcOmpH4MN5WdYgGq/yapiq\n"\
"crxXStJLnbsQ/LBMQeXtHT1eKJ2czL+zUdqnR+WEUwIDAQABo0IwQDAdBgNVHQ4E\n"\
"FgQUu69+Aj36pvE8hI6t7jiY7NkyMtQwDgYDVR0PAQH/BAQDAgEGMA8GA1UdEwEB\n"\
"/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAArx1UaEt65Ru2yyTUEUAJNMnMvl\n"\
"wFTPoCWOAvn9sKIN9SCYPBMtrFaisNZ+EZLpLrqeLppysb0ZRGxhNaKatBYSaVqM\n"\
"4dc+pBroLwP0rmEdEBsqpIt6xf4FpuHA1sj+nq6PK7o9mfjYcwlYRm6mnPTXJ9OV\n"\
"2jeDchzTc+CiR5kDOF3VSXkAKRzH7JsgHAckaVd4sjn8OoSgtZx8jb8uk2Intzna\n"\
"FxiuvTwJaP+EmzzV1gsD41eeFPfR60/IvYcjt7ZJQ3mFXLrrkguhxuhoqEwWsRqZ\n"\
"CuhTLJK7oQkYdQxlqHvLI7cawiiFwxv/0Cti76R7CZGYZ4wUAc1oBmpjIXUDgIiK\n"\
"boHGhfKppC3n9KUkEEeDys30jXlYsQab5xoq2Z0B15R97QNKyvDb6KkBPvVWmcke\n"\
"jkk9u+UJueBPSZI9FoJAzMxZxuY67RIuaTxslbH9qh17f4a+Hg4yRvv7E491f0yL\n"\
"S0Zj/gA0QHDBw7mh3aZw4gSzQbzpgJHqZJx64SIDqZxubw5lT2yHh17zbqD5daWb\n"\
"QOhTsiedSrnAdyGN/4fy3ryM7xfft0kL0fJuMAsaDk527RH89elWsn2/x20Kk4yl\n"\
"0MC2Hb46TpSi125sC8KKfPog88Tk5c0NqMuRkrF8hey1FGlmDoLnzc7ILaZRfyHB\n"\
"NVOFBkpdn627G190\n"\
"-----END CERTIFICATE-----\n";


typedef enum {
  ReadResponseCode=0,
  SearchPayload,
  ReadPayload,
  DiscardData
} web_readstate_t;

String host = "api.track.toggl.com";

uint8_t buffer[4096]; //4k puffer for data
StaticJsonDocument<4096> doc;

int8_t TogglStopTimeEntry( String apikey, uint32_t id );

int8_t TogglGetRequest( String apikey, String url, String* Result, int8_t* error);
int8_t TogglPutRequest( String apikey, String url, String* Result, int8_t* error);
int8_t TogglPostRequest( String apikey, String url, String payload, String* Result, int8_t* error);

void TogglParseTimeStamp( );
uint32_t TogglParseTimeStamp( String ISO8601Time ){
    // assemble time elements into time_t 
// note year argument is offset from 1970 (see macros in time.h to convert to other formats)
// previous version used full four digit year (or digits since 2000),i.e. 2009 was 2009 or 9

    String starttime =  String(ISO8601Time);
    uint16_t year = starttime.substring(0,4).toInt();
    uint8_t month = starttime.substring(5,7).toInt();
    uint8_t day = starttime.substring(8,10).toInt();
    uint8_t hour = starttime.substring(11,13).toInt();
    uint8_t minute = starttime.substring(14,16).toInt();
    uint8_t second = starttime.substring(17,19).toInt();
    uint8_t offset_hour = starttime.substring(20,22).toInt();
    uint8_t offset_minute = starttime.substring(22,24).toInt();
    int32_t offset=0;                
    char offsetsign = starttime.charAt(19);
    if( offsetsign=='+'){
        offset = offset + ( offset_hour * 60 * 60 ) + ( offset_minute * 60 );

    } else if ( offsetsign=='-'){
        offset = offset - ( offset_hour * 60 * 60 ) -  ( offset_minute * 60 ) ;        
    } else {
        //Parser error
         return 0;
    }

static int8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};
uint32_t ts;

if(month>12){
  month=1;
}

if(day>31){
  day=1;
}

if(hour>23){
  hour=0;
}

if(minute>59){
  minute=0;
}

if(second>59){
  second=0;
}

  int i;
  uint32_t seconds;
  if(year>=1970){
        year=year-1970;
  } else {
      return 0;
  }
  

  // seconds from 1970 till 1 jan 00:00:00 of the given year
  seconds= year*(SECS_PER_DAY * 365);
  for (i = 0; i < year; i++) {
    if (LEAP_YEAR(i)) {
      seconds +=  SECS_PER_DAY;   // add extra days for leap years
    }
  }
  
  // add days for this year, months start from 1
  for (i = 1; i < month; i++) {
    if ( (i == 2) && LEAP_YEAR(year)) { 
      seconds += SECS_PER_DAY * 29;
    } else {
      seconds += SECS_PER_DAY * monthDays[i-1];  //monthDay array starts from 0
    }
  }
  seconds+= (day-1) * SECS_PER_DAY;
  seconds+= hour * SECS_PER_HOUR;
  seconds+= minute * SECS_PER_MIN;
  seconds+= second;
  ts = seconds;
  if(offset < 0){
      if( abs(offset) < ts ) 
      {
          ts += offset;
      } else {
          ts = 0;
      }
 
  } else {
      ts += offset;
  }
  return  ts; 
}


void ToggleSetRooCA( void ){
    client.setCACert(root_ca);
}

 int8_t TogglGetCurrentEntry( String apikey, Toggl_Element_t* Element, bool* has_entry ){
   int8_t error=1;
    if(Element == NULL){
        *has_entry = false;
        return 1;
    }
    String url = "/api/v8/time_entries/current";
    String Result;
     
    if(-1 < TogglGetRequest(apikey,url,&Result, &error)){
        DeserializationError er = deserializeJson(doc, Result.c_str());

        if (er) {
            #if TOGGL_DEBUG > 0 
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(er.f_str());
            #endif
            error = 0;
            *has_entry = false;
        } else {
           if(false == doc["data"].isNull() ){
                JsonObject data = doc["data"];
                uint32_t data_id = data["id"]; 
                const char* data_start = data["start"]; 
          
                if(false == data["description"].isNull()){
                    const char* data_description = data["description"]; 
                    Element->description = String(data_description);
                } else {
                    Element->description = "";
                }
                
                Element->id = data_id;
                Element->starttime = TogglParseTimeStamp( data_start );
                 *has_entry = true;
                 error = 1;
           } else {
               #if TOGGL_DEBUG > 0
                Serial.print("Nothing running");
               #endif
                *has_entry = false;
                error = 1;
           }
        }
    } else {
         *has_entry = false;
         error = -1;
    }
    return error;
}

int8_t ToggleStopCurrentEntry( String apikey){

    Toggl_Element_t Result;
    uint8_t retry_count =0;
    bool has_entry=false;
    while( 1 != TogglGetCurrentEntry( apikey , &Result, &has_entry) ){
        retry_count++;
        delay( 100 );
        if(retry_count>3){
            #if TOGGL_DEBUG > 0
                Serial.println("Connection error");
            #endif
            return -1;
        }
   }
   if(true == has_entry){
        #if TOGGL_DEBUG > 0
            Serial.print("Item ");
            Serial.print(Result.id);
            Serial.print(" started at ");
            Serial.print( Result.starttime );
            Serial.print(" with description ");
            Serial.println( Result.description );
            Serial.println("Stop_Entry");
        #endif
        TogglStopTimeEntry( apikey, Result.id );

   } else {
       #if TOGGL_DEBUG > 0
            Serial.println("No valid entry returned");
       #endif
   }
   return 1;

}


/* If an entry is running it will be stopped and a new one generated */
int8_t ToggleStartNewEntry( String apikey , String description){
  
    String url = "/api/v8/time_entries/start";
    String payload;
    int8_t error;
    String JSONResult;
    
 
    const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2);
    DynamicJsonDocument doc(capacity);
  
    JsonObject time_entry = doc.createNestedObject("time_entry");
    if(description.length()>0){
        time_entry["description"] = description.c_str();
    }
    time_entry["created_with"] = "ESP32";
    
    
    serializeJson(doc, payload);
    return TogglPostRequest(apikey,url, payload, &JSONResult, &error );
    


}

int8_t TogglStopTimeEntry( String apikey, uint32_t id ){
     uint8_t retry_count =0;
     String id_str=String(id);
     String url = "/api/v8/time_entries/" + id_str + "/stop";
     String Result;
     int8_t error;
     while( 1 != TogglPutRequest(apikey,url,&Result,&error) ){
         retry_count++;
         if(retry_count>3){
             #if TOGGL_DEBUG > 0
                Serial.println("Connection error at PUT");
             #endif
             return -1;
         }

     } 
     return 1;
}

int8_t ToggleConnectToHost( void ){
    int8_t nw_error =1;
     if(false == client.connected()){
        #if TOGGL_DEBUG > 0 
            Serial.println("Reconnet to Host with SSL");
        #endif
        nw_error = client.connect(host.c_str(),443);
        if(!(nw_error)){
            #if TOGGL_DEBUG > 0 
            Serial.print("Connection failed with code");
            Serial.println(nw_error);
            #endif
        } else {
            #if TOGGL_DEBUG > 0 
            Serial.print("Connected to ");
            Serial.println(host);     
            #endif
        }
    } else {
        #if TOGGL_DEBUG > 0 
        Serial.print("Flush Data connection:");
        #endif
        client.flush();
        while( 0 !=  client.available() ){
                 int16_t data = client.read();
                 if(data>=0){
                    #if TOGGL_DEBUG > 0 
                        Serial.print(((char)(data)));
                    #endif
                 }
        }
        #if TOGGL_DEBUG > 0 
            Serial.println("");
        #endif
    }    
    return  nw_error;
}

/*
uint8_t ProcessResponse( String* Result, int8_t* error ){
    //We read untill the rist end of line ( LF )
    int8_t nw_error =1;
    uint16_t buffer_idx=0;
    uint16_t timeout = 0;
    web_readstate_t ReadState = ReadResponseCode;
    for(uint32_t i=0;i<sizeof(buffer);i++){
        buffer[i]=0;
    }
    
    while (client.available() == 0)
    {
        delay(10);
        if (timeout++ > 200){
            *error =  -3;
            return nw_error;
        }; //wait 2s max
    }
    timeout = 0;

    while (client.available()) {
         int16_t data = client.read();
         if(data>=0){
             Serial.print(((char)(data)));
             switch( ReadState){
                 case web_readstate_t::ReadResponseCode:{
                     if(buffer_idx<(sizeof(buffer)-1)){
                        buffer[buffer_idx]=(uint8_t)data;
                       
                        if( (data=='\n') && ( buffer_idx > 0 ) ) {
                            //We have the line read check what we have as result
                            if(0==strncmp("HTTP/1.1 200 OK",(char*)buffer, 15) ){
                                //Line fits
                                buffer_idx=0;
                                ReadState = web_readstate_t::SearchPayload;
                            } else if ( 0 == strncmp("HTTP/1.1 409 Conflict",(char*)buffer, 21)){
                                //What ever happend we have a conflicg e.g. not existing or aleady stopped...
                                //At this point we can consider the job done....
                                buffer[buffer_idx]=0;
                                *error =  -10;
                                ReadState = web_readstate_t::DiscardData;
                                #if TOGGL_DEBUG > 0 
                                    Serial.print("Responsecode confict with:");
                                    Serial.println((char*)buffer);
                                #endif
                                
                                
                                
                            } else {
                                //Problem
                                buffer[buffer_idx]=0;
                                *error =  -6;
                                ReadState = web_readstate_t::DiscardData;
                                #if TOGGL_DEBUG > 0
                                Serial.print("Responsecode failed with:");
                                Serial.println((char*)buffer);
                                #endif
                                
                            }

                        } else{
                             buffer_idx++;
                        }

                    } else {
                        *error =  -4;
                        ReadState = web_readstate_t::DiscardData;
                    }
                 } break;

                 case web_readstate_t::SearchPayload:{
                    if(buffer_idx<(sizeof(buffer)-1)){
                        
                        if(buffer_idx<3){
                            buffer[buffer_idx]=(uint8_t)data;
                            buffer_idx++;
                        } else {
                            //Move data....
                            buffer[0]=buffer[1];
                            buffer[1]=buffer[2];
                            buffer[2]=buffer[3];
                            buffer[3] = (uint8_t)data;

                            if(0==strncmp("\r\n\r\n",(char*)buffer, 4) ){
                                //Line fits
                                buffer_idx=0;
                                ReadState = web_readstate_t::ReadPayload;
                            } else{
                               //do nothing...
                            }

                        } 

                    } else {
                        *error =  -4;
                        ReadState = web_readstate_t::DiscardData;
                    }

                 }break;

                 case web_readstate_t::ReadPayload:{
                    if(buffer_idx<(sizeof(buffer)-1)){
                        buffer[buffer_idx]=(uint8_t)data;
                        buffer_idx++;
                    } else {
                        #if TOGGL_DEBUG > 0
                            Serial.println("Payload overflow");
                            Serial.println((char*)buffer);
                        #endif
                        buffer[buffer_idx]=0;
                        *error =  -4;
                        ReadState = web_readstate_t::DiscardData;
                    }

                 }break;

                case web_readstate_t::DiscardData:{
                    //We do nothing and wait till we run out of data.....
                }break;

                default:{
                    //We do nothing untill we run out of data...
                }break;
             }

         }
    }
    buffer[buffer_idx]=0; //terminate string
    if(ReadState!=web_readstate_t::ReadPayload){
        #if TOGGL_DEBUG > 0
            Serial.println("Payload not found");
        #endif
        *error =  -9;
    } else {
        *Result = String((char*)buffer);
        *error =  0;
    }
    
    return nw_error;
}
*/

int8_t TogglPostRequest( String apikey, String url, String payload, String* Result, int8_t* error){
    int8_t nw_error =0;
    nw_error = ToggleConnectToHost();
    if(1 != nw_error ){
        //This indicates a connection problem
        return nw_error;
    }
   
    String user = apikey + ":api_token";
    String b64 = base64::encode(user);

    /* 
    String header = "POST " + url + " HTTP/1.1\r\n" +
                    "Host: " + host + "\r\n" +
                    "Authorization: Basic " + b64 + "\r\n" +
                    "Content-Type: application/json" + "\r\n" +
                    "Content-length: " + payload.length() + "\r\n" +  "\r\n";
                    
    client.print(header);
    client.print(payload);
    nw_error = ProcessResponse( Result, error );
    */

    String link = "https://"+host+url;
    Serial.print("Acess:");
    Serial.println(link);
    Serial.println(payload);

    httpclient.begin(client,link);
    httpclient.setReuse(true);
    httpclient.setAuthorization((const char*)b64.c_str());
    httpclient.addHeader("Content-Type","application/json");
    int httpCode = httpclient.POST(payload);
    
    if (httpCode > 0) { //Check for the returning code
 
        if(httpCode!=200){
            Serial.println("Response Error");
            String payload = httpclient.getString();   
            Serial.println(httpCode);
            Serial.println(payload);
            *error = 1;
        } else{

            *Result = httpclient.getString();    ;
        }
    } else {
        Serial.print("Request Error");
        Serial.println(httpclient.errorToString(httpCode));
    }
    httpclient.end();



 
    return nw_error;

    

}

int8_t TogglPutRequest( String apikey, String url, String* Result, int8_t* error){
    int8_t nw_error =0;
    nw_error = ToggleConnectToHost();
    if(1 != nw_error ){
        //This indicates a connection problem
        return nw_error;
    }
    
    String user = apikey + ":api_token";
    String b64 = base64::encode(user);

    /*
   
    String header = "PUT " + url + " HTTP/1.1\r\n" +
                    "Host: " + host + "\r\n" +
                    "Authorization: Basic " + b64 + "\r\n" +
                    "Content-Type: application/json" + "\r\n" +
                    "Content-length: 0" + "\r\n" +"\r\n";

    
    client.print(header);
    nw_error = ProcessResponse( Result, error );

    */


    String link = "https://"+host+url;
    Serial.print("Acess:");
    Serial.println(link);
    httpclient.begin(client,link);
    httpclient.setReuse(true);
    httpclient.setAuthorization((const char*)b64.c_str());
    httpclient.addHeader("Content-Type","application/json");
    httpclient.addHeader("Content-length","0");
    int httpCode = httpclient.PUT((uint8_t*)(NULL),0);
    
   if (httpCode > 0) { //Check for the returning code
 
        if(httpCode!=200){
            Serial.println("Response Error");
            String payload = httpclient.getString();   
            Serial.println(httpCode);
            Serial.println(payload);
            *error = 1;
        } else{

            *Result = httpclient.getString();    ;
        }
    } else {
        Serial.print("Request Error");
        Serial.println(httpclient.errorToString(httpCode));
    }
    httpclient.end();

    return nw_error;

}


int8_t TogglGetRequest( String apikey, String url, String* Result, int8_t* error){
   
    int8_t nw_error = ToggleConnectToHost();
    if(1 != nw_error ){
        //This indicates a connection problem
        return nw_error;
    }
    
    
    String user = apikey + ":api_token";
    
    
    String b64 = base64::encode(user);
    
    /*
    String header = "GET " + url + " HTTP/1.1\r\n" +
                    "Host: " + host + "\r\n" +
                    "Authorization: Basic " + b64 + "\r\n\r\n";
    client.print(header);
    nw_error = ProcessResponse( Result, error );
    */
    
   String link = "https://"+host+url;
   Serial.print("Acess:");
   Serial.println(link);
   httpclient.begin(client,link);
   httpclient.setReuse(true);
   httpclient.setAuthorization((const char*)b64.c_str());
  
   int httpCode = httpclient.GET();                                     
 
    if (httpCode > 0) { //Check for the returning code
 
        if(httpCode!=200){
            Serial.println("Response Error");
            String payload = httpclient.getString();   
            Serial.println(httpCode);
            Serial.println(payload);
            *error = 1;
        } else{

            *Result = httpclient.getString();    ;
        }
    } else {
        Serial.print("Request Error");
        Serial.println(httpclient.errorToString(httpCode));
    }
    httpclient.end();
    
    return nw_error;

}