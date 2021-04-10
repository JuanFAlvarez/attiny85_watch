#include <JC_Button.h>
#include <TinyI2CMaster.h>
#include <Tiny4kOLED.h>

int mins = 0;  //min
int hrs = 0;   //hour
int wkdy = 0;  //weekday
int dy= 0;    //monthday
int mth = 0;
int yr = 0;   //year

const int RTCaddress=0x68;

bool isabove12;
//times
int timepressed=0;
int timepassed=0;
int time2wait=0;
int keypress=0;
int showTime=0;

//define button pins
const byte BUTTON1_PIN(3);
const byte BUTTON2_PIN(4);
//define button states
Button btn1(BUTTON1_PIN),
             btn2(BUTTON2_PIN);


void SetClock (int hr, int mi) { //comment out dates after first upload
  TinyI2C.start(RTCaddress, 0);//0 for all 4 for hr min
  TinyI2C.write(0);
  TinyI2C.write(0);
  TinyI2C.write(mi);
  TinyI2C.write(hr);
  TinyI2C.write(1); //day of week Mon-Sun -> 1-7 Datasheet: Day
  TinyI2C.write(0x29); // day month 01-31                   Date
  TinyI2C.write(0x12); //month    01-12                     Month
  TinyI2C.write(0x20); //year        00-99                  Year
  TinyI2C.stop();
}




void setup() {  
  //initialize button objects
  btn1.begin();
  btn2.begin();
  // start OLED
  oled.begin();
  oled.setFont(FONT8X16);
  // Clear the memory before turning on the display
  oled.clear();
  // Turn on the display
  oled.on();
  // Switch the half of RAM that we are writing to, to be the half that is non currently displayed
  oled.switchRenderFrame();
  SetClock(0x12, 0x00); //writing to it in HEX

}

void loop() {

  if (showTime==1){/////////////////////////////////////////////////////////--------------------SHOW TIME//////////////////////
// Read the time from the RTC
    TinyI2C.start(RTCaddress, 0);
    TinyI2C.write(1);
    TinyI2C.restart(RTCaddress, 6);
    mins = TinyI2C.read();  //min
    hrs = TinyI2C.read();   //hour
    wkdy = TinyI2C.read();  //weekday
    dy= TinyI2C.read();    //monthday
    mth = TinyI2C.read();  //month
    yr = TinyI2C.read();   //year
    TinyI2C.stop();
  }
  
  
  // read the buttons
  btn1.read();
  btn2.read();
  
  //button logic
  if (btn1.wasPressed()){  //nums have to be set to 0x10 after 0x09 since
                            //0x09+0x01=0x0A =/= 0x10
   timepressed=millis(); //store current time
   keypress+=1;
   if (keypress>3){ //if key is pressed more than 3 times in case of accidental press while displaying
   int newhours=hrs+0x01;
   if (newhours>0x23) //if higher than max
   SetClock(0,mins);
   else if (newhours==0x0A || newhours==0x1A){ //add additional correction of 0x06 
     newhours+=0x06;
     SetClock(newhours,mins);}
     else
       SetClock(newhours, mins);
   }
  }
  if (btn2.wasPressed()){ //same correction needed for 09 19 29 39 49 
    timepressed=millis(); //store current time
    keypress+=1;
    if (keypress>3){
      int newminutes=mins+0x01;
      if (newminutes>0x59)
        SetClock(hrs,0);
      else if (newminutes==0x0A || newminutes==0x1A || newminutes==0x2A || newminutes==0x3A|| newminutes==0x4A){
        newminutes+=0x06;
        SetClock(hrs,newminutes);}
      else
        SetClock(hrs, newminutes);
     }
  }




  if (showTime==1){////////////////////////////////////////////////////////SHOW TIME AGAIN///////////////////////////////////////

  // Clear the half of memory not currently being displayed.
  oled.clear();

  // Position the text cursor
  // In order to keep the library size small, text can only be positioned
  // with the top of the font aligned with one of the four 8 bit high RAM pages.
  // The Y value therefore can only have the value 0, 1, 2, or 3.
  // usage: oled.setCursor(X IN PIXELS, Y IN ROWS OF 8 PIXELS STARTING WITH 0);
  
  oled.setCursor(12, 2);
  oled.print(F("   "));
  if (hrs>=0x13)  //if hours is over 13 aka military)
    isabove12 = true;
  else
     isabove12 = false;  
  

  if (isabove12){
    if (hrs>0x19)
     oled.print(hrs-0x18);//no need to display in hex
     else
     oled.print(hrs-0x12);//no need to display in hex
  }
  
  else if (hrs<0x10 && hrs!=0){ //check if less than 12 and nonzero
    oled.print(0);        //to print an extra 0 before to make it double digit
    oled.print(hrs,HEX);}
  else if (hrs==0)//00 is 12AM so still 12
  oled.print(F("12")); 
  else
  {
   oled.print(hrs,HEX);}
   
  oled.print(F(":"));
  
  if (mins<0x10){         //make mins double digit
    oled.print(0);
    oled.print(mins,HEX);
  }
  else{
    oled.print(mins,HEX);}

  if (hrs>=0x12)
    oled.print(F("PM")); 
 
  else
    oled.print(F("AM"));
  
 oled.setCursor(16,0);

  
 

 switch(wkdy){

    case 1: oled.print(F("Mon"));
            break;
    case 2: oled.print(F("Tue"));
            break;
    case 3: oled.print(F("Wed"));
            break;
    case 4: oled.print(F("Thr"));
            break;
    case 5: oled.print(F("Fri"));
            break;
    case 6: oled.print(F("Sat"));
            break;
    case 7: oled.print(F("Sun"));
            break;
    default: oled.print(F("Mon"));
            break;
  }
  
 // oled.print(wkdy);
 
  oled.print(F(""));
  oled.print(mth,HEX);
  oled.print(F("/"));
  oled.print(dy,HEX);
  oled.print(F("/"));
  oled.print(yr,HEX);
  // delay(1000);

  // Swap which half of RAM is being written to, and which half is being displayed.
  // This is equivalent to calling both switchRenderFrame and switchDisplayFrame.
 oled.switchFrame();
 
 }////////////end of showitime
timepassed=millis();
if (timepassed-timepressed>2000) //wait 2 seconds and then reset keypress count which will turn off screen
  keypress=0;

  
 displayTime();
}


void displayTime(){
  if (keypress>0){
  oled.on();
  showTime=1;}
  else{
  oled.off();
  showTime=0;
  }
}
