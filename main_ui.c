/**
 * @file main_ui.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "main_ui.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_obj_t *fwdPower_bar;
static lv_obj_t *refPower_bar;
static lv_obj_t *current_bar;
static lv_obj_t *voltage_bar;
static lv_obj_t *temp_bar;

static lv_obj_t * close_btn;
//

static lv_obj_t *temp_label;
static lv_obj_t *swr_label;
static lv_obj_t *paVolt_label;
static lv_obj_t *paCurrent_label;



//static lv_group_t* g; //An Object Group

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
static void event_handler(lv_obj_t * obj, lv_event_t event)
{
   uint32_t id = lv_obj_get_user_data(obj);

  
    if(event == LV_EVENT_CLICKED) {
        //printf("Clicked\n");
        lv_ex_win_1();
    }
    else if(event == LV_EVENT_VALUE_CHANGED) {
        printf("Toggled\n");
    }
}
static void scr2_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        //printf("Clicked\n");
        ui_settings_create();
    }
    else if(event == LV_EVENT_VALUE_CHANGED) {
        printf("Toggled\n");
    }
}
void lv_ex_win_1(void)
{
    /*Create a window*/
    lv_obj_t * win = lv_win_create(lv_scr_act(), NULL);
    lv_win_set_title(win, "Settings");                        /*Set the title*/

    close_btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE);           /*Add close button and use built-in close action*/
    lv_group_add_obj (g, close_btn);

    lv_obj_set_event_cb(close_btn, lv_win_close_event_cb);
  //  lv_win_add_btn(win, LV_SYMBOL_SETTINGS);        /*Add a setup button*/

    lv_obj_t * v_label = lv_label_create(win, NULL);
    lv_label_set_text(v_label, "Eltek Voltage Offset");
    lv_obj_t * v_spinbox;
    v_spinbox = lv_spinbox_create(win, NULL);
    lv_spinbox_set_digit_format(v_spinbox, 2, 2);
    lv_spinbox_step_prev(v_spinbox);
    lv_obj_set_width(v_spinbox, 100);
    lv_group_add_obj (g, v_spinbox);
    lv_obj_set_event_cb(v_spinbox, event_handler);
    lv_obj_set_user_data(v_spinbox, 2);
    lv_obj_set_pos (v_spinbox, 5, 30);
    lv_obj_t * att_label = lv_label_create(win, NULL);
    lv_label_set_text(att_label, "Alarm on ATT Overheat");
    lv_obj_set_pos (att_label, 5, 60);
    lv_obj_t *sw1 = lv_sw_create(win, NULL);
    lv_sw_on(sw1, LV_ANIM_ON);
    lv_obj_set_pos (sw1, 5, 80);
    lv_group_add_obj (g, sw1);


}

void ui_create(lv_group_t* g)
{
     lv_obj_t * scr = lv_obj_create(NULL, NULL);
    lv_coord_t hres = lv_disp_get_hor_res(NULL);
    lv_coord_t vres = lv_disp_get_ver_res(NULL);

    static lv_style_t style_tv_btn_bg;
    lv_style_copy(&style_tv_btn_bg, &lv_style_plain);
    style_tv_btn_bg.body.main_color = lv_color_hex(0x487fb7);
    style_tv_btn_bg.body.grad_color = lv_color_hex(0x487fb7);
    style_tv_btn_bg.body.padding.top = 0;
    style_tv_btn_bg.body.padding.bottom = 0;

    static lv_style_t style_tv_btn_rel;
    lv_style_copy(&style_tv_btn_rel, &lv_style_btn_rel);
    style_tv_btn_rel.body.opa = LV_OPA_TRANSP;
    style_tv_btn_rel.body.border.width = 0;

    static lv_style_t style_tv_btn_pr;
    lv_style_copy(&style_tv_btn_pr, &lv_style_btn_pr);
    style_tv_btn_pr.body.radius = 0;
    style_tv_btn_pr.body.opa = LV_OPA_50;
    style_tv_btn_pr.body.main_color = LV_COLOR_WHITE;
    style_tv_btn_pr.body.grad_color = LV_COLOR_WHITE;
    style_tv_btn_pr.body.border.width = 0;
    style_tv_btn_pr.text.color = LV_COLOR_GRAY;

 
  temp_label= lv_label_create (lv_scr_act (), NULL);
  lv_label_set_text (temp_label, "Temp: -- C");
  lv_obj_set_pos (temp_label, 5, 5);
  
  // SWR Label
  swr_label= lv_label_create (lv_scr_act (), NULL);
  lv_label_set_text (swr_label, "SWR: -.- ");
  lv_obj_set_pos (swr_label, 244, 5);
  
  //
  static lv_style_t style_bar;
  static lv_style_t style_indic,style_indic_red;

  lv_style_copy (&style_bar, &lv_style_pretty);
  style_bar.body.main_color = LV_COLOR_BLACK;
  style_bar.body.grad_color = LV_COLOR_GRAY;

  style_bar.body.border.color = LV_COLOR_WHITE;

  lv_style_copy (&style_indic, &lv_style_pretty);
  style_indic.body.grad_color = LV_COLOR_GREEN;
  style_indic.body.main_color = LV_COLOR_LIME;

  style_indic.body.shadow.color = LV_COLOR_LIME;
 
  lv_style_copy (&style_indic_red, &lv_style_pretty);
  style_indic_red.body.grad_color = LV_COLOR_RED;
  style_indic_red.body.main_color = LV_COLOR_RED;

  style_indic_red.body.shadow.color = LV_COLOR_RED;

//
  lv_obj_t * fwdStatic_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_recolor(fwdStatic_label, true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(fwdStatic_label, LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
  lv_label_set_text(fwdStatic_label, "#66cd00 FWD: ---W#");
  lv_obj_set_width(fwdStatic_label, 150);
  lv_obj_set_pos (fwdStatic_label, 5, 30);

//

  lv_obj_t * refStatic_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_recolor(refStatic_label, true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(refStatic_label, LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
  lv_label_set_text(refStatic_label, "#e70100 REF: ---W#");
  lv_obj_set_width(refStatic_label, 150);
  lv_obj_set_pos (refStatic_label, 160, 30);

//

  fwdPower_bar = lv_bar_create (lv_scr_act (), NULL);
  lv_obj_set_size (fwdPower_bar, 150, 20);
  lv_bar_set_value (fwdPower_bar, 70,LV_ANIM_ON);
  lv_bar_set_style (fwdPower_bar, LV_BAR_STYLE_BG, &style_bar);
  lv_bar_set_range (fwdPower_bar, 0, 100);
  lv_bar_set_style (fwdPower_bar, LV_BAR_STYLE_INDIC, &style_indic);
  lv_obj_set_pos (fwdPower_bar, 5,50);
  
  refPower_bar = lv_bar_create (lv_scr_act (), NULL);
  lv_obj_set_size (refPower_bar, 150, 20);
  lv_bar_set_value (refPower_bar, 12,LV_ANIM_ON);
  lv_bar_set_style (refPower_bar, LV_BAR_STYLE_BG, &style_bar);
  lv_bar_set_range (refPower_bar, 0, 100);
  lv_bar_set_style (refPower_bar, LV_BAR_STYLE_INDIC, &style_indic_red);
  lv_obj_set_pos (refPower_bar, 160, 50);
  //
  lv_obj_t * currentStatic_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_recolor(currentStatic_label, true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(currentStatic_label, LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
  lv_label_set_text(currentStatic_label, "#e6e700 Current: --A#");
  lv_obj_set_width(currentStatic_label, 140);
  lv_obj_set_pos (currentStatic_label, 120, 80);

//
  current_bar = lv_bar_create (lv_scr_act (), NULL);
  lv_obj_set_size (current_bar, 310, 20);
  lv_bar_set_value (current_bar, 12,LV_ANIM_ON);
  lv_bar_set_style (current_bar, LV_BAR_STYLE_BG, &style_bar);
  lv_bar_set_range (current_bar, 0, 100);
  lv_bar_set_style (current_bar, LV_BAR_STYLE_INDIC, &style_indic_red);
  lv_obj_set_pos (current_bar, 5, 100);
 
//
  lv_obj_t * voltageStatic_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_recolor(voltageStatic_label, true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(voltageStatic_label, LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
  lv_label_set_text(voltageStatic_label, "#e77500 Voltage: --V#");
  lv_obj_set_width(voltageStatic_label, 140);
  lv_obj_set_pos (voltageStatic_label, 120, 130);

//
  voltage_bar = lv_bar_create (lv_scr_act (), NULL);
  lv_obj_set_size (voltage_bar, 310, 20);
  lv_bar_set_value (voltage_bar, 12,LV_ANIM_ON);
  lv_bar_set_style (voltage_bar, LV_BAR_STYLE_BG, &style_bar);
  lv_bar_set_range (voltage_bar, 0, 100);
  lv_bar_set_style (voltage_bar, LV_BAR_STYLE_INDIC, &style_indic_red);
  lv_obj_set_pos (voltage_bar, 5, 150);
//
  lv_obj_t * ptt_label;
  lv_obj_t * ptt = lv_btn_create(lv_scr_act(), NULL);
  ptt_label = lv_label_create(ptt, NULL);
  lv_label_set_text(ptt_label, "PTT");
  lv_obj_set_pos (ptt, 5, 180);
  lv_obj_set_size (ptt, 100, 50);
  lv_group_add_obj (g, ptt);

//
  lv_obj_t * alarm_label;
  lv_obj_t * alarm = lv_btn_create(lv_scr_act(), NULL);
  alarm_label = lv_label_create(alarm, NULL);
  lv_label_set_text(alarm_label, "NO ALRM");
  lv_obj_set_pos (alarm, 110, 180);
  lv_obj_set_size (alarm, 100, 50);
  lv_group_add_obj (g, alarm);

  //
  lv_obj_t * oper_label;
  lv_obj_t * oper = lv_btn_create(lv_scr_act(), NULL);
  oper_label = lv_label_create(oper, NULL);
  lv_label_set_text(oper_label, "SET");
  lv_obj_set_pos (oper, 215, 180);
  lv_obj_set_size (oper, 100, 50);
  lv_group_add_obj (g, oper);
  lv_obj_set_event_cb(oper, event_handler);

/*

  lv_obj_t * voltageStatic_label = voltageStatic_label(lv_scr_act(), NULL);
  lv_label_set_recolor(voltageStatic_label, true);                    
  lv_label_set_align(voltageStatic_label, LV_LABEL_ALIGN_CENTER);       
  lv_label_set_text(voltageStatic_label, "#e70100 REF: 100W#");
  lv_obj_set_width(voltageStatic_label, 150);
  lv_obj_set_pos (voltageStatic_label, 160, 30);

*/
}
