//Pour la carte SD
bool initSPI(){
  // Initialisation de la SD
  if(!SD.begin(P_CS_SD)){
    Serial.println("Card Mount Failed");
    return false;
  }

  // Verif avec type de carte
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return false;
  }
  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  // Information supplémentaire sur la carte
  // uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  // Serial.printf("SD Card Size: %lluMB\n", cardSize);

  //Verif d'existance des différent fichier
  if(!SD.exists(DATA_FILE)){
    createCSV(DATA_FILE, HEADER);
  }

  if(!SD.exists(CONFIG_FILE)){
    createJson(CONFIG_FILE, config); //TODO: Configs de base + Lecture configs
  }
  else{
    readJson(CONFIG_FILE, config);
  }

  //TODO: Directory pour les bin si existe pas

  return true;
}

// void listDir(const char* dirname, uint8_t levels){
//   Serial.printf("Listing directory: %s\n", dirname);

//   File root = SD.open(dirname);
//   if(!root){
//     Serial.println("Failed to open directory");
//     return;
//   }
//   if(!root.isDirectory()){
//     Serial.println("Not a directory");
//     return;
//   }

//   File file = root.openNextFile();
//   while(file){
//     if(file.isDirectory()){
//       Serial.print("  DIR : ");
//       Serial.println(file.name());
//       if(levels){
//         listDir(SD, file.name(), levels -1);
//       }
//     } else {
//       Serial.print("  FILE: ");
//       Serial.print(file.name());
//       Serial.print("  SIZE: ");
//       Serial.println(file.size());
//     }
//     file = root.openNextFile();
//   }
// }

// void createDir(const char* path){
//   Serial.printf("Creating Dir: %s\n", path);
//   if(SD.mkdir(path)){
//     Serial.println("Dir created");
//   } else {
//     Serial.println("mkdir failed");
//   }
// }

// void removeDir(const char* path){
//   Serial.printf("Removing Dir: %s\n", path);
//   if(SD.rmdir(path)){
//     Serial.println("Dir removed");
//   } else {
//     Serial.println("rmdir failed");
//   }
// }

//----- JSON -----
void createJson(const char* path, Config &cfg){ //TODO
  Serial.printf("Writing json: %s\n", path);

  // Open file for writing
  File file = SD.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }

  //Initializing default values in struct
  Serial.println("Conf: " + String(cfg.acquisitionParHeure));

  // Allocate a temporary JsonDocument
  JsonDocument doc;

  // Set the default values in the document
  //EX: doc["hostname"] = 0;
  doc["acquisitionParHeure"]        = cfg.acquisitionParHeure;
  doc["mb"]["baud"]                 = cfg.mb.baud;
  doc["mb"]["maxRetry"]             = cfg.mb.maxRetry;
  doc["mb"]["timeout"]              = cfg.mb.timeout;
  doc["sat"]["maxMsgLength"]        = cfg.sat.maxMsgLength;
  doc["sat"]["transmissionParJour"] = cfg.sat.transmissionParJour;
  doc["lum"]["offset"]              = cfg.lum.offset;
  doc["lum"]["facteur"]             = cfg.lum.facteur;
  doc["tempInt"]["offset"]          = cfg.tempInt.offset;
  doc["tempInt"]["facteur"]         = cfg.tempInt.facteur;
  doc["humInt"]["offset"]           = cfg.humInt.offset;
  doc["humInt"]["facteur"]          = cfg.humInt.facteur;
  doc["pressInt"]["offset"]         = cfg.pressInt.offset;
  doc["pressInt"]["facteur"]        = cfg.pressInt.facteur;
  doc["tempExt"]["offset"]          = cfg.tempExt.offset;
  doc["tempExt"]["facteur"]         = cfg.tempExt.facteur;
  doc["humExt"]["offset"]           = cfg.humExt.offset;
  doc["humExt"]["facteur"]          = cfg.humExt.facteur;
  doc["pressExt"]["offset"]         = cfg.pressExt.offset;
  doc["pressExt"]["facteur"]        = cfg.pressExt.facteur;
  doc["anemo"]["offset"]            = cfg.anemo.offset;
  doc["anemo"]["facteur"]           = cfg.anemo.facteur;
  doc["girou"]["offset"]            = cfg.girou.offset;
  doc["girou"]["facteur"]           = cfg.girou.facteur;


  // Serialize JSON to file
  if (serializeJson(doc, file) == 0) {
    Serial.println("Failed to write to file");
  }

  file.close();
}

void readJson(const char* path, Config &cfg){ //TODO
  // Open file for reading
  File file = SD.open(path);

  // Allocate a temporary JsonDocument
  JsonDocument doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error){
    Serial.println(F("Failed to read file, using default configuration"));
  }

  // Copy values from the JsonDocument to the Config struct
  //EX: config.port = doc["port"];
  cfg.acquisitionParHeure     = doc["acquisitionParHeure"];
  cfg.mb.baud                 = doc["mb"]["baud"];
  cfg.mb.maxRetry             = doc["mb"]["maxRetry"];
  cfg.mb.timeout              = doc["mb"]["timeout"];
  cfg.sat.maxMsgLength        = doc["sat"]["maxMsgLength"] = ;
  cfg.sat.transmissionParJour = doc["sat"]["transmissionParJour"];
  cfg.lum.offset              = doc["lum"]["offset"];
  cfg.lum.facteur             = doc["lum"]["facteur"];
  cfg.tempInt.offset          = doc["tempInt"]["offset"];
  cfg.tempInt.facteur         = doc["tempInt"]["facteur"];
  cfg.humInt.offset           = doc["humInt"]["offset"];
  cfg.humInt.facteur          = doc["humInt"]["facteur"];
  cfg.pressInt.offset         = doc["pressInt"]["offset"];
  cfg.pressInt.facteur        = doc["pressInt"]["facteur"];
  cfg.tempExt.offset          = doc["tempExt"]["offset"];
  cfg.tempExt.facteur         = doc["tempExt"]["facteur"];
  cfg.humExt.offset           = doc["humExt"]["offset"];
  cfg.humExt.facteur          = doc["humExt"]["facteur"];
  cfg.pressExt.offset         = doc["pressExt"]["offset"];
  cfg.pressExt.facteur        = doc["pressExt"]["facteur"];
  cfg.anemo.offset            = doc["anemo"]["offset"];
  cfg.anemo.facteur           = doc["anemo"]["facteur"];
  cfg.girou.offset            = doc["girou"]["offset"];
  cfg.girou.facteur           = doc["girou"]["facteur"];

  // Close the file (Curiously, File's destructor doesn't close the file)
  file.close();
}

//----- BIN -----
void createBin(const char* path, DataStruct &ds){ //Fonctionnelle
  Serial.printf("Writing file: %s\n", path);

  File file = SD.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.write(ds.raw, sizeof(ds.raw))){
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void readBin(const char* path, DataStruct &ds){ //Fonctionnelle
  Serial.printf("Reading file: %s\n", path);

  File file = SD.open(path);
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }

  if(file.read(ds.raw, sizeof(ds.raw)) == -1){
    Serial.println("Erreur de lecture");
  }

  file.close();
}

//----- CSV -----
void createCSV(const char * path, const char * header){
  Serial.printf("Writing file: %s\n", path);

  File file = SD.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }
  Serial.println(String(header));
  if(file.print(String(header))){
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void logCSV(const char* path, DataStruct &ds){ //Fonctionnelle
  Serial.printf("Appending to file: %s\n", path);
  
  File file = SD.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.println(formatLog(ds))){
      Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

String formatLog(DataStruct &ds){ //Fonctionnelle
  return (
    String(ds.m.timestamp) + "," +
    String(ds.m.longitude) + "," +
    String(ds.m.latitude) + "," + 
    String(ds.m.vBat) + "," + 
    String(ds.m.tempInt) + "," +
    String(ds.m.pressInt) + "," +
    String(ds.m.humInt) + "," +
    String(ds.m.accelX) + "," +
    String(ds.m.accelY) + "," +
    String(ds.m.accelZ) + "," +
    String(ds.m.tempExt) + "," +
    String(ds.m.pressExt) + "," +
    String(ds.m.humExt) + "," +
    String(ds.m.vitVent) + "," +
    String(ds.m.dirVent) + "," +
    String(ds.m.angleVent) + "," +
    String(ds.m.lum)
  );
}

//----- DEL -----
void deleteFile(const char* path){ //Fonctionnelle
  Serial.printf("Deleting file: %s\n", path);
  if(SD.remove(path)){
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

