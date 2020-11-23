#include "SD.h"
#include "SPI.h"
#include "SPIFFS.h"
#include <ArduinoJson.h>

#include "sdcard_if.h"

SPIClass SDSPI(HSPI);
Timecore* SDTimeCorePtr=nullptr;
int16_t CS_Pin = -1;
bool card_eject = false;


//We need a few parameter
SemaphoreHandle_t SDCfgSem;

SemaphoreHandle_t SDCardAccessSem;


bool LogEnable = false;
uint16_t LogInterval = 15;

void SDCardDataLog( void );
void SDCardLoging( void* param);
void SdCardLog_WriteConfig( void );
void SDCardLog_ReadConfig( void );


/**************************************************************************************************
 *    Function      : SDCardRegisterTimecore
 *    Description   : Will register a timesource for logging
 *    Input         : Timecore*
 *    Output        : void 
 *    Remarks       : none
 **************************************************************************************************/
void SDCardRegisterTimecore( Timecore* TC){
  SDTimeCorePtr=TC;
}

/**************************************************************************************************
 *    Function      : setup_sdcard
 *    Description   : This will prepare the sd-card interface
 *    Input         : int8_t sd_sck_pin , int8_t sd_miso_pin, int8_t sd_mosi_pin,  int8_t sd_cs_pin
 *    Output        : void
 *    Remarks       : none
 **************************************************************************************************/
void setup_sdcard( int8_t sd_sck_pin , int8_t sd_miso_pin, int8_t sd_mosi_pin, int8_t sd_cs_pin ){
  
  SDSPI.begin(sd_sck_pin, sd_miso_pin, sd_mosi_pin, -1);
  CS_Pin=sd_cs_pin;
  card_eject = true; //Card not mounted
  SDCardAccessSem  = xSemaphoreCreateBinary();
  if(nullptr == SDCardAccessSem){
    abort();
  }
  xSemaphoreGive(SDCardAccessSem);
  SDCfgSem = xSemaphoreCreateBinary();
  if( nullptr == SDCfgSem){
    abort();
  }

}

/**************************************************************************************************
 *    Function      : sdcard_umount
 *    Description   : this will try to unmount the sd card
 *    Input         : void
 *    Output        : void
 *    Remarks       : none
 **************************************************************************************************/
void sdcard_umount(){
    if(nullptr!=SDCardAccessSem){
      if (false == xSemaphoreTake(SDCardAccessSem, portMAX_DELAY ) ){
        return;
      } 
      card_eject = true;
      SD.end();
        #ifdef DEBUG_SERIAL
        Serial.println("SD-Card ejected");
        #endif
      xSemaphoreGive(SDCardAccessSem);
    }
}

/**************************************************************************************************
 *    Function      : sdcard_mount
 *    Description   : this will try to mount the sd card
 *    Input         : void
 *    Output        : void
 *    Remarks       : none
 **************************************************************************************************/
void sdcard_mount(){
  if(nullptr!=SDCardAccessSem){
    if (false == xSemaphoreTake(SDCardAccessSem, portMAX_DELAY )){
      return;
    } 
    SD.end();
  if(false == SD.begin(CS_Pin, SDSPI) ){
      #ifdef DEBUG_SERIAL
       Serial.println("SD-Card mount failed");
      #endif
     card_eject = true;
  } else {

    #ifdef DEBUG_SERIAL
      Serial.println("SD-Card mounted at '\\SD' ");
    #endif
   
   card_eject = false;
  }
  xSemaphoreGive(SDCardAccessSem);
 }
}

/**************************************************************************************************
 *    Function      : sdcard_getmounted
 *    Description   : returns if the card is mounted
 *    Input         : void
 *    Output        : bool
 *    Remarks       : none
 **************************************************************************************************/
bool sdcard_getmounted( void ){
  if(  card_eject == true){
    return false;
  } else {
    return true;
  }
}

/**************************************************************************************************
 *    Function      : sdcard_GetCapacity
 *    Description   : Gets the disk capacity in MB
 *    Input         : void
 *    Output        : uint32_t
 *    Remarks       : none
 **************************************************************************************************/
uint32_t sdcard_GetCapacity( void ){
  uint32_t size_mb=0;
  if(SDCardAccessSem==nullptr){
    return 0;
  }
  if (false == xSemaphoreTake(SDCardAccessSem, 5)){
    #ifdef DEBUG_SERIAL
      Serial.println("SD Card Semaphore locked");
    #endif
    return 0;
  } 
  if(true == sdcard_getmounted()){
      uint64_t bytesfree = SD.totalBytes();
      size_mb = bytesfree / 1024 / 1024 ;
  }
 


  xSemaphoreGive(SDCardAccessSem);
  return size_mb;
} 

/**************************************************************************************************
 *    Function      : sdcard_GetFreeSpace
 *    Description   : Gets the free disk space in MB
 *    Input         : void
 *    Output        : uint32_t
 *    Remarks       : none
 **************************************************************************************************/
uint32_t sdcard_GetFreeSpace( void  ){
  if(SDCardAccessSem==nullptr){
    return 0;
  }
  if (false == xSemaphoreTake(SDCardAccessSem, 5)){
    #ifdef DEBUG_SERIAL
      Serial.println("SD Card Semaphore locked");
    #endif
    return 0;
  } 
  uint64_t BytesFree = SD.totalBytes() - SD.usedBytes() ;
  uint32_t MBFree = ( BytesFree / 1024 / 1024 ); 
  xSemaphoreGive(SDCardAccessSem);
  return MBFree;
}

/**************************************************************************************************
 *    Function      : sdcard_readapikey
 *    Description   : Gets apikey from sdcard
 *    Input         : void
 *    Output        : uint32_t
 *    Remarks       : none
 **************************************************************************************************/
bool sdcard_readapikey( String* apikey  ){ //Reads api key from sdcard
  bool readfile = false;
  if(SDCardAccessSem==nullptr){
    return false;
  }
  if (false == xSemaphoreTake(SDCardAccessSem, 5)){
    #ifdef DEBUG_SERIAL
      Serial.println("SD Card Semaphore locked");
    #endif
    return false;
  } 

  if(true==SD.exists("apikey.txt")){
    File key = SD.open("apikey.txt");
    *apikey = key.readString();
    key.close();
    readfile=true;

  } else {
    readfile=false;
  }

  xSemaphoreGive(SDCardAccessSem);
  return readfile;
}