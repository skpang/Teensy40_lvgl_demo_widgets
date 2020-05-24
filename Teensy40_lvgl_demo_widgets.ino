/*
 * 
 * 
 * This demo is for the Teensy 4.0 with a ILI9488 3.5" Capacitive Touch screen.
 * http://skpang.co.uk/catalog/teensy-40-triple-can-board-with-480x320-35-touch-lcd-p-1589.html
 * 
 * Requires LVGL library v7
 * https://github.com/lvgl/lvgl
 * https://github.com/lvgl/lv_examples
 *  
 *  www.skpang.co.uk May 2020
 */
 
#include <lvgl.h>
#include <Wire.h>
#include <ILI9488_t3.h>

#include <Adafruit_FT6206.h>

#include "lv_examples.h"

#include <SPI.h>
int LED_R = 24;
int LED_B = 26;
int LED_G = 25;
int LCD_BL = 14;
int LCD_RST = 15;

int D8 = 8;

#define ON  LOW
#define OFF HIGH
#define TFT_CS 10
#define TFT_DC  9
ILI9488_t3 display = ILI9488_t3(&SPI, TFT_CS, TFT_DC);
Adafruit_FT6206 ts = Adafruit_FT6206();

#define LVGL_TICK_PERIOD 60
int screenWidth = 480;
int screenHeight = 320;

int oldTouchX = 0;
int oldTouchY = 0;

static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * LV_VER_RES_MAX / 10];   

int led = 13;
uint16_t i = 1000;

IntervalTimer tick;
static void lv_tick_handler(void)
{

  lv_tick_inc(LVGL_TICK_PERIOD);
}


/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{ 
  uint16_t width = (area->x2 - area->x1 +1);
  uint16_t height = (area->y2 - area->y1+1);

  display.writeRect(area->x1, area->y1, width, height, (uint16_t *)color_p);

  lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */
  
}

bool my_touchpad_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    uint16_t touchX, touchY;
  
    if (ts.touched())
    {   
        // Retrieve a point  
        TS_Point p = ts.getPoint(); 
    
        touchX = p.y;         // Rotate the co-ordinates
        touchY = p.x;
        touchY = 320-touchY;
  
         if ((touchX != oldTouchX) || (touchY != oldTouchY))
         {
              Serial.print("x= ");
              Serial.print(touchX,DEC);
              Serial.print(" y= ");
              Serial.println(touchY,DEC);
              
              oldTouchY = touchY;
              oldTouchX = touchX;
              data->state = LV_INDEV_STATE_PR; 
              data->point.x = touchX;
              data->point.y = touchY;
         
         }
    }else
    {
        data->point.x = oldTouchX;
        data->point.y = oldTouchY;
        data->state =LV_INDEV_STATE_REL;
     }
           
    return 0;
}

void setup() {
    pinMode(LED_R,OUTPUT);
    pinMode(LED_G,OUTPUT);
    pinMode(LED_B,OUTPUT);
    pinMode(LCD_BL,OUTPUT);
    pinMode(D8,OUTPUT);
    digitalWrite(LCD_BL, HIGH); 
    digitalWrite(D8, HIGH); 
    
    digitalWrite(LED_B, OFF);
    digitalWrite(LED_G, OFF);
    digitalWrite(LED_R, OFF);   
    digitalWrite(LED_R, ON);
    delay(200); 
    digitalWrite(LED_R, OFF);
    
    digitalWrite(LED_G, ON);
    delay(200); 
    digitalWrite(LED_G, OFF);
    
    digitalWrite(LED_B, ON);
    delay(200); 
    digitalWrite(LED_B, OFF);

    if (!ts.begin(40)) { 
        Serial.println("Unable to start touchscreen.");
    } 
    else { 
        Serial.println("Touchscreen started ok."); 
    }
  
    display.begin();
    display.fillScreen(ILI9488_BLUE);
    display.setRotation(1); 

    lv_init();
    lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * LV_VER_RES_MAX / 10);
  
   /*Initialize the display*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);
  
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);             /*Descriptor of a input device driver*/
    indev_drv.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
    indev_drv.read_cb = my_touchpad_read;      /*Set your driver function*/
    lv_indev_drv_register(&indev_drv);         /*Finally register the driver*/

    lv_demo_widgets();   // Start demo

    Serial.println("tick.begin");
    tick.begin(lv_tick_handler, LVGL_TICK_PERIOD * 1000);  // Start ticker
}

void loop() {

   lv_task_handler(); /* let the GUI do its work */
   delay(5);

}
