#define BLINKER_PRO_ESP
#define BLINKER_BUTTON
#define BLINKER_BUTTON_LONGPRESS_POWERDOWN
#define BLINKER_OTA_VERSION_CODE "0.1.1"
#include <Blinker.h>


char type[] = "bmIupvSZcr0U";
char auth[] = "4hybs8lCPYOa";//set device type, auth key for this device will automatic generated.


int32_t fA1 = 0; // Initial number of add food1 event
int32_t fA2 = 0; // Initial number of add food2 event


#define Slider_1 "Food1Amount" //food slide1
#define Slider_2 "Food2Amount" //food slide2
BlinkerSlider Slider1(Slider_1);
BlinkerSlider Slider2(Slider_2);
BlinkerNumber temperature("temperature");
BlinkerNumber waterRemain("waterLevel");
BlinkerButton Button1("addFoodBnt1");
BlinkerButton Button2("addFoodBnt2");

void slider1_callback(int32_t value){ // read the value from user's app
    BLINKER_LOG("get slider value: ", value);
    fA1 = value;

}
void slider2_callback(int32_t value){
    BLINKER_LOG("get slider value: ", value);
    fA2 = value;

}


void button1_callback(const String & state){// add hay
    BLINKER_LOG("get button state: ", state);
    if (state == "tap") {
    digitalWrite(25, HIGH);
    digitalWrite(26, HIGH);
    Blinker.delay(50*fA1);// action time is set by user's app silder
    digitalWrite(25, LOW);
    digitalWrite(26, LOW);
    Blinker.push("Food1 Added!");    
  }
}
void button2_callback(const String & state) {// add food2
    BLINKER_LOG("get button state: ", state);
    if (state == "tap") {
    digitalWrite(27, HIGH);
    Blinker.delay(50*fA2);
    digitalWrite(27, LOW);
    Blinker.push("Food2 Added!");   
  }
}


void heartbeat()
{
    BLINKER_LOG("heartbeat!");
    float tempval = getTemp();
    float waterTankLevel = getTankLevel();
    temperature.print(tempval);
    waterRemain.print(waterTankLevel);
    Slider1.print(fA1);
    Slider2.print(fA2);
}

float getTemp(){
  float tempR1 = float(analogRead(34));
  float a1 = (tempR1 *0.73 / 700 -0.5)* 100 - 2.5;
  float a = a1;
  int i = 0;
  for(i;i<20;i++){
     tempR1 = float(analogRead(34));
     a1 = (tempR1 *0.73 / 700 -0.5)* 100 - 2.5;
     a += a1;
  }
  return (a*1.8/21+32)+8;
}

float getTankLevel(){
  float waters = float(analogRead(32));
  float w1 = waters *0.73 / 700;
  float w = w1;
  int ii = 0;
  if(w1<20){
    Blinker.push("Water Level Low!");     
  }
  for(ii;ii<20;ii++){
     waters = float(analogRead(32));
     w1 = waters *0.73 / 700;
     w += w1;
  }
  return w/21*100;
}

float getSmallTank(){
  float waters = float(analogRead(35));
  float w1 = waters *0.73 / 700;
  float w = w1;
  int ii = 0;
  if(w1<20){
    Blinker.push("Water Level Low!");     
  }
  for(ii;ii<20;ii++){
     waters = float(analogRead(35));
     w1 = waters *0.73 / 700;
     w += w1;
  }
  return w/21;

  }

void dataStorage() {
  float tempval = getTemp();
  float waterTankLevel = getTankLevel();
  Blinker.dataStorage("temperature", tempval);
  Blinker.dataStorage("waterLevel", waterTankLevel);
}




void setup()
{
    Serial.begin(115200);
    BLINKER_DEBUG.stream(Serial);
    BLINKER_DEBUG.debugAll();

    pinMode(33, OUTPUT);// Pump Pin

    pinMode(25, OUTPUT);// Add Hay1
    pinMode(26, OUTPUT);// Add Hay2
    pinMode(27, OUTPUT);// Add Food

    pinMode(34, INPUT);// Temperature pin
    pinMode(32, INPUT); // WatertTankSensor PIN
    pinMode(35, INPUT); // Smallwatertank Sensor PIN

    Blinker.begin(auth, type);
    Blinker.setTimezone(-4.0);// set TimeZone as EST
    Blinker.attachHeartbeat(heartbeat);
    Blinker.attachDataStorage(dataStorage, 120 , 1); // 120s a cycle to transmit data to the cloud
    Slider1.attach(slider1_callback);
    Slider2.attach(slider2_callback);
    Button1.attach(button1_callback);
    Button2.attach(button2_callback);

}

void printSlideData(){
  Serial.println(fA1);
  Serial.println(fA2);
  delay(200);
  }

void loop(){
    if(getSmallTank()<0.3){
      digitalWrite(33,HIGH);
      }
    if (getSmallTank()>0.9){
      digitalWrite(33,LOW);
      }
    Blinker.run();
}
