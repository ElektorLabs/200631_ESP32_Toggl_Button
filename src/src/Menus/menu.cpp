
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

    //We will render into the AlarmList
    switch (key)
    {
        case KeyA_Pressed:
        {
            Serial.println("Key A");
        }
        break;

        case KeyB_Pressed:
        {
            Serial.println("Key B");
        }
        break;

        case KeyC_Pressed:
        {
            Serial.println("Key C");
        }
        break;

        case KeyAB_Pressed:{
             Serial.println("Key AB");
        }

        case KeyAC_Pressed:{
            Serial.println("Key AC");
        } break;

        case KeyBC_Pressed:{
            Serial.println("Key BC");
        } break;

        default:
        {
        
        }
        break;
        
    }
    switch (CurretnMenuState)
    {

    case NoMenu:
    {
        //If A is pressed we will show the AlarmList
        if (key == KeyA_Pressed)
        {
            CurretnMenuState = SettingsList;
            ShowMenuSettingsList(SelectedSettingsOptions, true);
        }

    }
    break;

    case SettingsList:
    {
        if (key == KeyB_Pressed)
        {
            if ((SelectedSettingsOptions+1) < GetUsedEntryCount())
            {
                SelectedSettingsOptions++;
                ShowMenuSettingsList(SelectedSettingsOptions, true);
            }
        }

        if (key == KeyC_Pressed)
        {
            if (SelectedSettingsOptions > 0)
            {
                SelectedSettingsOptions--;
                ShowMenuSettingsList(SelectedSettingsOptions, true);
            }
        }

        if (key == KeyA_Pressed)
        {
            //Make new entry....
            Serial.println("Generate new entry");
                _lcd->fillScreen(TFT_WHITE);
                drawBmp("/newtime.bmp", 64, 96);
                _lcd->setCursor(0, 0, 2);
                // Set the font colour to be white with a black background, set text size multiplier to 1
                 _lcd->setTextColor(TFT_BLACK,TFT_WHITE);  
                 _lcd->setTextSize(1);
                // We can now plot text on screen using the "print" class
                _lcd->setTextFont(4);
                _lcd->println("Genertae new Entry");
                _lcd->println("Description:");
                _lcd->println(DescriptionArray[SelectedSettingsOptions]);
              
                StartNewEntry( DescriptionArray[SelectedSettingsOptions]);
            CurretnMenuState = NoMenu;
        }

        if(key==KeyBC_Pressed){
            Serial.println("Leave menu");
            _lcd->fillScreen(TFT_WHITE);
            CurretnMenuState = NoMenu;
        }

    }
    break;

    default:
    {
        //If we end here we will leave the menu
        CurretnMenuState = NoMenu;
    }
    break;
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

  //Draw scrolling element
  //_lcd->fillRect(302,43,15,196,TFT_WHITE);
  _lcd->fillRect(303,43,15,197,TFT_WHITE);
  _lcd->drawRect(300,40,20,200,_lcd->color565( 112,116,122 ));
  _lcd->drawRect(301,41,18,198,_lcd->color565( 112,116,122 ));
  _lcd->drawRect(302,42,16,196,_lcd->color565( 112,116,122 ));
  //Next is to draw the elements as far as possibel....

  
  //We calcualte the range we need to draw or like to draw
  //We can display 5 Elements 
  uint8_t startidx=0;
  if(selected_idx>4){
    startidx = selected_idx -4 ;
  }

  //We set the scrollbar accordingly
  _lcd->fillRect(305,45 +(196/GetUsedEntryCount())*selected_idx ,10,(196/GetUsedEntryCount())-2 , TFT_DARKGREY);

  Serial.print("Selected index =");
  Serial.println(selected_idx);
  for(uint8_t i=startidx;i<GetUsedEntryCount();i++){
     
      if(i>=GetUsedEntryCount()){
        Serial.print("Menu item out of index at:");
        Serial.println(i);

      } else {
          DrawSettingsMenuEntry( (40*(i-startidx)) ,DescriptionArray[i].c_str() ,( i==selected_idx ) );
          Serial.print("Draw position ");
          Serial.print(i-startidx );
          Serial.print(" with index " );
          Serial.println(i);
      }
      
  }
 
}


/*

    DrawSettingsMenuEntry(40 * 0, "[Go Back]", (0 == selected_idx));
    DrawSettingsMenuEntry(40 * 1, "Review", (1 == selected_idx));
    DrawSettingsMenuEntry(40 * 2, "Article", (2 == selected_idx));
    DrawSettingsMenuEntry(40 * 3, "Software", (3 == selected_idx));
    DrawSettingsMenuEntry(40 * 4, "Toggl Overhead", (4 == selected_idx));
    DrawSettingsMenuEntry(40 * 5, "Entry 5", (5 == selected_idx));
    DrawSettingsMenuEntry(40 * 6, "Entry 6", (6 == selected_idx));
    DrawSettingsMenuEntry(40 * 7, "Entry 7", (7 == selected_idx));
    DrawSettingsMenuEntry(40 * 8, "Entry 8", (8 == selected_idx));
    DrawSettingsMenuEntry(40 * 8, "Entry 9", (9 == selected_idx));
    DrawSettingsMenuEntry(40 * 10, "Entry 10", (10 == selected_idx));
    DrawSettingsMenuEntry(40 * 11, "Entry 11", (11 == selected_idx));
    DrawSettingsMenuEntry(40 * 12, "Entry 12", (12 == selected_idx));
    DrawSettingsMenuEntry(40 * 13, "Entry 13", (13 == selected_idx));
    DrawSettingsMenuEntry(40 * 14, "Entry 14", (14 == selected_idx));
    DrawSettingsMenuEntry(40 * 15, "Entry 15", (15 == selected_idx));

*/