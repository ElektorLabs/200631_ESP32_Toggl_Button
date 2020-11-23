
#ifndef __MENUS_H__
 #define __MENUS_H__
//This will handle the menus, rendering and user inputs
#include "TFT_eSPI.h"
#include "Free_Fonts.h"


//We need a refference to the alarms
class Menus{

    public:
    
    //This are for Keyactions, we define Key A to B 
    typedef enum {
        NothingPressed =0,
        KeyA_Pressed,
        KeyA_Released,
        KeyB_Pressed,
        KeyB_Released,
        KeyC_Pressed,
        KeyC_Released,
        KeyAB_Pressed,
        KeyAB_Released,
        KeyAC_Pressed,
        KeyAC_Released,
        KeyBC_Pressed,
        KeyBC_Released
    }KeyAction_t;

    

    //Requiered element for the 
    Menus( TFT_eSPI& lcd);
    bool RenderMenu( KeyAction_t key  ); //Entrypoint for the Menu
    void SetDescriptionEntrys( uint8_t idx , String Entry );
    
    

    
    private:

    typedef enum{
        NoMenu=0,
        SettingsList,
        MenuTimeSettings,
        MenuDateSettings,
        MenuAlarmList,
        MenuAlarmSetting
    } MenuState_t;

    
    void DrawSettingsMenuEntry( uint32_t offset, const char* Name,  bool selected );
    void ShowMenuSettingsList( uint8_t selected_idx, bool refresh );
    uint8_t GetUsedEntryCount( void );
    
    

    

    TFT_eSPI* _lcd;         //Requiered for rendering
    

    MenuState_t CurretnMenuState;
    uint8_t SelectedAlarmIndex;

    uint8_t SelectedAlarmOptionIndex;
    uint8_t SelectedSettingsOptions;

    uint8_t SelectedTimeIndex;
    uint8_t SelectedDateIndex;
    
    
    
};

#endif