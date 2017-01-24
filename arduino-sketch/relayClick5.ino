#include <CurieBLE.h>
/**
 * This is a copy of the relay-click sketch with two LEDs instead of a relay
 * 
 * Uplaod this sketch to your arduino/genuino101, complete the circuit and access from the web!
 * Web Bluetoooth goodness!
 * 
 * To be added:
 *    2) BLE control. Done
 *    3) LED object. Done
 *    4) LED interval function. Done
 *    5) RTC "alarm clock" function. Yet to be done . . .
 */

/**
 * pins for each relay specified here. 
 * depend on the Arduino click shield specifications, with the relay click board 
 * plugged into slot 1
*/   


/**
 * pins for each led specified here. 
 * depend on the Arduino click shield specifications, with the led click board 
 * plugged into slot 1
*/   
      #define LED1 13 // pin to use for the LED
      #define LED2 12 // pin to use for the LED

class Led
{

  int ledID;
  int pinAssignment;
  boolean ledState;
  boolean intervalState;
  long interval;
  boolean intervalToggle;

  public:

  Led(int id, int pin )
  {
    ledID = id;
    pinAssignment = pin;
    ledState = false;  
    intervalState = false;
    interval = 0;
    intervalToggle = false;
  }
/*
   * The led is initialized to the physical pin assigned to it and 
   * this pin is set to low.
   * Not the led intial states are set in the led constructor.
   */
  void initialize()
  {
    pinMode( pinAssignment, OUTPUT);
    digitalWrite(pinAssignment,LOW);
  }
  
  int getLedID() {return ledID;}
  void setLedId(int id) {ledID = id;}

  int getPinAssignment() {return pinAssignment;}
  void setPinAssignment(int pin) {pinAssignment = pin;}

  boolean getLedState() {return ledState;}
  void setLedState(boolean state ) {ledState = state;}

  boolean getIntervalState() {return intervalState;}
  void setIntervalState(boolean state) { intervalState = state;}

  long getInterval(){return interval;}
  void setInterval(long i) {interval = i;}

  boolean getIntervalToggle() {return intervalToggle;}
  void setIntervalToggle(boolean tog){intervalToggle = tog;}

  void toggle() 
  { 
    intervalToggle = !intervalToggle;
    digitalWrite(pinAssignment, intervalToggle);
  }

  void turnOn()
  {
    digitalWrite(pinAssignment,HIGH);
  }

  void turnOff()
  {
    digitalWrite(pinAssignment,LOW);
  }

};
Led led1(1,LED1);
Led led2(2,LED2);

// timing parameters
long currentMillis, previousMillisLed1, previousMillisLed2;

union
{
  long interval;
  unsigned char bytes[4];  
} ledSettings;


BLEPeripheral blePeripheral;  // BLE Peripheral Device (the board you're programming)
BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLECharacteristic ledCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite, 5);
BLEDescriptor ledDescriptor("2902","led"); //not sure what "2902 stands for

void setup() {
  Serial.begin(9600);

  // set LED pin to output mode
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  // set advertised local name and service UUID:
  blePeripheral.setLocalName("LED");
  blePeripheral.setAdvertisedServiceUuid(ledService.uuid());

  // add service and characteristic:
  blePeripheral.addAttribute(ledService);
  blePeripheral.addAttribute(ledCharacteristic);


  blePeripheral.addAttribute(ledDescriptor);

  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
  ledCharacteristic.setEventHandler(BLEWritten, ledCharacteristicWritten);
  
  // All BLE characteristics should be initialized to a starting value prior
  // using them.
  const unsigned char ledInitializer[5] = {0,0,0,0,0};
  ledCharacteristic.setValue(ledInitializer,5);
  
  // begin advertising BLE service:
  blePeripheral.begin();
  // Set leds to initial states
  led1.initialize();
  led2.initialize();


  // intialize timing paramters
  currentMillis = millis();
  previousMillisLed1 = millis();
  previousMillisLed2 = millis();
  
  Serial.println("BLE LED Peripheral");
}

void loop() {

    /**
   * The main loop() checks for the leds being set to intervals and
   * toggles then is so and the interval is exceeded.
   */
  // listen for BLE peripherals to connect:
  BLECentral central = blePeripheral.central();

  currentMillis = millis();
  
  if (led1.getIntervalState()) 
  {
    if ((currentMillis - previousMillisLed1) > led1.getInterval())
    {
      led1.toggle();
      previousMillisLed1 = currentMillis;
      Serial.println("toggle1");
    }
  }
  }


void blePeripheralConnectHandler(BLECentral& central) 
{
  // central connected event handler
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLECentral& central) 
{
  // central disconnected event handler
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
}

void ledCharacteristicWritten(BLECentral& central, BLECharacteristic& characteristic)
{
  /**
   * This function is called whenever the user changes the ledCharacteristic on the hybrid
   * web app. It is used to chage the state of the led object, these changes are then
   * checked for in the loop. 
   * Why? Well if we wished to simply turn the leds off and on, this approach is unnecessary. 
   * we could just simply turn the leds off and on in the swtch/case statement. However
   * we would like to add other functionality to the leds, such as interval on/off timing.
   * Since we have 2 leds we can;t use the CurieTImer library, it has only one timer and we may
   * wish to have different timing intervals for each led. Addtionally this approach will allow
   * us to more easily extend this app to many leds, which is is more realisitc for
   * a home automation system.
   */
  const unsigned char* ledData = ledCharacteristic.value();
  
  ledSettings.bytes[0] = ledData[4];
  ledSettings.bytes[1] = ledData[3];
  ledSettings.bytes[2] = ledData[2];
  ledSettings.bytes[3] = ledData[1];

  // convert interval byte array to long by casting;
  long interval = (long)ledSettings.interval;
  switch(ledData[0])
  {
    case 0:
      // 0 value, do nothing
      Serial.print(ledData[0]);
      Serial.print(" written ");Serial.print("interval: ");Serial.println(interval);
      break;
    case 1:
      // led 1 selected
      Serial.print("led1: ");Serial.print(ledData[0]);Serial.print(" interval: ");Serial.println(interval);
      if (interval != 0) 
      {
        led1.setIntervalState(true);
        led1.setLedState(true);
        led1.setInterval(interval);
      } else {
        if (led1.getLedState()) 
        { 
          led1.setLedState(false); 
          led1.turnOff(); 
          led1.setIntervalState(false);
          led1.setInterval(0); 
        }
        else {
          led1.setLedState(true); 
          led1.turnOn();
          led1.setIntervalState(false);
          led1.setInterval(0);
        }       
      }
      break;
    case 2:
      // led 2 selected
      Serial.print("led2: ");Serial.print(ledData[0]);Serial.print(" interval: ");Serial.println(interval);
      if (interval != 0) 
      {
        led2.setIntervalState(true);
        led2.setLedState(true);
        led2.setInterval(interval);
      } else {
        if (led2.getLedState()) 
        { 
          led2.setLedState(false); 
          led2.turnOff(); 
          led2.setIntervalState(false);
          led2.setInterval(0); 
        }
        else {
          led2.setLedState(true); 
          led2.turnOn();
          led2.setIntervalState(false);
          led2.setInterval(0);
        }       
      }
      break;     
  }
}  

