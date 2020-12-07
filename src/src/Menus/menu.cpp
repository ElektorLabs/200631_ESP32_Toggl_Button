
#include "menu.h"

#define MAX_MENU_ENTRY  16 

extern void StartNewEntry( String Description);
extern void drawBmp(const char *filename, int16_t x, int16_t y);

String DescriptionArray[MAX_MENU_ENTRY];

uint8_t Menus::GetUsedEntryCount(){
    return MAX_MENU_ENTRY;
}




Menus::Menus(TFT_eSPI &lcd)
{
    _lcd = &lcd;
    CurretnMenuState = NoMenu;
    SelectedAlarmIndex = 0;
    SelectedAlarmOptionIndex = 0;
    SelectedSettingsOptions = 0;

    SelectedTimeIndex = 0;
    SelectedDateIndex = 0;

    for(uint8_t i=0;i<MAX_MENU_ENTRY;i++){
        DescriptionArray[i]=String("Empty Entry");
    }

}

void Menus::SetDescriptionEntrys( uint8_t idx , String Entry ){
    
    if(idx<MAX_MENU_ENTRY){
        DescriptionArray[idx]=Entry;
    }

}


//Returns false if in NoMenu....
bool Menus::RenderMenu(KeyAction_t key )
{
    bool redraw = false;
    bool update = false;
    switch (CurretnMenuState)
    {
        case NoMenu:
        {
            //If A is pressed we will show the AlarmList
            if (key == KeyA_Pressed)
            {
                CurretnMenuState = SettingsList;
                redraw=true;
                update=true;
            }
        }
        break;

        case SettingsList:
        {
            if (key == KeyB_Pressed) {
                if ((SelectedSettingsOptions+1) < GetUsedEntryCount()){
                    SelectedSettingsOptions++;
                    update=true;
                }
            }

            if (key == KeyC_Pressed) {
                if (SelectedSettingsOptions > 0)
                {
                    SelectedSettingsOptions--;
                    update=true;
                }
            }

            if (key == KeyA_Pressed){
                //Make new entry....
                _lcd->fillScreen(TFT_WHITE);
                drawBmp("/newtime.bmp", 64, 96); //Ugly implimentation with extern
                _lcd->setCursor(0, 0, 2);
                _lcd->setTextColor(TFT_BLACK,TFT_WHITE);  
                _lcd->setTextSize(1);
                _lcd->setTextFont(4);
                _lcd->println("Genertae new Entry");
                _lcd->println("Description:");
                _lcd->println(DescriptionArray[SelectedSettingsOptions]);
                StartNewEntry( DescriptionArray[SelectedSettingsOptions]);
                CurretnMenuState = NoMenu;
            }

            if(key==KeyBC_Pressed){
                _lcd->fillScreen(TFT_WHITE);
                CurretnMenuState = NoMenu;
            }

           

        }
        break;

        default:
        {
            CurretnMenuState = NoMenu;    //If we end here we will leave the menu
        }
        break;
    }

    if( (CurretnMenuState==SettingsList) && (update==true) ){

                ShowMenuSettingsList(SelectedSettingsOptions, redraw);
    }
    return (NoMenu == CurretnMenuState);
}

void Menus::DrawSettingsMenuEntry(uint32_t offset, const char *Name, bool selected)
{

    if (selected == true)
    {
        _lcd->fillRect(0, 40 + offset, 300, 40, _lcd->color565(92, 96, 102));
    }
    else
    {
        _lcd->fillRect(0, 40 + offset, 300, 40, TFT_WHITE);
    }
    _lcd->drawRect(0, 40 + offset, 300, 40, _lcd->color565(112, 116, 122));
    _lcd->setTextColor(TFT_BLACK, TFT_BLACK);
    _lcd->setFreeFont(FSB12);
    _lcd->setCursor(0, 65 + offset);

    if (selected == true)
    {
        _lcd->print(" >");
    }

    _lcd->setCursor(26, 70 + offset);
    _lcd->print(Name);
}

//We need to set if we have already drawn stuff to reduce flicker 
//as good as possible
void Menus::ShowMenuSettingsList(uint8_t selected_idx, bool refresh){
 
  /* Draw Headline */
  if(true == refresh){
    _lcd->fillRect(0,0,320,40,_lcd->color565( 45,47,50 ) );  
    _lcd->setTextColor(_lcd->color565( 112,116,122 ),TFT_BLACK);
    _lcd->setFreeFont(FSB18); 
    _lcd->setCursor(160- ( _lcd->textWidth("Description") / 2) ,30);
    _lcd->print("Description");
    
  } 
  /* Headline done */

  /* Draw scrollbar */
  _lcd->fillRect(303,43,15,197,TFT_WHITE);
  _lcd->drawRect(300,40,20,200,_lcd->color565( 112,116,122 ));
  _lcd->drawRect(301,41,18,198,_lcd->color565( 112,116,122 ));
  _lcd->drawRect(302,42,16,196,_lcd->color565( 112,116,122 ));
  _lcd->fillRect(305,45 +(196/GetUsedEntryCount())*selected_idx ,10,(196/GetUsedEntryCount())-2 , TFT_DARKGREY);
  /* Scrollbar done */
  


  /* Menu entries */
  
  uint8_t startidx=0;
  uint8_t endindex=0;
  
  if(selected_idx>4){ //We can display 5 Elements 
    startidx = selected_idx -4 ;
  }
  if((startidx+5)>GetUsedEntryCount()){ //Limit last drawn item
      endindex=GetUsedEntryCount();
  }else{
      endindex=startidx+5;
  }
  /*Draw up to five elements*/
  for(uint8_t i=startidx;i<endindex;i++){  
          DrawSettingsMenuEntry( (40*(i-startidx)) ,DescriptionArray[i].c_str() ,( i==selected_idx ) );
  }
  /* Menu entries drawn */
 
}