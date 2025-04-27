//Pour les alimentation et le sleep

void initPower(){ //Fonctionnelle
  pinMode(P_SHDN_12V, OUTPUT);  
  pinMode(P_SHDN_5V, OUTPUT);
  pinMode(P_SHDN_3V3, OUTPUT);  
}

//Pour activer les régulateurs/commutateurs
void enable12V()  {digitalWrite(P_SHDN_12V, HIGH);} //Fonctionnelle
void enable5V()   {digitalWrite(P_SHDN_5V, HIGH);} //Fonctionnelle
void enable3V3()  {digitalWrite(P_SHDN_3V3, HIGH);} //Fonctionnelle

//Pour désactiver les tensions
void disable12V() {digitalWrite(P_SHDN_12V, LOW);}
void disable5V()  {digitalWrite(P_SHDN_5V, LOW);}
void disable3V3() {digitalWrite(P_SHDN_3V3, LOW);}

void readVBat(DataStruct &ds){ //À TESTER
  voltage = analogRead(P_VBAT);
  voltage *= ((1e7 + 1e6) / 1e6); // Multiply back 1 MOhm / (10 MOhm + 1 MOhm)
  voltage *= 3.3; // Multiply by 3.3V reference voltage
  voltage /= 4096; // Convert to voltage
  ds.m.vBat = voltage;
}

void goToSleep(){ //À TESTER
  //Éteindre les sources d'alimentation
  //TODO

  // Configuration des périphériques à conserver en fonction
  //TODO

  // Mettre le esp32 en deep sleep
  // TODO
}
