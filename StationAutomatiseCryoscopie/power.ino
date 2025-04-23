//Pour les alimentation et le sleep

void initPower(){
  pinMode(P_SHDN_12V, OUTPUT);  
  pinMode(P_SHDN_5V, OUTPUT);
  pinMode(P_SHDN_3V3, OUTPUT);  
}

//Pour activer les régulateurs/commutateurs
void enable12V()  {digitalWrite(P_SHDN_12V, HIGH);}
void enable5V()   {digitalWrite(P_SHDN_5V, HIGH);}
void enable3V3()  {digitalWrite(P_SHDN_3V3, HIGH);}

//Pour désactiver les tensions
void disable12V() {digitalWrite(P_SHDN_12V, LOW);}
void disable5V()  {digitalWrite(P_SHDN_5V, LOW);}
void disable3V3() {digitalWrite(P_SHDN_3V3, LOW);}

void readVBat(DataStruct &dataStruct){ //À TESTER
  dataStruct.m.vBat = digitalRead(P_VBAT); //TODO :produit croisé 
}

void goToSleep(){
  //TODO
}