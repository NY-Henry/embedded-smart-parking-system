// --- 1. HARDWARE PIN DEFINITIONS ---
// 'const' means these pin numbers will never change
const int greenLED = 2;
const int redLED = 3;
const int yellowLED = 4;
const int trigPin = 9;
const int echoPin = 10;

// --- 2. SYSTEM STATE VARIABLES ---
long duration;           // Microseconds it takes for sound to bounce back
int distance;            // Calculated distance in cm
bool isOccupied = false; // Tracks if the spot has a car in it

// --- 3. TIMER VARIABLES (The Non-Blocking Timer) ---
unsigned long parkedStartTime = 0;             // Records the exact moment a car arrives
const unsigned long warningThreshold = 10000;  // 10,000 milliseconds = 10 seconds

void setup() {
  // Start the serial monitor so we can print debug text (like console.log)
  Serial.begin(9600);

  // Define which pins send data out (OUTPUT) and which read data in (INPUT)
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
}

// --- HELPER FUNCTION: Get the distance in cm ---
int getDistance() {
  // 1. Clear the trigger pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // 2. Fire the sound wave (high for 10 microseconds)
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // 3. Listen for the echo and calculate distance
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; // Speed of sound math
  
  return distance;
}

// --- MAIN EVENT LOOP (Runs continuously) ---
void loop() {
  int currentDistance = getDistance();
  
  // Print to the Serial Monitor so we can see the hidden math
  Serial.print("Distance: ");
  Serial.print(currentDistance);
  Serial.println(" cm");

  // === STATE A: FREE (Distance > 100cm) ===
  if (currentDistance > 100) {
    digitalWrite(greenLED, HIGH); // Green ON
    digitalWrite(redLED, LOW);    // Red OFF
    digitalWrite(yellowLED, LOW); // Yellow OFF
    
    isOccupied = false;           // Reset state
    parkedStartTime = 0;          // Reset timer
  }
  
  // === STATE B & C: OCCUPIED & WARNING (Distance <= 100cm) ===
  else {
    // If a car JUST arrived this exact millisecond
    if (isOccupied == false) {
      isOccupied = true;
      parkedStartTime = millis(); // START THE STOPWATCH!
    }

    // Turn on the standard Occupied UI
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);

    // Calculate how long they have been sitting there
    unsigned long timeParked = millis() - parkedStartTime;
    
    // Check if they exceeded the 10-second limit
    if (timeParked >= warningThreshold) {
      
      // BLINK TRICK: This blinks the Yellow LED every 500ms 
      // WITHOUT using delay() so the sensor keeps reading!
      if ((millis() / 500) % 2 == 0) {
         digitalWrite(yellowLED, HIGH);
      } else {
         digitalWrite(yellowLED, LOW);
      }
      
    } else {
      digitalWrite(yellowLED, LOW); // Still under 10 seconds
    }
  }
  
  // Tiny pause to keep the Tinkercad simulator from lagging
  delay(50);
}