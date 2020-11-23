#ifndef __TOGLECLIENT_H__
 #define __TOGLECLIENT_H__

typedef struct {
    uint32_t id;
    String description;
    uint32_t starttime;
} Toggl_Element_t;

void ToggleSetRooCA( void );
int8_t TogglGetCurrentEntry( String apikey, Toggl_Element_t* Element, bool* has_entry );
int8_t ToggleStopCurrentEntry( String apikey);
int8_t ToggleStartNewEntry( String apikey , String description);


#endif