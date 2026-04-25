// =============================================================================
// SMART PARKING SPOT MONITOR
// Uses an ultrasonic sensor to detect a parked vehicle and warns the driver
// with a blinking yellow LED if they exceed the allowed parking duration.
// =============================================================================


// --- 1. HARDWARE PIN ASSIGNMENTS ---
// These map our Arduino's physical pins to meaningful names.
// 'const' ensures these values are locked in at compile time and cannot be
// accidentally reassigned anywhere else in the program.
const int PIN_LED_AVAILABLE   = 2;   // Green LED  -> spot is free
const int PIN_LED_OCCUPIED    = 3;   // Red LED    -> spot is taken
const int PIN_LED_OVERTIME    = 4;   // Yellow LED -> driver has overstayed
const int PIN_ULTRASONIC_TRIG = 9;   // Sends out the ultrasonic pulse
const int PIN_ULTRASONIC_ECHO = 10;  // Receives the bounced-back pulse


// --- 2. SYSTEM STATE VARIABLES ---
// These hold the live status of the parking spot as the program runs.
long  echoTravelTimeMicros;    // How long (in microseconds) the sound wave took to return
int   distanceToObjectCm;      // The computed distance to whatever is in front of the sensor
bool  spotIsOccupied = false;  // True when a vehicle is currently detected in the spot


// --- 3. NON-BLOCKING TIMER VARIABLES ---
// We use millis() instead of delay() so the sensor can keep scanning
// even while we are tracking how long the car has been parked.
unsigned long timeVehicleArrivedMs = 0;             // Timestamp (ms since boot) of when the car pulled in
const unsigned long MAX_PARKING_DURATION_MS = 10000; // 10-second limit before the overtime warning kicks in


// =============================================================================
// SETUP — runs exactly once when the Arduino powers on or resets
// =============================================================================
void setup() {
  // Open the serial connection at 9600 baud so we can stream debug
  // information to the Serial Monitor (think of this as console.log).
  Serial.begin(9600);

  // Configure the ultrasonic sensor pins:
  //   TRIG is an OUTPUT because we send a pulse OUT of it.
  //   ECHO is an INPUT  because we listen for a pulse coming back IN.
  pinMode(PIN_ULTRASONIC_TRIG, OUTPUT);
  pinMode(PIN_ULTRASONIC_ECHO, INPUT);

  // All three LEDs are OUTPUTS — we are driving them, not reading them.
  pinMode(PIN_LED_AVAILABLE, OUTPUT);
  pinMode(PIN_LED_OCCUPIED,  OUTPUT);
  pinMode(PIN_LED_OVERTIME,  OUTPUT);
}


// =============================================================================
// HELPER: measureDistanceCm()
// Fires a brief ultrasonic pulse, times the echo, and converts that time
// into a physical distance in centimeters.
// =============================================================================
int measureDistanceCm() {
  // Step 1: Make sure the trigger line is LOW so we start from a clean state.
  // A 2 microsecond settle time prevents leftover signal from skewing readings.
  digitalWrite(PIN_ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);

  // Step 2: Send a precise 10-microsecond HIGH pulse.
  // This is the "ping" — the sensor emits an ultrasonic burst at this moment.
  digitalWrite(PIN_ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_ULTRASONIC_TRIG, LOW);

  // Step 3: pulseIn() blocks until the ECHO pin goes HIGH then LOW again,
  // returning how many microseconds the round-trip took.
  echoTravelTimeMicros = pulseIn(PIN_ULTRASONIC_ECHO, HIGH);

  // Step 4: Convert time-of-flight into distance.
  //   - Speed of sound ≈ 0.034 cm/microsecond
  //   - Divide by 2 because the sound travels TO the object AND back
  distanceToObjectCm = echoTravelTimeMicros * 0.034 / 2;

  return distanceToObjectCm;
}


// =============================================================================
// LOOP — Arduino's main event loop. Runs forever, as fast as possible.
// =============================================================================
void loop() {
  // Take a fresh reading on every iteration so our decisions are based on
  // the current state of the parking spot, not stale data.
  int currentDistanceCm = measureDistanceCm();

  // Stream the reading to the Serial Monitor for live debugging.
  Serial.print("Distance: ");
  Serial.print(currentDistanceCm);
  Serial.println(" cm");


  // ---------------------------------------------------------------------------
  // STATE A — SPOT IS AVAILABLE (nothing within 100 cm of the sensor)
  // ---------------------------------------------------------------------------
  if (currentDistanceCm > 100) {
    // Show the "free" indicator and switch off the others.
    digitalWrite(PIN_LED_AVAILABLE, HIGH);
    digitalWrite(PIN_LED_OCCUPIED,  LOW);
    digitalWrite(PIN_LED_OVERTIME,  LOW);

    // Reset our occupancy tracking so the next vehicle starts a fresh timer.
    spotIsOccupied        = false;
    timeVehicleArrivedMs  = 0;
  }

  // ---------------------------------------------------------------------------
  // STATE B / C — SPOT IS OCCUPIED (something is within 100 cm)
  // ---------------------------------------------------------------------------
  else {
    // Detect the exact transition from "free" to "occupied" and stamp
    // the arrival time. This branch only runs ONCE per parking session.
    if (spotIsOccupied == false) {
      spotIsOccupied       = true;
      timeVehicleArrivedMs = millis();  // Start the parking stopwatch
    }

    // Standard "occupied" indicator: red ON, green OFF.
    digitalWrite(PIN_LED_AVAILABLE, LOW);
    digitalWrite(PIN_LED_OCCUPIED,  HIGH);

    // Work out how long the vehicle has been sitting in the spot.
    unsigned long elapsedParkedTimeMs = millis() - timeVehicleArrivedMs;

    // Has the driver exceeded the allowed parking duration?
    if (elapsedParkedTimeMs >= MAX_PARKING_DURATION_MS) {

      // NON-BLOCKING BLINK:
      // (millis() / 500) increments by 1 every 500 ms, and modulo 2 alternates
      // between 0 and 1, giving us a clean ON/OFF cycle every half second
      // WITHOUT pausing the program — so the sensor keeps scanning.
      if ((millis() / 500) % 2 == 0) {
        digitalWrite(PIN_LED_OVERTIME, HIGH);
      } else {
        digitalWrite(PIN_LED_OVERTIME, LOW);
      }

    } else {
      // Still within the allowed window — keep the warning LED off.
      digitalWrite(PIN_LED_OVERTIME, LOW);
    }
  }

  // A small pause keeps the Tinkercad simulator from lagging.
  // 50 ms still gives us 20 readings per second — plenty for this use case.
  delay(50);
}