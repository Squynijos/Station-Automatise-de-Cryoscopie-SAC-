
bool initSPI(){
  //init SD
  if(!SD.begin(P_CS_SD)){
    Serial.println("Card Mount Failed");
    return false;
  }
  uint8_t cardType = SD.cardType();
    if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    }
  return true;
}

    //Find next available filename
bool newFile(String filename){
    for (int i = 0; i <= 999; i++) {
      String name = filename + String(i) + ".csv";
      if (!SD.exists(name)) {
        filename = name;
        break;
      }
    }

    //Create file and write header
    if (logfile = SD.open(filename, FILE_WRITE)) {
      D(Serial.println("Writing header to " + filename));
      logfile.print(HEADER);
    } else {
      D(Serial.println("Failed to create file " + filename));
    }
  }


bool openNewFile(fs::FS &fs, const char * dirname, const char * fileName){
  filename += "0.csv";
  // File root = fs.open(dirname);
  // auto  path  = '/';
  // auto dir = dirname;
  // path +=  dir;
  // if(!root.isDirectory()){
  //   Serial.println("creating directory");
  //   createDir(fs, path);
  // }
  // path += "/" ; path += fileName;
  // File file = fs.open(path);
  // int counter = 0;
  // while(!file){
  //   path += counter;
  //   counter++;
  // }
  // file.close();
  // return true;
}
