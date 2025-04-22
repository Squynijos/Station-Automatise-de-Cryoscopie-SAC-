//Pour la carte SD
void initSPI(){
  SD.begin(P_CS_SD);
}


void logSD(String path, String content){
  File file = SD.open(path, FILE_WRITE);
  file.println(content);
  file.close();
}

void readSD(String path){
  File file = SD.open(path, FILE_WRITE);
  file.read();
}
