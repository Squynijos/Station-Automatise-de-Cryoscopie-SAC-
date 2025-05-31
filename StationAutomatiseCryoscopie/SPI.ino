//Pour la carte SD

bool initSPI(){ //Fonctionnelle
  // Initialisation de la SD
  D(Serial.println("Initializing SD Card"));
  delay(100);
  if(!SD.begin(P_CS_SD)){
    D(Serial.println("\t! Card Mount Failed"));
    return false;
  }

  // Verif avec type de carte
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE){
    D(Serial.println("\t! No SD card attached"));
    return false;
  }

  // Print le type et les détails de la carte
  if(DEBUG){
    Serial.print("\t- SD Card Type: ");
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
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("\t- SD Card Size: %lluMB\n", cardSize);
  }

  //Verif d'existance des différents fichiers, sinon création
  if(!SD.exists(DATA_FILE)){
    createCSV(DATA_FILE, HEADER);
  }

  if(!SD.exists(CONFIG_FILE)){
    createJson(CONFIG_FILE, config);
  }
  else{
    readJson(CONFIG_FILE, config);
  }

  if(firstBoot){
    //Clear previous binary files
    D(Serial.println("Deleting previous bin files"));
    File dir = SD.open("/binary");
    while(true){
      //Opening file
      File file = dir.openNextFile();

      //Verif que c'es une fichier
      if(!file){
        D(Serial.println("\t> No more file"));
        break; //Il n'y a plus de file
      }

      //Get le nom
      const char* fileName = file.name();

      //Lecture et addition des valeurs
      D(Serial.println("\t- Deleting file: " + String(fileName)));
      deleteFile(("/binary/" + String(fileName)).c_str());
    }
  }  
  createDir("/binary");

  //TODO: Clear binary dir

  return true;
}

bool deinitSPI(){
  SD.end();
  return true;
}

//----- Folders -----
void createDir(const char* path){
  D(Serial.printf("Creating Dir: %s\n", path));
  if(SD.mkdir(path)){
    D(Serial.println("\t> Dir created"));
  } else {
    D(Serial.println("\t- mkdir failed"));
  }
}

//----- JSON -----
bool createJson(const char* path, Config &cfg){ //Fonctionnelle
  D(Serial.printf("Writing json: %s\n", path));

  // Open file for writing
  File file = SD.open(path, FILE_WRITE);
  if(!file){
    D(Serial.println("\t! Failed to open file for writing"));
    return false;
  }

  // Allocate a temporary JsonDocument
  JsonDocument doc;

  // Set the default values in the document
  //EX: doc["hostname"] = 0;
  doc["acquisitionParHeure"]        = cfg.acquisitionParHeure;

  doc["mb"]["baud"]                 = cfg.mb.baud;
  doc["mb"]["maxRetry"]             = cfg.mb.maxRetry;
  doc["mb"]["retryDelai"]           = cfg.mb.retryDelai;
  doc["mb"]["timeout"]              = cfg.mb.timeout;

  doc["sat"]["baud"]                = cfg.sat.baud;
  doc["sat"]["maxMsgLength"]        = cfg.sat.maxMsgLength;
  doc["sat"]["transmissionParJour"] = cfg.sat.transmissionParJour;
  doc["sat"]["timeout"]             = cfg.sat.timeout;

  doc["gps"]["baud"]                = cfg.gps.baud;
  doc["gps"]["timeout"]             = cfg.gps.timeout;

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
    D(Serial.println("\t! Failed to write to file"));
    file.close();
    return false;
  }

  file.close();
  return true;
}

bool readJson(const char* path, Config &cfg){ //À TESTER
  D(Serial.printf("Reading json: %s\n", path));

  // Open file for reading
  File file = SD.open(path);
  if(!file){
    D(Serial.println("\t! Failed to open file for writing"));
    return false;
  }

  // Allocate a temporary JsonDocument
  JsonDocument doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error){
    D(Serial.println("\t! Failed to read file, using default configuration"));
    file.close();
    return false;
  }

  // Copy values from the JsonDocument to the Config struct
  //EX: config.port = doc["port"];
  cfg.acquisitionParHeure     = doc["acquisitionParHeure"];

  cfg.mb.baud                 = doc["mb"]["baud"];
  cfg.mb.maxRetry             = doc["mb"]["maxRetry"];
  cfg.mb.retryDelai           = doc["mb"]["retryDelai"];
  cfg.mb.timeout              = doc["mb"]["timeout"];

  cfg.sat.baud                = doc["sat"]["baud"];
  cfg.sat.maxMsgLength        = doc["sat"]["maxMsgLength"];
  cfg.sat.transmissionParJour = doc["sat"]["transmissionParJour"];
  cfg.sat.timeout             = doc["sat"]["timeout"];

  cfg.gps.baud                = doc["gps"]["baud"];
  cfg.gps.timeout             = doc["gps"]["timeout"];

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
  return true;
}

//----- BIN -----
bool createBin(const char* path, DataStruct &ds){ //Fonctionnelle
  D(Serial.printf("Writing bin: %s\n", path));

  File file = SD.open(path, FILE_WRITE);
  if(!file){
    D(Serial.println("\t! Failed to open file for writing"));
    return false;
  }
  if(!file.write(ds.raw, sizeof(ds.raw))){
    D(Serial.println("\t! Write failed"));
    file.close();
    return false;
  }
  file.close();
  return true;
}

bool readBin(const char* path, DataStruct &ds){ //Fonctionnelle
  D(Serial.printf("Reading bin: %s\n", path));

  File file = SD.open(path);
  if(!file){
    D(Serial.println("\t! Failed to open file for reading"));
    return false;;
  }

  if(file.read(ds.raw, sizeof(ds.raw)) == -1){
    D(Serial.println("\t! Erreur de lecture"));
    file.close();
    return false;
  }

  file.close();
  return true;
}

void moyenneBin(DataStruct &ds){ //TODO
  D(Serial.println("Averaging data"));

  //Structure temporaire pour l'ouverture des fichiers
  DataStruct tempData;

  float moyenne = 1.0; //On a toujours au moins la struct de base

  //opening binary folder
  D(Serial.println("\t- Opening folder"));
  File dir = SD.open("/binary");

  //Adding data
  while(true){
    //Opening file
    File file = dir.openNextFile();

    //Verif que c'es une fichier
    if(!file){
      D(Serial.println("\t> No more file"));
      break; //Il n'y a plus de file
    }

    //Get le nom
    const char* fileName = file.name();

    //Lecture et addition des valeurs
    D(Serial.println("\t- Reading file: " + String(fileName)));
    if (readBin(("/binary/" + String(fileName)).c_str(), tempData)){
      D(Serial.println("\t- Adding data"));
      ds.m.tempInt += tempData.m.tempInt;
      ds.m.tempExt += tempData.m.tempExt;

      ds.m.humInt += tempData.m.humInt;
      ds.m.humExt += tempData.m.humExt;

      ds.m.pressExt += tempData.m.pressExt;

      ds.m.lum += tempData.m.lum;

      ds.m.vitVent += tempData.m.vitVent;
      ds.m.dirVent += tempData.m.dirVent;
      moyenne++;
    }

    //Removing file
    file.close();
    deleteFile(("/binary/" + String(fileName)).c_str());
  }

  //###-MOYENNE-###
  D(Serial.println("\t- Division par:" + String(moyenne)));
  ds.m.tempInt = ds.m.tempInt /moyenne;
  ds.m.tempExt = ds.m.tempExt /moyenne;

  ds.m.humInt = ds.m.humInt /moyenne;
  ds.m.humExt = ds.m.humExt /moyenne;

  ds.m.pressExt = ds.m.pressExt /moyenne;

  ds.m.lum = ds.m.lum /moyenne;

  ds.m.vitVent = ds.m.vitVent /moyenne;
  ds.m.dirVent = ds.m.dirVent /moyenne;
}

//----- CSV -----
bool createCSV(const char * path, const char * header){ //Fonctionnelle
  D(Serial.printf("Writing csv: %s\n", path));

  File file = SD.open(path, FILE_WRITE);
  if(!file){
    D(Serial.println("\t! Failed to open file for writing"));
    return false;
  }

  if(!file.print(String(header))){
    D(Serial.println("\t! Write failed"));
    file.close();
    return false;
  }

  file.close();
  return true;
}

bool logCSV(const char* path, DataStruct &ds){ //Fonctionnelle
  D(Serial.printf("Appending to file: %s\n", path));
  
  File file = SD.open(path, FILE_APPEND);
  if(!file){
    D(Serial.println("\t! Failed to open file for appending"));
    return false;
  }

  if(!file.println(formatLog(ds))){
    D(Serial.println("\t! Append failed"));
    file.close();
    return false;
  }

  file.close();
  return true;
}

String formatLog(DataStruct &ds){ //Fonctionnelle
  return (
    String(ds.m.timestamp) + "," +
    String(ds.m.longitude) + "," +
    String(ds.m.latitude) + "," + 
    String(ds.m.hdop) + "," + 
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
bool deleteFile(const char* path){ //Fonctionnelle
  D(Serial.printf("Deleting file: %s\n", path));

  if(!SD.remove(path)){
    D(Serial.println("\t! Delete failed"));
    return false;
  }

  return true;
}



