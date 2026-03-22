// Pin assignments (must be PWM capable for steady brightness phase)
const int led1 = 9;   // LED 1 (PWM)
const int led2 = 10;  // LED 2 (PWM)
const int led3 = 11;  // LED 3 (PWM)
const int led4 = 8;   // LED 4 (always ON)
const int led5 = 3;   // LED 5 (PWM fade in/out faster now)
const int led6 = 12;  // LED 6 (always ON)

// Timing control
unsigned long stateStartTime;
bool inAnimation = true;  // true = animation, false = steady

// For LED timing (animation phase)
unsigned long prev1 = 0;  // LED 1 rapid flash
unsigned long prev2 = 0;  // LED 2 sporadic flash
unsigned long prev3 = 0;  // LED 3 regular flash

int state1 = LOW;
int state2 = LOW;
int state3 = LOW;

unsigned long interval1 = 100;   // LED 1 rapid flash (100ms)
unsigned long interval2 = 0;     // LED 2 random flash interval
unsigned long interval3 = 500;   // LED 3 1-second cycle (500ms on / off)

// Durations
const unsigned long animationDuration = 3000; // 3 seconds animation
unsigned long steadyDuration = 0;             // randomized later

// For LED5 fade effect
int brightness5 = 0;        // Current brightness
int fadeAmount5 = 10;       // Step size (was 2 → faster fade now)
unsigned long prevFade5 = 0;
const unsigned long fadeInterval5 = 15;  // ms per step (was 20 → faster)

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(led6, OUTPUT);

  // LED4 and LED6 are always ON
  digitalWrite(led4, HIGH);
  digitalWrite(led6, HIGH);

  randomSeed(analogRead(A0));  // Seed randomness (if A0 floating)
  stateStartTime = millis();
  interval2 = random(100, 500);  // Initial random interval for LED 2
}

void loop() {
  unsigned long now = millis();

  // --- LED5 independent fast fade in/out ---
  if (now - prevFade5 >= fadeInterval5) {
    prevFade5 = now;

    // Constrain brightness to valid PWM range
    brightness5 = constrain(brightness5, 0, 255);
    analogWrite(led5, brightness5);

    // Update brightness for next step
    brightness5 += fadeAmount5;

    // Reverse direction when hitting limits
    if (brightness5 <= 0 || brightness5 >= 255) {
      fadeAmount5 = -fadeAmount5;
    }
  }

  if (inAnimation) {
    // --- Animation Phase ---
    // LED 1: rapid flash (100ms interval)
    if (now - prev1 >= interval1) {
      prev1 = now;
      state1 = !state1;
      digitalWrite(led1, state1);
    }

    // LED 2: sporadic flash with random interval
    if (now - prev2 >= interval2) {
      prev2 = now;
      state2 = !state2;
      digitalWrite(led2, state2);
      interval2 = random(100, 600); // change interval randomly
    }

    // LED 3: regular 1s cycle (500ms on / 500ms off)
    if (now - prev3 >= interval3) {
      prev3 = now;
      state3 = !state3;
      digitalWrite(led3, state3);
    }

    // Switch to steady brightness after animation duration
    if (now - stateStartTime >= animationDuration) {
      inAnimation = false;
      stateStartTime = now; // reset for steady duration

      // Pick random duration between 3–8 seconds
      steadyDuration = random(3000, 8001);

      // Set steady brightness for LED1 & LED2
      analogWrite(led1, 255); // full intensity
      analogWrite(led2, 127); // half intensity

      // Reset LED3 timer for flashing during steady mode
      prev3 = now;
      state3 = LOW;
      digitalWrite(led3, state3);
    }

  } else {
    // --- Steady Brightness Phase ---
    // LED1 & LED2 stay at fixed PWM levels

    // LED3 keeps flashing (same 500ms cycle as animation)
    if (now - prev3 >= interval3) {
      prev3 = now;
      state3 = !state3;
      digitalWrite(led3, state3);
    }

    // End steady state after random duration
    if (now - stateStartTime >= steadyDuration) {
      // Go back to animation
      inAnimation = true;
      stateStartTime = now;

      // Reset timers for animation restart
      prev1 = prev2 = prev3 = now;
      state1 = state2 = state3 = LOW;
      digitalWrite(led1, state1);
      digitalWrite(led2, state2);
      digitalWrite(led3, state3);
      interval2 = random(100, 500); // fresh random interval for LED2
    }
  }
}
