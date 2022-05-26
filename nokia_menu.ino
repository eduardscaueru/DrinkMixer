#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#define OK_BTN PD4
#define BACK_BTN PD3
#define NEXT_BTN PD2
#define HOLD_BTN PD5
#define TRIG_PIN PD6
#define ECHO_PIN PD7
#define WATER_PIN PB5
#define VODKA_PIN PB4
#define JUICE_PIN PB3
#define GIN_PIN PB2
#define WATER_SENSOR A5
#define VODKA_SENSOR A4

Adafruit_PCD8544 display = Adafruit_PCD8544(8, 9, A2, A1, A0);

int x = 0;

const int debounceTime = 200;

int lowerThreshold = 640;
int upperThreshold = 660;

byte Arrow[8] =
{
0b10000,
0b11000,
0b11100,
0b11110,
0b11110,
0b11100,
0b11000,
0b10000
};

int running = 0;
int currentDrink = 0;
bool reset = false;

const int numOfScreens = 4;
volatile int currentScreen = 0;
String screens[numOfScreens][2] = {{"Press a button", "to continue..."},
{"Make Drink", "Show Status"}, {"Water Vodka", "Juice Gin"},
{"Hold OK for", " "}};
volatile int screenSelection[numOfScreens][4];
volatile bool changeScreen = true;
volatile bool buttonChanged = true;
bool leaveScreen = false;
volatile bool pumpOn = false;
int pumps[] = {WATER_PIN, VODKA_PIN, JUICE_PIN, GIN_PIN};

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
int lastButtonState = 0;
int buttonState;
unsigned long lastDebounceTime2 = 0;
int lastButtonState2 = 0;
int buttonState2;

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600);
  
  //Serial.println("PCD test");
  display.begin();
  // init done

  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(30);

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(BLACK, WHITE);
  display.setCursor(0, 0);
  display.print("ON");
  display.setTextColor(WHITE,BLACK);
  display.setCursor(0, 20);
  display.print("OFF");

  display.display();

  screenSelection[1][0] = 1;
  screenSelection[2][0] = 1;

  // Buttons setup
  //pinMode(OK_BTN, INPUT_PULLUP);
  DDRD &= ~(1 << OK_BTN);
  PORTD |= (1 << OK_BTN);
  //pinMode(BACK_BTN, INPUT_PULLUP);
  DDRD &= ~(1 << BACK_BTN);
  PORTD |= (1 << BACK_BTN);
  //pinMode(NEXT_BTN, INPUT_PULLUP);
  DDRD &= ~(1 << NEXT_BTN);
  PORTD |= (1 << NEXT_BTN);
  //pinMode(HOLD_BTN, INPUT_PULLUP);
  DDRD &= ~(1 << HOLD_BTN);
  PORTD |= (1 << HOLD_BTN);

  // DISTANCE SENSOR FOR GLASS
  //pinMode(TRIG_PIN, OUTPUT);
  DDRD |= (1 << TRIG_PIN);
  //pinMode(ECHO_PIN, INPUT);
  DDRD &= ~(1 << ECHO_PIN);

  // PUMP PINS
  //pinMode(WATER_PIN, OUTPUT);
  DDRB |= (1 << WATER_PIN);
  //digitalWrite(WATER_PIN, HIGH);
  PORTB |= (1 << WATER_PIN);

  //pinMode(VODKA_PIN, OUTPUT);
  DDRB |= (1 << VODKA_PIN);
  //digitalWrite(VODKA_PIN, HIGH);
  PORTB |= (1 << VODKA_PIN);

  //pinMode(JUICE_PIN, OUTPUT);
  DDRB |= (1 << JUICE_PIN);
  //digitalWrite(JUICE_PIN, HIGH);
  PORTB |= (1 << JUICE_PIN);

  //pinMode(GIN_PIN, OUTPUT);
  DDRB |= (1 << GIN_PIN);
  //digitalWrite(GIN_PIN, HIGH);
  PORTB |= (1 << GIN_PIN);

  // External Interrupts setup for buttons BACK_BTN and NEXT_BTN
  EICRA |= (1 << ISC11) | (1 << ISC10);
  EICRA |= (1 << ISC00) | (1 << ISC01);
  EIMSK |= (1 << INT1) | (1 << INT0);

  // Pin Change Interrupts setup for button OK_BTN
  PCICR |= (1 << PCIE2);
  PCMSK2 |= (1 << PCINT20);

  // Activate global interrupts
  sei();
}

ISR(INT0_vect) {
  /*volatile static unsigned long last_interrupt_time = 0;
  volatile unsigned long interrupt_time = millis();

  if (pumpOn) return;

  if (interrupt_time - last_interrupt_time > debounceTime) {
    if (currentScreen == 1) {
      if (screenSelection[1][0] == 1) {
        screenSelection[1][0] = 0;
        screenSelection[1][1] = 1;
      }
      else {
        screenSelection[1][0] = 1;
        screenSelection[1][1] = 0;
      }
    } else if (currentScreen == 2) {
      for (int i = 0; i < 4; i++) {
        if (screenSelection[2][i] == 1) {
          screenSelection[2][i] = 0;
          screenSelection[2][(i + 1) % 4] = 1;
          break;
        }
      }
    } else {
      if (currentScreen != 3) {
        currentScreen++;
      }
    }
    changeScreen = true;
  }

  last_interrupt_time = interrupt_time;*/
}

ISR(INT1_vect) {
  /*volatile static unsigned long last_interrupt_time = 0;
  volatile unsigned long interrupt_time = millis();

  Serial.println(pumpOn);

  if (pumpOn) return;

  //if (interrupt_time - last_interrupt_time > debounceTime) {
    Serial.println("CPLM");

    currentScreen--;
    if (currentScreen == -1) currentScreen = 0;

    changeScreen = true;
  //}

  last_interrupt_time = interrupt_time;*/
}

ISR(PCINT2_vect) {
  volatile static unsigned long last_interrupt_time = 0;
  volatile unsigned long interrupt_time = millis();

  if (pumpOn) return;

  if (interrupt_time - last_interrupt_time > 1) {
    last_interrupt_time = interrupt_time;
    if ((PIND & (1 << OK_BTN)) == 0) {
      //Serial.println("BRRRR");
      
      if (buttonChanged) {
        buttonChanged = !buttonChanged;
        if (currentScreen == 1) {
          if (screenSelection[1][0] == 1) {
            currentScreen++;
            changeScreen = true;
          } else {
            currentScreen = 4;
            changeScreen = true;
          }
        } else if (currentScreen == 2) {
          currentScreen++;
          changeScreen = true;
        } else {
          //Serial.println("3");
        }
        
      }
    } else {
      buttonChanged = !buttonChanged;
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (changeScreen) {
    printScreen();
    changeScreen = false;
  }

  checkForGlass();

  readSelectButton();

  readBackButton();

  readHoldButton();
}

void readSelectButton() {
  int reading = PIND & (1 << NEXT_BTN);
  if (reading != lastButtonState2) {
    // reset the debouncing timer
    lastDebounceTime2 = millis();
  }
  if ((millis() - lastDebounceTime2) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState2) {
      buttonState2 = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState2 != 0) {
        if (currentScreen == 1) {
          if (screenSelection[1][0] == 1) {
            screenSelection[1][0] = 0;
            screenSelection[1][1] = 1;
          }
          else {
            screenSelection[1][0] = 1;
            screenSelection[1][1] = 0;
          }
        } else if (currentScreen == 2) {
          for (int i = 0; i < 4; i++) {
            if (screenSelection[2][i] == 1) {
              screenSelection[2][i] = 0;
              screenSelection[2][(i + 1) % 4] = 1;
              break;
            }
          }
        } else {
          if (currentScreen != 3) {
            currentScreen++;
          }
        }
        changeScreen = true;
      }
    }
  }

  lastButtonState2 = reading;
}

void readHoldButton() {
  if (currentScreen == 3) {
    if (!(PIND & (1 << HOLD_BTN))) {
      pumpOn = true;
      cli();
      //Serial.println(1);
      //digitalWrite(pumps[currentDrink], LOW);
      PORTB &= ~(1 << pumps[currentDrink]);
    } else {
      //Serial.println(0);
      //digitalWrite(pumps[currentDrink], HIGH);
      PORTB |= (1 << pumps[currentDrink]);
      pumpOn = false;
      sei();
    }
  }
}

void checkForGlass() {
  if (currentScreen == 0) {
    long duration, distance;
  
    //digitalWrite(TRIG_PIN, LOW);
    PORTD &= ~(1 << TRIG_PIN);
    delayMicroseconds(2);
    //digitalWrite(TRIG_PIN, HIGH);
    PORTD |= (1 << TRIG_PIN);
    delayMicroseconds(10);
    //digitalWrite(TRIG_PIN, LOW);
    PORTD &= ~(1 << TRIG_PIN);
    
    duration = pulseIn(ECHO_PIN, HIGH);
    
    distance = (duration / 2) / 29.1;
    if (distance <= 3) {
      pumpOn = true;
      cli();
      PORTB &= ~(1 << pumps[0]);
    } else {
      PORTB |= (1 << pumps[0]);
      pumpOn = false;
      sei();
    }
    //Serial.print(distance);
    //Serial.println(" cm");

    //delay(500);
  }
}

void readBackButton() {
  int reading = PIND & (1 << BACK_BTN);
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState != 0) {

        if (currentScreen == 4) {
          currentScreen = 1;
        } else {
          currentScreen--;
          if (currentScreen == -1) currentScreen = 0;
        }

        changeScreen = true;
      }
    }
  }

  lastButtonState = reading;
}

void printScreen() {
  display.clearDisplay();

  if (currentScreen == 1) {
    if (screenSelection[1][0] == 1) {
      display.setTextColor(WHITE, BLACK);
      display.setCursor(0, 0);
      display.print(screens[currentScreen][0]);
      display.setTextColor(BLACK, WHITE);
      display.setCursor(0, 20);
      display.print(screens[currentScreen][1]);
    } else {
      display.setTextColor(BLACK, WHITE);
      display.setCursor(0, 0);
      display.print(screens[currentScreen][0]);
      display.setTextColor(WHITE, BLACK);
      display.setCursor(0, 20);
      display.print(screens[currentScreen][1]);
    }
    
    display.display();
  } else if (currentScreen == 2) {
    String d1 = getValue(screens[currentScreen][0], ' ', 0);
    String d2 = getValue(screens[currentScreen][0], ' ', 1);
    String d3 = getValue(screens[currentScreen][1], ' ', 0);
    String d4 = getValue(screens[currentScreen][1], ' ', 1);

    if (screenSelection[2][0] == 1) {
      display.setTextColor(WHITE, BLACK);
      display.setCursor(0, 0);
      display.print(d1);
      display.setTextColor(BLACK, WHITE);
      display.setCursor(0, 10);
      display.print(d2);
      display.setCursor(0, 20);
      display.print(d3);
      display.setCursor(0, 30);
      display.print(d4);
    } else if (screenSelection[2][1] == 1) {
      display.setTextColor(BLACK, WHITE);
      display.setCursor(0, 0);
      display.print(d1);
      display.setTextColor(WHITE, BLACK);
      display.setCursor(0, 10);
      display.print(d2);
      display.setTextColor(BLACK, WHITE);
      display.setCursor(0, 20);
      display.print(d3);
      display.setCursor(0, 30);
      display.print(d4);
    } else if (screenSelection[2][2] == 1) {
      display.setTextColor(BLACK, WHITE);
      display.setCursor(0, 0);
      display.print(d1);
      display.setCursor(0, 10);
      display.print(d2);
      display.setTextColor(WHITE, BLACK);
      display.setCursor(0, 20);
      display.print(d3);
      display.setTextColor(BLACK, WHITE);
      display.setCursor(0, 30);
      display.print(d4);
    } else if (screenSelection[2][3] == 1) {
      display.setTextColor(BLACK, WHITE);
      display.setCursor(0, 0);
      display.print(d1);
      display.setCursor(0, 10);
      display.print(d2);
      display.setCursor(0, 20);
      display.print(d3);
      display.setTextColor(WHITE, BLACK);
      display.setCursor(0, 30);
      display.print(d4);
    }

    display.display();
  } else if (currentScreen == 3) {
    String d1 = getValue(screens[2][0], ' ', 0);
    String d2 = getValue(screens[2][0], ' ', 1);
    String d3 = getValue(screens[2][1], ' ', 0);
    String d4 = getValue(screens[2][1], ' ', 1);

    display.setTextColor(BLACK, WHITE);
    display.setCursor(0, 0);
    display.print(screens[currentScreen][0]);
    display.setCursor(0, 20);
    
    if (screenSelection[2][0] == 1) {
      currentDrink = 0;
      display.print(d1);
    } else if (screenSelection[2][1] == 1) {
      currentDrink = 1;
      display.print(d2);
    } else if (screenSelection[2][2] == 1) {
      currentDrink = 2;
      display.print(d3);
    } else {
      currentDrink = 3;
      display.print(d4);
    }

    display.display();
  } else if (currentScreen == 4) {
    String d1 = getValue(screens[2][0], ' ', 0);
    String d2 = getValue(screens[2][0], ' ', 1);
    String d3 = getValue(screens[2][1], ' ', 0);
    String d4 = getValue(screens[2][1], ' ', 1);

    String waterLevel = getLevel(WATER_SENSOR);
    String vodkaLevel = getLevel(VODKA_SENSOR);

    display.setTextColor(BLACK, WHITE);
    display.setCursor(0, 0);
    display.print(d1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(45, 0);
    display.print(waterLevel);
    display.setTextColor(BLACK, WHITE);
    display.setCursor(0, 10);
    display.print(d2);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(45, 10);
    display.print(vodkaLevel);
    display.setTextColor(BLACK, WHITE);
    display.setCursor(0, 20);
    display.print(d3);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(45, 20);
    display.print(waterLevel);
    display.setTextColor(BLACK, WHITE);
    display.setCursor(0, 30);
    display.print(d4);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(45, 30);
    display.print(waterLevel);

    display.display();
  } else {
    display.setTextColor(BLACK, WHITE);
    display.setCursor(0, 0);
    display.print(screens[currentScreen][0]);
    display.setCursor(0, 20);
    display.print(screens[currentScreen][1]);

    display.display();
  }
}

String getLevel(int PIN) {
  int level = analogRead(PIN);
  
  if (level == 0) {
    return "Empty";
  }
  else if (level > 0 && level <= lowerThreshold) {
    return "Low";
  }
  else if (level > lowerThreshold && level <= upperThreshold) {
    return "Medium";
  }
  else if (level > upperThreshold) {
    return "High";
  }
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
