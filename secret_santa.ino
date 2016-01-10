/*  // For the uno
#define BRIGHTNESS A2  // The brightness potentiometer
#define DELAY A1 // The delay potentiometer

#define FREEZE 2  // Button that freezes color
#define MODE_SELECT 3  // Button that changes the mode

#define FREEZE_LED 4
#define MODE_SELECT_LED 5

#define RED 9
#define GREEN 10
#define BLUE 11

#define MODE1 6
#define MODE2 7
#define MODE3 8
*/

    // For the nano
#define BRIGHTNESS A7  // The brightness potentiometer
#define DELAY A6 // The delay potentiometer

#define FREEZE A3  // Button that freezes color
#define MODE_SELECT A1  // Button that changes the mode
#define FAN_SELECT A2

#define FREEZE_LED 8
#define MODE_SELECT_LED A4
#define FAN_LED 9
#define THE_FAN A0

#define RED 3
#define GREEN 10
#define BLUE 6
/*
#define MODE1 6
#define MODE2 7
#define MODE3 8
*/


#define NUM_MODES_MINUS_ONE 2
int mode = 0;  // 0 = flash
// 1 = flash 
// 2 = flash rand
// 3 = smooth
//
//
int delayTime = 0;  // Make these EEPROM later if so desire
boolean frozen = false;
boolean fanOn = false;

unsigned long lastChange = 0;

unsigned int rgb[3] = {0, 0, 0};

boolean modeChange = true;  // For defining when a mode has been switched for
                            // initial conditions, etc

int brightnessConst = 1;

/// For rainbow flasher

int flashTable[6][3] = { {255, 0, 0}, // red
                         {255, 50, 0}, // orange
                         {255, 100, 0}, // yellow
                         {0, 255, 0}, // green
                         {0, 0, 255}, // blue
                         {255, 0, 225} // purple
};

int flashTablePtr = 0;

/// end for rainbow flasher

/// For smooth rainbow fade

int decColor = 0;
int incColor = /*decColor == 2 ? 0 : decColor +*/ 1; // Wait til internet to figure out what that comes out to
int counter = 0;

/// end for smooth rainbow fade

/// for random flasher

int dontLight = 0;

/// end for random flasher

void setup() {
  pinMode(BRIGHTNESS, INPUT);
  pinMode(DELAY, INPUT);
  
  pinMode(MODE_SELECT, INPUT);
  pinMode(FREEZE, INPUT);
  
  pinMode(FAN_SELECT, INPUT);
  pinMode(FAN_LED, OUTPUT);
  pinMode(THE_FAN, OUTPUT);
  /*
  pinMode(MODE1, OUTPUT);
  pinMode(MODE2, OUTPUT);
  pinMode(MODE3, OUTPUT);
*/
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
}

void loop() {
  
  //showModes();
  
  // Mode change button was pressed
  if (digitalRead(MODE_SELECT) == HIGH) {
    digitalWrite(MODE_SELECT_LED, HIGH);
    modeChange = true;
    if (mode == NUM_MODES_MINUS_ONE) {
      mode = 0;
    } else {
      ++mode;
    }
    delay(500);
    digitalWrite(MODE_SELECT_LED, LOW);
  }

  // Freeze button was pressed
  if (digitalRead(FREEZE) == HIGH) {
    digitalWrite(FREEZE_LED, HIGH);
    frozen = !frozen;  // This should work right?
    delay(500);
    digitalWrite(FREEZE_LED, LOW);
  }

  if (!frozen) {
    digitalWrite(FREEZE_LED, LOW);
    switch (mode) {
      case 0:
        flash();
        break;
      case 1:
        flashRand();
        break;
      case 2:
        smooth();
        break;

    }

//    writeLEDs();

  } else {
    digitalWrite(FREEZE_LED, HIGH);
  }
  
      writeLEDs();
  
  if (digitalRead(FAN_SELECT) == HIGH) {
    fanOn = !fanOn;
    fanCall();
    delay(250);
  } 
  fanCall();
  
  
  
  // The delay should be taken care of inside function calls
  //delay(analogRead(DELAY));
  // Dont want to delay entire program and miss inputs
/*
  flash();
  //flashRand();
  //smooth();
  writeLEDs();*/
}

void fanCall() {
  if (fanOn) {
    digitalWrite(FAN_LED, HIGH);  // opposite for pulled high
    digitalWrite(THE_FAN, HIGH);
    //digitalWrite(FAN, LOW);
  } else {
    //digitalWrite(FAN, HIGH);
    digitalWrite(THE_FAN, LOW);
    digitalWrite(FAN_LED, LOW);
  }
}


void writeLEDs() {
  getBrightness();
  //brightnessConst = 1;
  analogWrite(RED, rgb[0] / brightnessConst);
  analogWrite(GREEN, rgb[1] / brightnessConst);
  analogWrite(BLUE, rgb[2] / brightnessConst);

}

void getBrightness() {
  
  //brightnessConst = sqrt(analogRead(BRIGHTNESS) / 4);
  brightnessConst = analogRead(BRIGHTNESS);
  /*if (brightnessConst > 700) {
    brightnessConst = 85 + 0.5 * (brightnessConst - 700);
  } else if (brightnessConst > 200) {
    brightnessConst = 10 + 0.15 * (brightnessConst - 200);
  } else {
    brightnessConst = 0.005 * brightnessConst;
  }*/
  
  brightnessConst = analogRead(BRIGHTNESS);
  if (brightnessConst > 600) {
    brightnessConst = 18 + 0.3 * (brightnessConst - 600);
  } else {
    brightnessConst = 0.03 * brightnessConst;
  }
  
  
  if (0 == brightnessConst) {
    brightnessConst = 1;
  }
  
}

void flash() {
  
  /*
  if (modechange) {
    // Any initial conditions?
    modeChange = false;
  }*/
  flashDelay();
  // Anything special for delay?
  // no
  if (millis() - lastChange > delayTime) {
    lastChange = millis();

    if (5 == flashTablePtr) {
      flashTablePtr = 0;
    } else {
      ++flashTablePtr;
    }

    rgb[0] = flashTable[flashTablePtr][0];
    rgb[1] = flashTable[flashTablePtr][1];
    rgb[2] = flashTable[flashTablePtr][2];

  }

}

void flashDelay() {
  delayTime = analogRead(DELAY);
  if (delayTime > 800) {
    delayTime = 1538 + 10*(delayTime - 800);
  } else if (delayTime > 300) {
    delayTime = 63 + 3*(delayTime - 300);
  } else {
    delayTime = sqrt(10 * delayTime);
  }

}


void smooth() {
  // this looks like a viable optoin, test when ready https://www.arduino.cc/en/Tutorial/ColorCrossfader
  // This looks good too, less complicated https://gist.github.com/jamesotron/766994

  // Right now using jamesotron's
  
  if (modeChange) {
    rgb[0] = 255;
    rgb[1] = 0;
    rgb[2] = 0;
    decColor = 0;
    incColor = 1;
    counter = 0;
    lastChange = micros();
    modeChange = false;
  }
  modeChange = false;
  
  // Anything special for delay?
  smoothDelay();
  //flashDelay();
  // divide by 2 since this will move slow anyway
  if (micros() - lastChange > delayTime) {

    lastChange = micros();
    
    if (255 == counter) {
      counter = 0;
      
      if (incColor == 2) {
        incColor = 0;
        ++decColor; // It equals 2 now
      } else if (2 == decColor) {
        decColor = 0;
        ++incColor; // Its going to be 1
      } else {
        ++decColor;
        ++incColor;
      }

      //incColor = decColor == 2 ? 0 : decColor + 1;

    }

      ++counter;

      --rgb[decColor];
      ++rgb[incColor];

    

  }

}

void smoothDelay() {
  delayTime = analogRead(DELAY);
  if (delayTime > 250) {
    delayTime = 1565 + 31*(delayTime - 250);
  } else if (delayTime > 50) {
    delayTime = 150 + 7*(delayTime - 50);
  } else {
    delayTime = 3 * delayTime;
  }
  //delayTime = (int) pow(delayTime, 1.5);
  
}



void flashRand() {
  
  /*
  if (modeChange) {
    // Any initial conditions?
    modeChange = false;
  }
  */
  flashDelay();
  // Anything special for delay?
  // no
  if (millis() - lastChange > delayTime) {
    lastChange = millis();
/*
    rgb[0] = random(0, 255);  // Max is exclusive but
    rgb[1] = random(0, 255);  // to save that extra byte...
    rgb[2] = random(0, 255);
*/
    
    if (random(0,3) >= 1) {  //66% of the time
      // Only do 2
      dontLight = random(0,8);
      rgb[dontLight % 3] = 0;
      rgb[(dontLight + 1) % 3] = random(0, 255);
      rgb[(dontLight + 2) % 3] = random(0, 255);
    } else {
      rgb[0] = random(0, 255);  // Max is exclusive but
      rgb[1] = random(0, 255);  // to save that extra byte...
      rgb[2] = random(0, 255);
      
    }
    
  }


}

/*
void showModes() {
  
  switch (mode) {
    case 0:
      digitalWrite(MODE1, HIGH);
      digitalWrite(MODE2, LOW);
      digitalWrite(MODE3, LOW);
      break;
    case 1:
      digitalWrite(MODE1, LOW);
      digitalWrite(MODE2, HIGH);
      digitalWrite(MODE3, LOW);
      break;
    case 2:
      digitalWrite(MODE1, LOW);
      digitalWrite(MODE2, LOW);
      digitalWrite(MODE3, HIGH);
      break;
      
  }
}*/



