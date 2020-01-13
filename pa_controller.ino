#include <lvgl.h>
#include <Ticker.h>
#include <TFT_eSPI.h>
#include <ESP32CAN.h>
#include <CAN_config.h>
extern "C"{
  #include "main_ui.h"
}
#define LVGL_TICK_PERIOD 20

/* Pins */
const int tempPin = 12;
const int voltPin = 34;
const int pwrPin  = 35;
const int refPin  = 36;

const int relay1  = 32;
const int relay2  = 33;

const int alarm_in  = 7;
const int alarm_out = 8;

const int rot_a     = 9;
const int rot_b     = 10;
const int but       = 11;
Ticker tick; /* timer for interrupt handler */
TFT_eSPI tft = TFT_eSPI(); /* TFT instance */
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];
CAN_device_t CAN_cfg; //can bus object
CAN_frame_t rx_frame;
CAN_frame_t tx_frame;

word outputvoltage = 4800; //variable for charger voltage setpoint, set startup voltage to 47,6V (offset = 0,01)
word outputcurrent = 255; //variable for charger current setpoint, set startup current to 25,5A (offset = 0,1)
word overvoltage = 5050;

unsigned char voltamp1[8] = {lowByte(outputcurrent), highByte(outputcurrent), lowByte(v_out), highByte(v_out), lowByte(v_out), highByte(v_out),lowByte(overvoltage), highByte(overvoltage)};


unsigned char serialnr[8] = {0x12,0x35,0x71,0x10,0x16,0x79,0x00,0x00}; //Flatpack Serial
unsigned long int login_ID = 0x05004804;
unsigned long int send_ID = 0x05FF4004;

int pv_tempin; //Temperature in from flatpack
int pv_tempout; //Temperature out from flatpack
int pv_inputvoltage; //input voltage from flatpack
float pv_voltage; //output voltage from flatpack
float pv_current; //output current from flatpack


#if USE_LV_LOG != 0
/* Serial debugging */
void my_print(lv_log_level_t level, const char * file, uint32_t line, const char * dsc)
{

  Serial.printf("%s@%d->%s\r\n", file, line, dsc);
  delay(100);
}
#endif

void setVoltage(int t_voltage) { //can be used to set desired voltage to i.e. 80% SOC

  if(t_voltage >= 4750.0 && t_voltage <= 4860.0){ //makes sure voltage is in range 

    outputvoltage = t_voltage;

  }

 }


void setCurrent(int t_current) { //can be used to reduce or adjust charging speed

  if(t_current >= 0.0 && t_current <= 420.0){ //makes sure current is in range of psu

    outputcurrent = t_current;

  }

}

/* Display flushing */
void canWrite(unsigned char message_data[8], long int ID){

  tx_frame.FIR.B.FF = CAN_frame_ext;
  tx_frame.MsgID = ID;
  tx_frame.FIR.B.DLC = 8;

  for(int i = 0; i < 8; i++){

    tx_frame.data.u8[i] = message_data[i];

  }

  ESP32Can.CANWriteFrame(&tx_frame);

  //Serial.println("Packet sent");

}


void canRead(){

  if(xQueueReceive(CAN_cfg.rx_queue,&rx_frame, 3*portTICK_PERIOD_MS)==pdTRUE){


      switch (rx_frame.MsgID) {

        case 0x05009888: //if CANID = 0500xxyy where xxyy the last 2 digits of the serial nr
        case 0x05014400:{ //this is the request from the Flatpack rectifier during walk-in (start-up) or normal operation when no log-in response has been received for a while (ID1 or ID2)

          canWrite(serialnr, login_ID);

        }break;

        case 0x05014004:                          //01 ID of  Flatpacks in Header
        case 0x05014008:
        case 0x05014010:
        case 0x0501400C:{

          //Serial.println("Daten gelesen");
          pv_tempin = rx_frame.data.u8[0]; //Inlet temperature is byte 0 in the status message
          pv_tempout = rx_frame.data.u8[7]; //Outlet temperature is byte 7 in the status message
          pv_inputvoltage = 256*rx_frame.data.u8[6]+rx_frame.data.u8[5]; //Input voltage is byte 6 (highbyte) and byte 5 (lowbyte) in the status message
          pv_voltage = (0.01*(256*rx_frame.data.u8[4]+rx_frame.data.u8[3])); //Output voltage is byte 4 (highbyte) and 3 (lowbyte) in the status message
          pv_current = (0.1*(256*rx_frame.data.u8[2]+rx_frame.data.u8[1])); //Output current is byte 2 (highbyte) and 1 (lowbyte) in the status message

     
        }

      }

  }

}
void readTimer() { 

  canRead();

}

void loginTimer() { 

  canWrite(serialnr, login_ID); //Login every 5 seconds

}

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint16_t c;

  tft.startWrite(); /* Start new TFT transaction */
  tft.setAddrWindow(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1)); /* set the working window */
  for (int y = area->y1; y <= area->y2; y++) {
    for (int x = area->x1; x <= area->x2; x++) {
      c = color_p->full;
      tft.writeColor(c, 1);
      color_p++;
    }
  }
  tft.endWrite(); /* terminate TFT transaction */
  lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */
}

/* Interrupt driven periodic handler */
static void lv_tick_handler(void)
{

  lv_tick_inc(LVGL_TICK_PERIOD);
}

/* Reading input device (simulated encoder here) */
bool read_encoder(lv_indev_drv_t * indev, lv_indev_data_t * data)
{
  static int32_t last_diff = 0;
  int32_t diff = 0; /* Dummy - no movement */
  int btn_state = LV_INDEV_STATE_REL; /* Dummy - no press */

  data->enc_diff = diff - last_diff;;
  data->state = btn_state;

  last_diff = diff;

  return false;
}

void setup() {

  Serial.begin(115200); /* prepare for possible serial debug */
  CAN_cfg.speed=CAN_SPEED_125KBPS; //125kbit ist der richtige Wert!
  CAN_cfg.tx_pin_id = GPIO_NUM_5;
  CAN_cfg.rx_pin_id = GPIO_NUM_35;
  CAN_cfg.rx_queue = xQueueCreate(1,sizeof(CAN_frame_t));

  ESP32Can.CANInit(); //start CAN Module

  delay(50);

  lv_init();

#if USE_LV_LOG != 0
  lv_log_register_print(my_print); /* register print function for debugging */
#endif

  tft.begin(); /* TFT init */
  tft.setRotation(1); /* Landscape orientation */

  lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

  /*Initialize the display*/
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = 320;
  disp_drv.ver_res = 240;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);


  /*Initialize the touch pad*/
  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_ENCODER;
  indev_drv.read_cb = read_encoder;
  lv_indev_drv_register(&indev_drv);

  /*Initialize the graphics library's tick*/
  tick.attach_ms(LVGL_TICK_PERIOD, lv_tick_handler);

  /* Create simple label */
 ui_create();
}


void loop() {

  lv_task_handler(); /* let the GUI do its work */
  delay(5);
}
