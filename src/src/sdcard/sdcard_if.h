#include "../TimeCore/TimeCore.h"

/**************************************************************************************************
 *    Function      : setup_sdcard
 *    Description   : This will prepare the sd-card interface
 *    Input         : int8_t sd_sck_pin , int8_t sd_miso_pin, int8_t sd_mosi_pin,  int8_t sd_cs_pin
 *    Output        : void
 *    Remarks       : none
 **************************************************************************************************/
void setup_sdcard( int8_t sd_sck_pin , int8_t sd_miso_pin, int8_t sd_mosi_pin,  int8_t sd_cs_pin );

/**************************************************************************************************
 *    Function      : sdcard_mount
 *    Description   : this will try to mount the sd card
 *    Input         : void
 *    Output        : void
 *    Remarks       : none
 **************************************************************************************************/
void sdcard_mount( void );


/**************************************************************************************************
 *    Function      : sdcard_umount
 *    Description   : this will try to unmount the sd card
 *    Input         : void
 *    Output        : void
 *    Remarks       : none
 **************************************************************************************************/
void sdcard_umount( void );

/**************************************************************************************************
 *    Function      : sdcard_getmounted
 *    Description   : returns if the card is mounted
 *    Input         : void
 *    Output        : bool
 *    Remarks       : none
 **************************************************************************************************/
bool sdcard_getmounted( void );

/**************************************************************************************************
 *    Function      : sdcard_GetCapacity
 *    Description   : Gets the disk capacity in MB
 *    Input         : void
 *    Output        : uint32_t
 *    Remarks       : none
 **************************************************************************************************/
uint32_t sdcard_GetCapacity( void ); //Capacity in MB

/**************************************************************************************************
 *    Function      : sdcard_GetFreeSpace
 *    Description   : Gets the free disk space in MB
 *    Input         : void
 *    Output        : uint32_t
 *    Remarks       : none
 **************************************************************************************************/
uint32_t sdcard_GetFreeSpace( void  ); //FreeSpace in MB

/**************************************************************************************************
 *    Function      : sdcard_readapikey
 *    Description   : Gets apikey from sdcard
 *    Input         : void
 *    Output        : uint32_t
 *    Remarks       : none
 **************************************************************************************************/
bool sdcard_readapikey( String* apikey  ); //Reads api key from sdcard



/**************************************************************************************************
 *    Function      : SDCardRegisterTimecore
 *    Description   : Will register a timesource for logging
 *    Input         : Timecore*
 *    Output        : void 
 *    Remarks       : none
 **************************************************************************************************/
void SDCardRegisterTimecore( Timecore* TC);

