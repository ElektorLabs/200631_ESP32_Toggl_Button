#ifndef DATASTORE_H_
 #define DATASTORE_H_
 
 
#include "./src/TimeCore/timecore.h"

typedef struct {
  char ssid[128];
  char pass[128];
} credentials_t;
/* 256 byte */

typedef struct{
  bool WIFI_Ena;
  char ap_pass[128];
  bool AP_Enc_Ena;
  bool AP_Mode;
  bool NoFallbackToAP;
} wifi_param_config_t;

typedef struct{
  char ntpServerName[129];
  bool NTPEnable;
  int32_t SyncIntervall;
} ntp_config_t; 
/* 137 byte */


void write_toggl_apikey( String apikey);
String read_toggle_apikey( void );


/**************************************************************************************************
 *    Function      : datastoresetup
 *    Description   : Gets the EEPROM Emulation set up
 *    Input         : none 
 *    Output        : none
 *    Remarks       : We use 4096 byte for EEPROM 
 **************************************************************************************************/
void datastoresetup();

/**************************************************************************************************
 *    Function      : write_wifi_config
 *    Description   : writes the wifi config
 *    Input         : wifi_param_config_t c 
 *    Output        : none
 *    Remarks       : none 
 **************************************************************************************************/
void write_wifi_config(wifi_param_config_t c);


/**************************************************************************************************
 *    Function      : read_wifi_config
 *    Description   : writes the wifi config
 *    Input         : none
 *    Output        : wifi_param_config_t
 *    Remarks       : none
 **************************************************************************************************/
wifi_param_config_t read_wifi_config( void );

/**************************************************************************************************
 *    Function      : write_ntp_config
 *    Description   : writes the ntp config
 *    Input         : ntp_config_t c 
 *    Output        : none
 *    Remarks       : none 
 **************************************************************************************************/
void write_ntp_config(ntp_config_t c);


/**************************************************************************************************
 *    Function      : read_ntp_config
 *    Description   : writes the ntp config
 *    Input         : none
 *    Output        : ntp_config_t
 *    Remarks       : none
 **************************************************************************************************/
ntp_config_t read_ntp_config( void );


/**************************************************************************************************
 *    Function      : write_credentials
 *    Description   : writes the wifi credentials
 *    Input         : credentials_t
 *    Output        : none
 *    Remarks       : none
 **************************************************************************************************/
void write_credentials(credentials_t c);


/**************************************************************************************************
 *    Function      : read_credentials
 *    Description   : reads the wifi credentials
 *    Input         : none
 *    Output        : credentials_t
 *    Remarks       : none
 **************************************************************************************************/
credentials_t read_credentials( void );

/**************************************************************************************************
 *    Function      : eepwrite_notes
 *    Description   : writes the user notes 
 *    Input         : uint8_t* data, uint32_t size
 *    Output        : none
 *    Remarks       : none
 **************************************************************************************************/
void write_notes(uint8_t* data, uint32_t size);

/**************************************************************************************************
 *    Function      : eepread_notes
 *    Description   : reads the user notes 
 *    Input         : uint8_t* data, uint32_t size
 *    Output        : none
 *    Remarks       : none
 **************************************************************************************************/
void read_notes(uint8_t* data, uint32_t size);

/**************************************************************************************************
 *    Function      : write_timecoreconf
 *    Description   : writes the time core config
 *    Input         : timecoreconf_t
 *    Output        : none
 *    Remarks       : none
 **************************************************************************************************/
void write_timecoreconf(timecoreconf_t c);


/**************************************************************************************************
 *    Function      : read_timecoreconf
 *    Description   : reads the time core config
 *    Input         : none
 *    Output        : timecoreconf_t
 *    Remarks       : none
 **************************************************************************************************/
timecoreconf_t read_timecoreconf( void );

/**************************************************************************************************
 *    Function      : datastore_erase
 *    Description   : deletes the datastore 
 *    Input         : none
 *    Output        : none
 *    Remarks       : This will invalidate all user data 
 **************************************************************************************************/
void datastore_erase( void );


#endif
