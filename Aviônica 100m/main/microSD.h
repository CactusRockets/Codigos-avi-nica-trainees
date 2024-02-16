/* CONFIGURAÇÕES CARTÃO MICROSD */

#define CS_SDPIN 4

File myFile;

void leitura() {
  Serial.println("Teste de importação de biblioteca!");
}

void setupSd() {
  Serial.println("Inicializando o cartão SD...");
  // verifica se o cartão SD está presente e se pode ser inicializado
  
  while(!SD.begin(CS_SDPIN)) {
    // programa encerrado 
    Serial.println("Falha, verifique se o cartão está presente.");
  }

  // Cria arquivo data.txt e abre
  myFile = SD.open("/data.txt", FILE_WRITE);                        
  // Escreve dados no arquivo
  if (myFile) {
    Serial.println("Gravando dados iniciais!");
    myFile.println("Time, Altitude, Temperature");
    myFile.close();
  } else {
    Serial.println("Error ao abrir data.txt");
  }
}

void writeOnSD(String str) {
  myFile = SD.open("/data.txt", FILE_WRITE);

  if (myFile) {
    Serial.println("(Dados Gravados!)");
    myFile.println(str);
    myFile.close();

  } else {
    Serial.println("Error ao gravar em data.txt");
  }
}
