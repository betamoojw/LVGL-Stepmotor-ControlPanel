/**
/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// Example CYD  ESP32-2432S028R LVGL STEPMOTOR SLIDER                      //
// Design UI on Squareline Studio. LVGL V9.1                               //
// Youtube:https://www.youtube.com/@pangcrd                                //
// Github: https://github.com/pangcrd                                      //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
**/
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include "lvgl.h"
#include "ui.h"
#include <HardwareSerial.h>
#include <ArduinoJson.h>

HardwareSerial mySerial(1);  // UART1

static float maxDistance = 300; // Khoảng cách tối đa của hệ trượt
bool HomePressed = false; 

typedef struct MotorData {

  float distance;
  int count;
  float pos;
  bool reset;
  bool home;

}  MotorData;

MotorData Data;


/*Don't forget to set Sketchbook location in File/Preferences to the path of your UI project (the parent foder of this INO file)*/
/*Change to your screen resolution*/
static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 240;

enum { SCREENBUFFER_SIZE_PIXELS = screenWidth * screenHeight / 10 };
static lv_color_t buf [SCREENBUFFER_SIZE_PIXELS];

TFT_eSPI tft = TFT_eSPI( screenWidth, screenHeight ); /* TFT instance */
/*Touch screen config*/
#define XPT2046_IRQ 36 //GPIO touch driver 
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33
SPIClass tsSpi = SPIClass(VSPI);
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);
//Run calib_touch files to get value 
uint16_t touchScreenMinimumX = 200, touchScreenMaximumX = 3700, touchScreenMinimumY = 240,touchScreenMaximumY = 3800; //Chạy Calibration để lấy giá trị mỗi màn hình mỗi khác

/* Display flushing */
void my_disp_flush (lv_display_t *disp, const lv_area_t *area, uint8_t *pixelmap)
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    if (LV_COLOR_16_SWAP) {
        size_t len = lv_area_get_size( area );
        lv_draw_sw_rgb565_swap( pixelmap, len );
    }

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( (uint16_t*) pixelmap, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp );
}

/*========== Read the encoder rotary ==========*/
void my_touch_read (lv_indev_t *indev_drv, lv_indev_data_t * data)
{
    if(ts.touched())
    {
        TS_Point p = ts.getPoint();
        //Some very basic auto calibration so it doesn't go out of range
        if(p.x < touchScreenMinimumX) touchScreenMinimumX = p.x;
        if(p.x > touchScreenMaximumX) touchScreenMaximumX = p.x;
        if(p.y < touchScreenMinimumY) touchScreenMinimumY = p.y;
        if(p.y > touchScreenMaximumY) touchScreenMaximumY = p.y;
        //Map this to the pixel position
        data->point.x = map(p.x,touchScreenMinimumX,touchScreenMaximumX,1,screenWidth); /* Touchscreen X calibration */
        data->point.y = map(p.y,touchScreenMinimumY,touchScreenMaximumY,1,screenHeight); /* Touchscreen Y calibration */
        data->state = LV_INDEV_STATE_PR;

        // Serial.print( "Touch x " );
        // Serial.print( data->point.x );
        // Serial.print( " y " );
        // Serial.println( data->point.y );
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
}

/*Set tick routine needed for LVGL internal timings*/
static uint32_t my_tick_get_cb (void) { return millis(); }
/** Send Motor Data Json [Option 1] */
// void SendMotorData(){

//     JsonDocument motorData;

//     motorData["type"] = "motordata";  // Add a type to distinguish.
//     motorData["distance"] = Data.distance;
//     motorData["Count"] = Data.count;
    

//     String output;
//     serializeJson(motorData, output);
//     mySerial.println(output);

// }

// void sendButtonState(){

//     JsonDocument btnState;

//     btnState["type"] = "button";  
//     btnState["Reset"] = Data.reset;
//     btnState["Home"] = Data.home;

//     String output;
//     serializeJson(btnState, output);
//     mySerial.println(output);


// }
/** Option 2 */
void SendData(const String &type) {

    JsonDocument data;

    data["type"] = type;  // check data types.


    if (type == "motordata") {
        data["distance"] = Data.distance;
        data["Count"] = Data.count;
    } 
    else if (type == "ResetBTN") {
        data["Reset"] = Data.reset;
        
    }
    else if (type == "HomeBTN") {
        data["Home"] = Data.home;
    }

    String output;
    serializeJson(data, output);
    mySerial.println(output);
}
/** ///////////////////////////////////// */
/** Received Data from Slave */
void ReceiveMotorData2() {

    while (mySerial.available()) { 
        char input[128];  // Buffer large enough to hold a JSON.
        int len = mySerial.readBytesUntil('}', input, sizeof(input) - 2);
        input[len] = '}'; // Close JSON
        input[len + 1] = '\0'; // Finished a JSON

        //Serial.print("Recv data: "); Serial.println(input);

        JsonDocument jsonRecvData;
        DeserializationError error = deserializeJson(jsonRecvData, input);

        if (!error) {
            String type = jsonRecvData["type"].as<String>();
            if (type == "posdata") {    
                Data.pos = jsonRecvData["position"];
                Serial.print("Pos = "); Serial.println(Data.pos);

                String PosString = String(Data.pos) + " mm";
                lv_label_set_text(ui_Label5, PosString.c_str());

                int barValue = map(Data.pos, 0, maxDistance, 0, 100); 
                lv_bar_set_value(ui_Bar1, barValue, LV_ANIM_ON);
            }
        }
    }
}
/** /////////////////////////////////////////// */
/**  */
void ControlPanel (lv_timer_t *timer){

    if (startNexit_flag) {

        Serial.print("Final Distance: ");
        Serial.println(setDistance);
        Serial.print("Final Count: ");
        Serial.println(setCount);
        startNexit_flag = false;

        Data.distance = setDistance;  
        Data.count = setCount;


        char buffer[60]; 
        snprintf(buffer, sizeof(buffer), "Goto Distance: %.2fmm \nCount: %d", setDistance, setCount);
        lv_textarea_set_text(ui_TextArea1, buffer);

        //SendMotorData();
        SendData("motordata");
    }


    if (HomeState) {
        Data.home = HomeState;
        HomeState = false;
        
        //sendButtonState()
        SendData("HomeBTN");
    }
    
    if (resetState) {
        Data.reset = resetState;
        resetState = false;
        ///Serial.println(Data.reset);
        //sendButtonState();
        SendData("ResetBTN");
    }

    ReceiveMotorData2();

}
void setup (){

    Serial.begin( 115200 );
    mySerial.begin(115200, SERIAL_8N1, 22, 27); // tx, rx
    
    delay(10);
    lv_init();

    //Initialise the touchscreen
    tsSpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS); /* Start second SPI bus for touchscreen */
    ts.begin(tsSpi);      /* Touchscreen init */
    ts.setRotation(3);   /* Inverted landscape orientation to match screen */

    tft.begin();         /* TFT init */
    tft.setRotation(3); /* Landscape orientation, flipped */
                                             

    

    static lv_disp_t* disp;
    disp = lv_display_create( screenWidth, screenHeight );
    lv_display_set_buffers( disp, buf, NULL, SCREENBUFFER_SIZE_PIXELS * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL );
    lv_display_set_flush_cb( disp, my_disp_flush );

    //Initialize the Rotary Encoder input device. For LVGL version 9+ only
    lv_indev_t *touch_indev = lv_indev_create();
    lv_indev_set_type(touch_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(touch_indev, my_touch_read);


    lv_tick_set_cb( my_tick_get_cb );

    ui_init();

    lv_timer_t *btnTimer = lv_timer_create(ControlPanel, 50, NULL );

    Serial.println( "Setup done" );
}


void loop ()
{   
    
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);
    
}
