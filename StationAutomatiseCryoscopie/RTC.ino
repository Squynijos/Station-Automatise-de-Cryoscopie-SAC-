//Pour le RTC du ESP32

bool initRTC(){ //À TESTER
  rtc.begin(false); //true, ramènerait le time a 0
  unixtime = rtc.getEpoch();

  return true;
}

bool readRTC(DataStruct &ds){ //À TESTER
  D(Serial.println("Reading RTC"));

  ds.m.timestamp = rtc.getEpoch();
  D(Serial.println("\t> RTC Time: " + String(ds.m.timestamp)));

  return true;
}
