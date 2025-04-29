// //Pour le RTC du ESP32

bool initRTC(){ //À TESTER
  //rtc.begin(false); //true, ramènerait le time a 0
  rtc.setTime(1609459200);
  unixtime = rtc.getEpoch();

  return true;
}

bool readRTC(DataStruct &ds){ //À TESTER
  D(Serial.println("Reading RTC"));

  ds.m.timestamp = rtc.getEpoch();
  D(Serial.println("\t> RTC Time: " + String(ds.m.timestamp)));

  return true;
}

bool setRTC(uint32_t epoch){
  D(Serial.println("Setting RTC"));
  D(Serial.println("\t- Value: " + String(epoch)));
  rtc.setTime(epoch);
}
