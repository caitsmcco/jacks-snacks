// Group 15 "Jacks snacks" Arduino Project

// Setup:
// IR reciever
//    white = 7, green = 3.3V, blue = ground
// fan
//    red = 3, black = ground
// servo
//    orange = 9, black = ground, red = 5v

#include <Servo.h>     // servo config v1.2.2
#include <IRremote.h>  // IR Receiver Configuration  v3.6.1
int pos = 180;
const int RECV_PIN = 7;                                                          // Pin connected to the IR receiver
const unsigned long correctSequence[] = { 0xE31CFF00, 0xAD52FF00, 0xF30CFF00 };  // HEX for 5, 8, 1
const int sequenceLength = sizeof(correctSequence) / sizeof(correctSequence[0]);
unsigned long receivedSequence[sequenceLength];  // Store the received sequence
int currentIndex = 0;                            // Track progress in the sequence
IRrecv irrecv(RECV_PIN);
decode_results results;  // Servo and Fan Motor Configuration
Servo doorServo;         // Servo motor for the snack box door
const int fanPin = 3;    // Fan motor connected to pin 3
void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);  // Initialize IR Receiver
  irrecv.enableIRIn();
  Serial.println("System ready. Use remote to enter the sequence.");  // Initialize Fan Motor
  pinMode(fanPin, OUTPUT);
  digitalWrite(fanPin, LOW);  // Ensure fan motor is off
}

void loop() {
  if (irrecv.decode()) {
    unsigned long receivedCode = irrecv.decodedIRData.decodedRawData;  // Get IR code    // Ignore '0' codes
    if (receivedCode == 0) {
      Serial.println("Ignored: Received '0' code.");
      irrecv.resume();
      return;  // Do nothing and exit the loop
    }          // Print received code
    Serial.print("Received HEX Code: ");
    Serial.println(receivedCode, HEX);  // Store the code in the sequence
    if (currentIndex < sequenceLength) {
      receivedSequence[currentIndex] = receivedCode;
      currentIndex++;
      Serial.print("Step ");
      Serial.print(currentIndex);
      Serial.print(" of ");
      Serial.println(sequenceLength);  // Check if the full sequence has been entered
      if (currentIndex == sequenceLength) {
        evaluateSequence();  // Check if the sequence is correct
        currentIndex = 0;    // Reset for the next attempt
      }
    } else {
      Serial.println("Sequence overflow! Resetting.");
      currentIndex = 0;  // Reset sequence on overflow
    }
    irrecv.resume();  // Prepare for the next signal
  }
}  // Function to Check the Entered Sequence
void evaluateSequence() {
  Serial.println("Evaluating sequence...");
  for (int i = 0; i < sequenceLength; i++) {
    if (receivedSequence[i] != correctSequence[i]) {
      Serial.println("FAIL: Incorrect sequence! Activating Jack-in-the-box...");
      jackInTheBox();  // Trigger FAIL logic
      return;          // Exit the function
    }
  }  // If the sequence is correct
  Serial.println("PASS: Correct sequence! Unlocking snack box...");
  unlockSnackBox();  // Trigger PASS logic
}  // Function to Unlock the Snack Box (PASS Logic)
void unlockSnackBox() {
  digitalWrite(fanPin, LOW);   // Stop the fan motor

  doorServo.attach(9);  // Attach the servo motor    

  doorServo.write(80);
  delay(2000);
  for (pos = 120; pos >= 0; pos -= 1) {
      doorServo.write(pos);
      delay(50);
  }
  delay(2000);
  doorServo.detach();  // Detach the servo to avoid conflicts  digitalWrite(fanPin, LOW);   // Ensure fan motor is off
  Serial.println("Snack box unlocked.");
}  
// Function to Activate Jack-in-the-box (FAIL Logic)
void jackInTheBox() {
  digitalWrite(fanPin, HIGH);  // Run the fan motor to wind down the spring
  delay(120);                 // Run motor for 10 seconds
  digitalWrite(fanPin, LOW);   // Stop the fan motor
  Serial.println("Jack-in-the-box activated.");
}
