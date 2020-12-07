#include <SPI.h>
#include <TFT_eSPI.h>

#include <Arduino.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "FS.h"
#include <Wire.h> 
#include "SPIFFS.h"
#include <Update.h>


#include "./wifi_net.h"

#include "./src/TimeCore/timecore.h"
#include "./src/NTPClient/ntp_client.h"

#include "./src/sdcard/sdcard_if.h"

#include "./src/togglClient/togglClient.h"

#include "./datastore.h"
#include "webserver_time_fnc.h"

#include "./src/M5StackPowerClass/power.h"
#include "./src/Menus/menu.h"



#include "./src/Menus/Free_Fonts.h"

/* We define the pins used for the various components */
#ifdef ARDUINO_M5Stack_Core_ESP32
  

  #define I2C0_SCL             ( 22 )
  #define I2C0_SDA             ( 21 )


  #define SD_MISO              ( 15 )
  #define SD_MOSI              ( 13 )
  #define SD_SCK               ( 14 )
  #define SD_CS0               ( 12 )

  #define USERBTN0             ( 39 )
  #define USERBTN1             ( 37 )
  #define USERBTN2             ( 38 )

  #define LCD_CS               ( 14 )
  #define LCD_DC               ( 27 )
  #define LCD_RST              ( 33 )
  #define LCD_BL               ( 32 )

  #define SPEAKER              ( 25 )

#else
  #error "Board not supported"
#endif

//--------------------------------------------------------------------------------------------------
Timecore TimeCore;
NTP_Client NTPC;
// Use hardware SPI
TFT_eSPI tft = TFT_eSPI();
Menus Menu = Menus(tft);

//Power management for M5
POWER Power;

//--------------------------------------------------------------------------------------------------

void StartToggl( void );


// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.
uint16_t read16(fs::File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(fs::File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

void drawBmp(const char *filename, int16_t x, int16_t y) {

  if ((x >= tft.width()) || (y >= tft.height())) return;

  fs::File bmpFS;

  // Open requested file on SD card
  bmpFS = SPIFFS.open(filename, "r");

  if (!bmpFS)
  {
    Serial.print("File not found");
    return;
  }

  uint32_t seekOffset;
  uint16_t w, h, row, col;
  uint8_t  r, g, b;

  uint32_t startTime = millis();

  if (read16(bmpFS) == 0x4D42)
  {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);

    if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0))
    {
      y += h - 1;

      bool oldSwapBytes = tft.getSwapBytes();
      tft.setSwapBytes(true);
      bmpFS.seek(seekOffset);

      uint16_t padding = (4 - ((w * 3) & 3)) & 3;
      uint8_t lineBuffer[w * 3 + padding];

      for (row = 0; row < h; row++) {
        
        bmpFS.read(lineBuffer, sizeof(lineBuffer));
        uint8_t*  bptr = lineBuffer;
        uint16_t* tptr = (uint16_t*)lineBuffer;
        // Convert 24 to 16 bit colours
        for (uint16_t col = 0; col < w; col++)
        {
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }

        // Push the pixel row to screen, pushImage will crop the line if needed
        // y is decremented as the BMP image is drawn bottom up
        tft.pushImage(x, y--, w, 1, (uint16_t*)lineBuffer);
      }
      tft.setSwapBytes(oldSwapBytes);
      Serial.print("Loaded in "); Serial.print(millis() - startTime);
      Serial.println(" ms");
    }
    else Serial.println("BMP format not recognized.");
  }
  bmpFS.close();
}





/**************************************************************************************************
 *    Function      : setup_iopins
 *    Description   : This will enable the IO-Pins
 *    Input         : none 
 *    Output        : none
 *    Remarks       : none 
 **************************************************************************************************/
void setup_iopins( void ){
  // Every pin that is just IO will be defined here 
  pinMode( USERBTN0, INPUT_PULLUP );
  //If this button is read as 0 this means it is pressed
  #ifdef USERBTN1
    if(USERBTN1 >= 0){
      pinMode( USERBTN1, INPUT_PULLUP );
    }
  #endif

  #ifdef USERBTN2
    if(USERBTN2 >= 0){
      pinMode( USERBTN2, INPUT_PULLUP );
    }
  #endif

  #ifdef ARDUINO_M5Stack_Core_ESP32
    pinMode( USERBTN2, INPUT_PULLUP );
   
    /*
    pinMode( LCD_CS, OUTPUT );
    digitalWrite(LCD_CS,LOW);

    pinMode( LCD_DC, OUTPUT );
    digitalWrite(LCD_DC,LOW);

    pinMode( LCD_RST, OUTPUT );
    digitalWrite(LCD_RST,LOW);
    */
    
    pinMode( LCD_BL, OUTPUT );
    digitalWrite(LCD_BL,LOW);
    
    pinMode ( SPEAKER , OUTPUT );
    digitalWrite(SPEAKER,LOW);
  #endif

}


/**************************************************************************************************
 *    Function      : StartOTA
 *    Description   : This will prepare the OTA Service
 *    Input         : none 
 *    Output        : none
 *    Remarks       : none 
 **************************************************************************************************/
void StartOTA(){
  Serial.println("Start OTA Service");
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

}

/**************************************************************************************************
 *    Function      : NTP_Task
 *    Description   : Task for the NTP Sync 
 *    Input         : none 
 *    Output        : none
 *    Remarks       : none 
 **************************************************************************************************/
void NTP_Task( void * param){
  #ifdef DEBUG_SERIAL
    Serial.println(F("Start NTP Task now"));
  #endif
  NTPC.ReadSettings();
  NTPC.begin( &TimeCore );
  NTPC.Sync();

  /* As we are in a sperate thread we can run in an endless loop */
  while( 1==1 ){
    /* This will send the Task to sleep for one second */
    vTaskDelay( 1000 / portTICK_PERIOD_MS );  
    NTPC.Tick();
    NTPC.Task();
  }
}


void StartNTP( void ){
    //This is a dedecated Task for the NTP Service 
  xTaskCreatePinnedToCore(
      NTP_Task,       /* Function to implement the task */
      "NTP_Task",  /* Name of the task */
      10000,          /* Stack size in words */
      NULL,           /* Task input parameter */
      1,              /* Priority of the task */
      NULL,           /* Task handle. */
      1); 
}




/**************************************************************************************************
 *    Function      : setup
 *    Description   : This will onyl run once after boot
 *    Input         : none 
 *    Output        : none
 *    Remarks       : none 
 **************************************************************************************************/
void setup() {
  Serial.begin(115200);
  SPIFFS.begin(); /* This can be called multiple times, allready mounted it will just return */
  datastoresetup();
  
  setup_iopins();

  //We also need to mount the SPIFFS
  if(!SPIFFS.begin(true)){
      Serial.println("An Error has occurred while mounting SPIFFS");
  }

  tft.init();
  tft.invertDisplay( true ); // Where invert is true or false
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);
  drawBmp("/wifi.bmp", 128, 128);
  Power.begin();
  //Next step is to setup wifi and check if the configured AP is in range
  WiFiClientEnable(true); //Force WiFi on 
  WiFiForceAP(false); //Diable Force AP
  #ifdef DEBUG_SERIAL
    Serial.println("Continue boot");
  #endif
  TimeCore.begin(true);
  
  Webserver_Time_FunctionRegisterTimecore(&TimeCore);
  Webserver_Time_FunctionRegisterNTPClient(&NTPC);
  
  
  RegisterWiFiConnectedCB(StartOTA);
  RegisterWiFiConnectedCB(StartNTP);
  RegisterWiFiConnectedCB(StartToggl);

  tft.setCursor(0, 0, 2);
  // Set the font colour to be white with a black background, set text size multiplier to 1
  tft.setTextColor(TFT_BLACK,TFT_WHITE);  tft.setTextSize(1);
  // We can now plot text on screen using the "print" class
  tft.setTextFont(4);
  tft.println("Initialize WiFi");
  Serial.println("Initialize WiFi");
  //If the button is pressed during start we will go to the AP Mode
  if( ( 0==digitalRead(USERBTN2) ) && (0 == digitalRead(USERBTN1) ) ){
      Serial.println("Force System to AP");
      tft.println("Force System to AP");
      initWiFi( false , true );   
  } else {
      initWiFi( false, false );
  }
  
 
 
}


/**************************************************************************************************
 *    Function      : loop
 *    Description   : Main Loop
 *    Input         : none 
 *    Output        : none
 *    Remarks       : none 
 **************************************************************************************************/
void loop() {
  /* This will be executed in the arduino main loop */
  ArduinoOTA.handle();
  NetworkLoopTask();
}


void RefreshEntry( String apikey, bool force_redraw){
    static Toggl_Element_t PrevEntry = {.id=0, .description=String(""), .starttime=0 };
    bool has_entry=false;
    bool con_error = true;
    Toggl_Element_t Entry;
              while(true ==  con_error ){
                if( 1 == TogglGetCurrentEntry(  apikey, &Entry, &has_entry ) ){
                  //Request okay no error
                  if(true==has_entry){
                    if( (Entry.id != PrevEntry.id ) || ( Entry.description != PrevEntry.description ) || ( Entry.starttime != PrevEntry.starttime ) ||( force_redraw == true )){
                        PrevEntry.id = Entry.id;
                        PrevEntry.description = Entry.description;
                        PrevEntry.starttime = Entry.starttime;
                        tft.fillScreen(TFT_WHITE);
                        tft.setCursor(0, 0, 2);
                        // Set the font colour to be white with a black background, set text size multiplier to 1
                        tft.setTextColor(TFT_BLACK,TFT_WHITE);  
                        tft.setTextSize(1);
                        // We can now plot text on screen using the "print" class
                        tft.setTextFont(4);
                        tft.fillScreen(TFT_WHITE);
                        tft.setCursor(0,0);
                            
                        tft.println("Description:");
                        tft.println(Entry.description);
                        
                        tft.println("Begin: ");
                        uint32_t now = TimeCore.ConvertToLocalTime( Entry.starttime);
                        datum_t date = TimeCore.ConvertToDatum(now);
                        tft.print(date.year);
                        tft.print("-");
                        if(date.month<10){
                          tft.print("0");  
                        }
                        tft.print(date.month);
                        tft.print("-");
                        if(date.day<10){
                          tft.print("0");  
                        }
                        tft.print(date.day);
                        tft.print(" @ ");
                        if(date.hour<10){
                          tft.print("0");  
                        }
                        tft.print(date.hour);
                        tft.print(":");

                        if(date.minute<10){
                          tft.print("0");  
                        }
                        tft.print(date.minute);
                        tft.print(":");
                        
                        if(date.second<10){
                          tft.print("0");  
                        }
                        
                        tft.println(date.second);
                    }


                  } else {
                    tft.fillScreen(TFT_WHITE);
                    tft.setCursor(0, 0, 2);
                    // Set the font colour to be white with a black background, set text size multiplier to 1
                    tft.setTextColor(TFT_BLACK,TFT_WHITE);  
                    tft.setTextSize(1);
                    // We can now plot text on screen using the "print" class
                    tft.setTextFont(4);
                    tft.fillScreen(TFT_WHITE);
                    tft.setCursor(0,0);
                    tft.println("No entry is runnning");
                    
                  }
                  drawBmp("/new.bmp", 240, 176);
                  drawBmp("/stop.bmp", 128, 176);
                  drawBmp("/power.bmp", 32, 176);
                  con_error = false;
                 
                } else {
                  //Connection error retry.....
                    tft.fillScreen(TFT_WHITE);
                    tft.setCursor(0, 0, 2);
                    // Set the font colour to be white with a black background, set text size multiplier to 1
                    tft.setTextColor(TFT_BLACK,TFT_WHITE);  
                    tft.setTextSize(1);
                    // We can now plot text on screen using the "print" class
                    tft.setTextFont(4);
                    tft.fillScreen(TFT_WHITE);
                    tft.setCursor(0,0);
                    tft.println("Connection error");
                    tft.println("No answare from toggl");
                    tft.println("Retry in 5 seconds");
                    PrevEntry = {.id=0, .description=String(""), .starttime=0 };
                  delay(5000);
                  con_error = true;
                }
            }
}



String ToggApiKeyGet(){
   String apikey =  read_toggle_apikey();
   return apikey;
}

void StartNewEntry( String Description){
  String apikey = ToggApiKeyGet();
  ToggleStartNewEntry(apikey,Description);
  return;
}




void TogglTask( void* param){
 
    bool force_refresh=true;
    bool has_api_key=true;
    uint8_t last_usrbtn2=1;
    uint8_t last_usrbtn1=1;
    uint8_t last_usrbtn0=1;
    uint32_t refreshtimer =0;
   
    Menu.SetDescriptionEntrys(0,"No Description");
    Menu.SetDescriptionEntrys(1,"Videoconference");
    Menu.SetDescriptionEntrys(2,"Rollcall");
    Menu.SetDescriptionEntrys(3,"Check Mails");
    Menu.SetDescriptionEntrys(4,"Check Teams");
    Menu.SetDescriptionEntrys(5,"Review (payed)");
    Menu.SetDescriptionEntrys(6,"Review (shop)");
    Menu.SetDescriptionEntrys(7,"Article writing");
    Menu.SetDescriptionEntrys(8,"Coding");
    Menu.SetDescriptionEntrys(9,"GoProof");
    Menu.SetDescriptionEntrys(10,"Make Photos");
    Menu.SetDescriptionEntrys(11,"Move Text to EP");
    Menu.SetDescriptionEntrys(12,"Add Material to EP");
    Menu.SetDescriptionEntrys(13,"IT infrastuctur");
    Menu.SetDescriptionEntrys(14,"Office 365 2FS");
    Menu.SetDescriptionEntrys(15,"Toggl Overhead");
    
    //ToggleSetRooCA();
    
    while (1 == 1)
    {
      if (ToggApiKeyGet().length() < 1)
      {
        if (has_api_key == true)
        {
          tft.fillScreen(TFT_WHITE);
          tft.setCursor(0, 0);
          tft.print("NO API KEY SET");
          has_api_key = false;
          force_refresh = true;
          delay(1000);
        }
      }
      else
      {
        has_api_key = true;

        if (true == Menu.RenderMenu(Menus::NothingPressed))
        {

          if (refreshtimer <= 0)
          {
            RefreshEntry(ToggApiKeyGet(), force_refresh);
            refreshtimer = 240;
            force_refresh = false;
          }

          if (0 == digitalRead(USERBTN2) && (last_usrbtn2 == 1))
          {
            Serial.println("Stop current entry");
            tft.fillScreen(TFT_WHITE);
            drawBmp("/endtime.bmp", 64, 64);
            tft.setCursor(0, 0);
            tft.print("Stop current entry");

            ToggleStopCurrentEntry(ToggApiKeyGet());
            refreshtimer = 2;
          }

          if ((0 == digitalRead(USERBTN1)) && (last_usrbtn1 == 1))
          {

            Menu.RenderMenu(Menus::KeyA_Pressed);
          }

          if ((0 == digitalRead(USERBTN0)) && (last_usrbtn0 == 1))
          {
            Serial.println("Shutdown");
            tft.fillScreen(TFT_WHITE);
            tft.setCursor(0, 0);
            tft.print("Power down");
            digitalWrite(LCD_BL, HIGH);
            Power.powerOFF();
            refreshtimer = 2;
          }

          if (refreshtimer > 0)
          {
            refreshtimer--;
          }

          last_usrbtn2 = digitalRead(USERBTN2);
          last_usrbtn1 = digitalRead(USERBTN1);
          last_usrbtn0 = digitalRead(USERBTN0);
          delay(250);
        }
        else
        {
          //Menu is beeing processed....
          uint8_t usrbtn2_pressed = digitalRead(USERBTN2);
          uint8_t usrbtn1_pressed = digitalRead(USERBTN1);
          uint8_t usrbtn0_pressed = digitalRead(USERBTN0);
          if (0 == usrbtn2_pressed && last_usrbtn2 == 1)
          {
            Menu.RenderMenu(Menus::KeyC_Pressed);
          }

          if (0 == usrbtn1_pressed && last_usrbtn1 == 1)
          {

            Menu.RenderMenu(Menus::KeyA_Pressed);
          }

          if (0 == usrbtn0_pressed && last_usrbtn0 == 1)
          {
            Menu.RenderMenu(Menus::KeyB_Pressed);
          }

          if ((usrbtn2_pressed == 0) && (usrbtn0_pressed == 0))
          {
            Menu.RenderMenu(Menus::KeyBC_Pressed);
          }

          force_refresh = true;
          last_usrbtn2 = usrbtn2_pressed;
          last_usrbtn1 = usrbtn1_pressed;
          last_usrbtn0 = usrbtn0_pressed;
          refreshtimer = 0;
          delay(100);
        }
      }
    }
}

void StartToggl( void ){
    

      xTaskCreatePinnedToCore(
      TogglTask,       /* Function to implement the task */
      "TogglTask",  /* Name of the task */
      20000,          /* Stack size in words */
      NULL,           /* Task input parameter */
      1,              /* Priority of the task */
      NULL,           /* Task handle. */
      1); 
}





