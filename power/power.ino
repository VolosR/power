#include <TFT_eSPI.h>
#include <ESP32Time.h>
#include "driver/temp_sensor.h"
#include "esp_clk.h"
#include "midleFont.h"
#include "bigFont.h"
#define VREF 1100 

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

unsigned short grays[13];

//mhz
int mhz=240;

// battery
float voltage=12.34;
ESP32Time rtc(0); 

int deb=0;
int bri[8]={255,230,200,160,120,80,40,20};
int b=0;

unsigned long sta=0;
unsigned long end=0;

long fps=0;
float tmp=0;
uint32_t cpu=0; 
int PPgraph[24]={0};

void initTempSensor(){
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    temp_sensor.dac_offset = TSENS_DAC_L2;  // TSENS_DAC_L2 is default; L4(-40°C ~ 20°C), L2(-10°C ~ 80°C), L1(20°C ~ 100°C), L0(50°C ~ 125°C)
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();
}

void setup() {

  pinMode(15,OUTPUT);
  digitalWrite(15,1);
  pinMode(14,INPUT_PULLUP);
  initTempSensor();
  
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  sprite.createSprite(300,150);

     ledcSetup(1, 10000, 8);
     ledcAttachPin(38, 1);
     ledcWrite(1, bri[b]);
   
    rtc.setTime(30, 24, 15, 23, 10, 2023,0);   

    int co=220;
     for(int i=0;i<13;i++)
     {grays[i]=tft.color565(co, co, co);
     co=co-20;}  
}


void draw()
  {
     sprite.fillSprite(TFT_BLACK);

     sprite.fillRect(0,30,100,2,0x786E);
     sprite.fillRect(200,30,100,2,0x786E);

      sprite.fillRect(0,120,80,2,0x0ACF);
     sprite.fillRect(220,120,80,2,0x0ACF);

    for(int j=0;j<24;j++)
    for(int i=0;i<PPgraph[j];i++)
    sprite.fillRect(104+(j*4),147-(i*4),3,3,grays[2]);

     sprite.loadFont(midleFont);
     sprite.setTextDatum(0);
    sprite.setTextColor(grays[3],TFT_BLACK);
    
     sprite.drawString(String(rtc.getTime()),0,127,4);
     sprite.drawString(String(voltage)+" V",0,0,4);
     sprite.drawString(String(cpu/1000000)+" Mhz",0,40,4);
     sprite.drawString(String(bri[b]),260,0,4);
     sprite.drawString(String(tmp)+" C",230,127,4);
     sprite.setTextDatum(4);
      sprite.setTextColor(0x9402,TFT_BLACK);
     sprite.drawString("FPS",150,38);
     sprite.unloadFont();
      sprite.loadFont(bigFont);
      sprite.setTextColor(grays[0],TFT_BLACK);
      sprite.drawString(String(fps),150,82,7);
      sprite.unloadFont();
     sprite.pushSprite(10,10);
  }

void loop() {

sta=millis();

   int adc = analogRead(4);
   voltage = ((float)adc / 4095.0) * 2.0 * 3.3 * (VREF / 1000.0);

  temp_sensor_read_celsius(&tmp);

    for(int i=0;i<24;i++)
    PPgraph[i]=random(1,12);
  
  if(digitalRead(14)==0){
  if(deb==0) {deb=1; b++; if(b==8) b=0; ledcWrite(1, bri[b]);}
  }else deb=0;

  cpu = esp_clk_cpu_freq();
  draw();
  
  end=millis();
  fps=1000.00/(end-sta);
}

