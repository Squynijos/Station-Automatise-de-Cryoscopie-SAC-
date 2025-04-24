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
  //if(!SD.exists(DATA_FILE)){
    createTxt(SD, DATA_FILE, HEADER);
  //}

  if(!SD.exists(CONFIG_FILE)){
    createTxt(SD, CONFIG_FILE, ""); //TODO: Configs de base + Lecture configs
  }

  //TODO: Directory pour les bin si existe pas

  return true;
}

// void listDir(fs::FS &fs, const char* dirname, uint8_t levels){
//   Serial.printf("Listing directory: %s\n", dirname);

//   File root = fs.open(dirname);
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
//         listDir(fs, file.name(), levels -1);
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

// void createDir(fs::FS &fs, const char* path){
//   Serial.printf("Creating Dir: %s\n", path);
//   if(fs.mkdir(path)){
//     Serial.println("Dir created");
//   } else {
//     Serial.println("mkdir failed");
//   }
// }

// void removeDir(fs::FS &fs, const char* path){
//   Serial.printf("Removing Dir: %s\n", path);
//   if(fs.rmdir(path)){
//     Serial.println("Dir removed");
//   } else {
//     Serial.println("rmdir failed");
//   }
// }

void readBin(fs::FS &fs, const char* path, DataStruct &ds){ //Fonctionnelle
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }

  if(file.read(ds.raw, sizeof(ds.raw)) == -1){
    Serial.println("Erreur de lecture");
  }

  file.close();
}

void createTxt(fs::FS &fs, const char * path, const char * header){
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
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

void createBin(fs::FS &fs, const char* path, DataStruct &ds){ //Fonctionnelle
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
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

void logCSV(fs::FS &fs, const char* path, DataStruct &ds){ //Fonctionnelle
  Serial.printf("Appending to file: %s\n", path);
  
  File file = fs.open(path, FILE_APPEND);
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

void deleteFile(fs::FS &fs, const char* path){ //Fonctionnelle
  Serial.printf("Deleting file: %s\n", path);
  if(fs.remove(path)){
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
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
