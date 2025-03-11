#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#define MINPRESSURE 200
#define MAXPRESSURE 1000

const int XP = 8, XM = A2, YP = A3, YM = 9; 
const int TS_LEFT = 907, TS_RT = 136, TS_TOP = 942, TS_BOT = 139;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
Adafruit_GFX_Button touch_btn;

int pixel_x, pixel_y;
bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
        pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width());
        pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
    }
    return pressed;
}

#define BLACK   0x0000
#define WHITE   0xFFFF
#define CYAN    0x07FF
#define GREEN   0x07E0

void setup(void)
{
    Serial.begin(9600);
    uint16_t ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);
    
    tft.begin(ID);
    tft.setRotation(0);
    tft.fillScreen(BLACK);
    
    touch_btn.initButton(&tft, 120, 200, 140, 50, WHITE, CYAN, BLACK, "Bana Dokun", 2);
    touch_btn.drawButton(false);
}

void loop(void)
{
    bool down = Touch_getXY();
    touch_btn.press(down && touch_btn.contains(pixel_x, pixel_y));
    
    if (touch_btn.justReleased())
        touch_btn.drawButton();
    
    if (touch_btn.justPressed()) {
        touch_btn.drawButton(true);
        tft.setCursor(80, 260);
        tft.setTextColor(GREEN);
        tft.setTextSize(2);
        tft.print("Dokunuldu!");
    }
}
