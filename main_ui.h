/**
 * @file mainui.h
 *
 */


#ifndef _MAIN_UI_h
#define _MAIN_UI_h

/*********************
 *      INCLUDES
 *********************/

#include <lvgl.h>
#include <Arduino.h> //needed for Serial.println
#include <string.h> //needed for memcpy



/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_group_t *g;

/**
 * Create main ui
 */
void ui_create(lv_group_t* g);

/**********************
 *      MACROS
 **********************/
#endif
