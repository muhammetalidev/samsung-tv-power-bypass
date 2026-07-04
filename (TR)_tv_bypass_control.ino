#define buzz 8
#define soc 2
#define panel 4
#define vamp 3
#define sinyal A0

void setup() {
  pinMode(buzz, OUTPUT);    // |
  pinMode(soc, OUTPUT);     // |
  pinMode(panel, OUTPUT);   // -> Pinlerimizi tanımlıyoruz.
  pinMode(vamp, OUTPUT);    // |
  pinMode(sinyal, INPUT);   // |
  
  digitalWrite(buzz, LOW);  // |
  digitalWrite(soc, LOW);   // | -> Bu kısım kullandığınız röle kartlarına göre değiştirilecek kısım.
  digitalWrite(panel, LOW); // |
  digitalWrite(vamp, HIGH); // |
}

void loop() {
  // En başta sinyal HIGH ise sistemi beklemeye al ve direkt loop'un başına dön
  if (digitalRead(sinyal) == HIGH) {
    noTone(buzz);
    return;
  }
  
  // Sinyal LOW oldu, tetikleme başlıyor:
  digitalWrite(soc, HIGH); // SoC rölesini aç
  delay(100);
  digitalWrite(vamp, LOW); // Amfi Rölesini aç
  delay(100); 
  tone(buzz, 523);         // 1. BIOS sesi
  delay(100);
  noTone(buzz);
  delay(200);              // Sinyalin gitmesini bekle

  // =========================================================================
  // SİNYAL YAKALAMA VE ZAMAN AŞIMI DÖNGÜSÜ
  // =========================================================================
  bool sinyalYakalandi = false; 
  unsigned long baslangicZamani = millis();

  // 5 saniye boyunca pini hızlıca örnekle, sinyal gelirse döngüyü kır
  while (millis() - baslangicZamani < 5000) { 
    if (digitalRead(sinyal) == LOW) { 
      sinyalYakalandi = true;
      break; 
    }
  }

  // Eğer sinyal 5 saniye içinde hiç gelmediyse alarm çal
  if (!sinyalYakalandi) {
    digitalWrite(panel, LOW); 
    digitalWrite(soc, LOW); 
    digitalWrite(vamp, HIGH); 
    
    for(;;) {
      tone(buzz, 330); // 1. ALARM NOKTASI: Sinyal hiç gelmedi alarmı
      delay(500); 
      noTone(buzz);
      delay(20);
    }
  }

  // Sinyal yakalandıktan sonra kararlı hale gelmesi için filtre süresi
  delay(50); 
  
  // İsteğe bağlı 10 ms gecikme
  delay(10); 
  // =========================================================================

  // Sinyal sorunsuz geldiyse panel voltajını ver
  digitalWrite(panel, HIGH); 
  
  // PARAZİT ENGELLEME: Panel açıldığı an voltaj sıçramasından dolayı 
  // sinyal pini anlık sapıtabilir. Sinyalin kendine gelmesi için 150 ms avans veriyoruz.
  delay(150); 

  // ACİL DURUM 2: Panel açıldıktan sonra sinyal hala çökmüş durumdaysa (HIGH ise)
  if(digitalRead(sinyal) == HIGH) { 
    digitalWrite(panel, LOW); 
    digitalWrite(soc, LOW); 
    digitalWrite(vamp, HIGH); 
    
    for(;;) {
      tone(buzz, 330); // 2. ALARM NOKTASI: Panel açıldıktan sonra çöktü alarmı
      delay(250);      // Bu alarmın ritmi biraz daha hızlı (250ms)
      noTone(buzz);
      delay(20);
    }
  }

  // =========================================================================
  // 1 DAKİKALIK GÖZLEM DÖNGÜSÜ (Televizyonun bootloopa girmesini önlemek için)
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

  // GÜVENLİ KAPANIŞ TAKİBİ:
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
