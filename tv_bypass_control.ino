#define buzz 8
#define soc 2
#define panel 4
#define vamp 3
#define sinyal A0

void setup() {
  pinMode(buzz, OUTPUT);    // |
  pinMode(soc, OUTPUT);     // |
  pinMode(panel, OUTPUT);   // -> Define our pins.
  pinMode(vamp, OUTPUT);    // |
  pinMode(sinyal, INPUT);   // |
  
  digitalWrite(buzz, LOW);  // |
  digitalWrite(soc, LOW);   // | -> This section should be adjusted based on the relay modules you use.
  digitalWrite(panel, LOW); // |
  digitalWrite(vamp, HIGH); // |
}

void loop() {
  // If the signal is HIGH at the very beginning, keep the system on standby and restart the loop
  if (digitalRead(sinyal) == HIGH) {
    noTone(buzz);
    return;
  }
  
  // Signal is LOW, trigger sequence starts:
  digitalWrite(soc, HIGH); // Turn on SoC relay
  delay(100);
  digitalWrite(vamp, LOW); // Turn on Amp relay
  delay(100); 
  tone(buzz, 523);         // 1st BIOS beep
  delay(100);
  noTone(buzz);
  delay(200);              // Wait for the signal to stabilize

  // =========================================================================
  // SIGNAL DETECTION AND TIMEOUT LOOP
  // =========================================================================
  bool sinyalYakalandi = false; 
  unsigned long baslangicZamani = millis();

  // Sample the pin quickly for 5 seconds; if a signal arrives, break the loop
  while (millis() - baslangicZamani < 5000) { 
    if (digitalRead(sinyal) == LOW) { 
      sinyalYakalandi = true;
      break; 
    }
  }

  // If the signal does not arrive within 5 seconds, trigger the alarm
  if (!sinyalYakalandi) {
    digitalWrite(panel, LOW); 
    digitalWrite(soc, LOW); 
    digitalWrite(vamp, HIGH); 
    
    for(;;) {
      tone(buzz, 330); // 1st ALARM POINT: Signal failed to arrive alarm
      delay(500); 
      noTone(buzz);
      delay(20);
    }
  }

  // Filter delay for signal stabilization after detection
  delay(50); 
  
  // Optional 10ms delay
  delay(10); 
  // =========================================================================

  // If the signal arrived successfully, provide voltage to the panel
  digitalWrite(panel, HIGH); 
  
  // NOISE SUPPRESSION: The signal pin might flicker briefly when the panel is powered on.
  // Adding 150ms allowance for the signal to stabilize.
  delay(150); 

  // EMERGENCY 2: If the signal is collapsed (HIGH) after the panel is powered on
  if(digitalRead(sinyal) == HIGH) { 
    digitalWrite(panel, LOW); 
    digitalWrite(soc, LOW); 
    digitalWrite(vamp, HIGH); 
    
    for(;;) {
      tone(buzz, 330); // 2nd ALARM POINT: Signal collapsed after panel power-on alarm
      delay(250);      // Slightly faster alarm rhythm (250ms)
      noTone(buzz);
      delay(20);
    }
  }

  // =========================================================================
  // 1-MINUTE OBSERVATION LOOP (To prevent the TV from entering a bootloop)
  // =========================================================================
  for(int z = 0; z < 600; z++) { 
    if(digitalRead(sinyal) == HIGH) {
      digitalWrite(panel, LOW); 
      digitalWrite(soc, LOW); 
      digitalWrite(vamp, HIGH); 
      
      for(;;) {
        tone(buzz, 330); 
        delay(500); 
        noTone(buzz);
        delay(20);
      }
    }
    delay(100); 
  }

  // SAFE SHUTDOWN MONITORING:
  for(;;) {
    if(digitalRead(sinyal) == HIGH) {
      delay(3000); 
      digitalWrite(panel, LOW); 
      delay(100);
      digitalWrite(soc, LOW); 
      delay(100);
      digitalWrite(vamp, HIGH); 
      return; 
    }
    delay(50); 
  }
}