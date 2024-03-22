#include <SPI.h>
#include <SD.h>
#include <MIDI.h>

extern volatile unsigned long timer0_millis;

MIDI_CREATE_DEFAULT_INSTANCE();

File myFile;

int coluna = 0;
int linhaAtual = 0;
const int numBotoes = 8;
int notas[numBotoes] = { 60, 62, 64, 65, 67, 69, 71, 72};
const int botoes[numBotoes] = { 9, 8, 7, 6, 5, 10, 3, 2};
boolean botoesValor[numBotoes] = { false, false, false, false, false, false, false, false};
unsigned long tempoBouce[numBotoes] = { 0, 0, 0, 0, 0, 0, 0, 0};
unsigned long tempoLinha = 0;
unsigned long tempoMem = 0;
unsigned long tempoMem12 = 0;
unsigned long linhasLedTempo[200];
int indice[200];
int indAtual = 0;
boolean modo[200];
boolean colunasLedAtivo[numBotoes][numBotoes];
const int linhasLed[numBotoes] = { 22, 24, 26, 28, 30, 32, 34, 36};
const int colunasLed[numBotoes] = { A0, A1, A2, A3, A7, A4, A6, A5};
unsigned long simulTempo = 0;
unsigned long tempoAciona[numBotoes] = { 0, 0, 0, 0, 0, 0, 0, 0};
int botaOn[numBotoes] = { 0, 0, 0, 0, 0, 0, 0, 0};
unsigned long bounce = 10;
String txt = "";
String txtGravado = "";
boolean gravar = false;
boolean animar = false;
unsigned long temporaria = 0;
unsigned long tempoAnima = 0;
int anColuna[8] = { 0, 0, 0, 0, 0, 0, 0, 0};
int anFutura[8] = { 0, 0, 0, 0, 0, 0, 0, 0};
unsigned long anTempo[8] = { 0, 0, 0, 0, 0, 0, 0};
unsigned long anTemporaria[8] = { 0, 0, 0, 0, 0, 0, 0};
boolean anStatus[8] = { false, false, false, false, false, false, false, false};
boolean fimStatus[8] = { false, false, false, false, false, false, false, false};
int linhaAnima = 0;
int futuraColuna = 0;
boolean animaLed = false;
int musicaGrav = 0;

void setup() {
  
  MIDI.begin();
  Serial.begin(9600);
  
  for(int i = 0; i < numBotoes; i++){
    pinMode(botoes[i],INPUT);
    pinMode(colunasLed[i],OUTPUT);
    pinMode(linhasLed[i],OUTPUT);
  }

   pinMode(11,INPUT);
   pinMode(12,INPUT);
   //previa();
   tempoLinha = millis();
   tempoMem = millis();
   tempoMem12 = millis();
   
}

void loop() {
  
 if((millis() - tempoMem) > 500){
  if (digitalRead(11)){
    tempoMem = millis();
    if(gravar){
      gravar = false;
      //Serial.println(txt);
      String nome = ultimoTxt();//nome do novo arquivo
      GravarSd( nome, txt);//grava o texto no novo arquivo
      
    }else{
      
      zeraMillis();
      txt = "";
      tempoMem = millis();
      gravar = true;
      simulTempo = millis();
      
      //Serial.println("On");
    }
  }
 }

 if((millis() - tempoMem12) > 500){
  
  if (digitalRead(12)){
    
    if(animar){
      animaLed = false;
      animar = false;
      Apagar();
      
    }else{
     
      
      String musTxt =leituraTxts(musicaGrav);
      txtGravado = leitura("musica2.txt");
      manipulaTxt();

      zeraMillis();
      tempoMem12 = millis();
      animaLed = true;
      animar = true;
      //Serial.println(musTxt);
      musicaGrav++;
     
    }
    
    tempoMem12 = millis();
  }
 }
  
 for(int i = (numBotoes - 1); i >= 0; i--){//botoes
  
  if((millis() - tempoBouce[i]) > 10){
    
    tempoBouce[i] = millis();
    
    if (digitalRead(botoes[i]) != botoesValor[i]){
      botoesValor[i] = digitalRead(botoes[i]);
      
      if (digitalRead(botoes[i])){
        //Serial.println("Note T On");
        
        MIDI.sendNoteOn(notas[i], 127, 1);
        
        if(gravar){
          montaTxt(i,1);
        }
       
      }else{
        //Serial.println("Note T Off");
        MIDI.sendNoteOff(notas[i], 0, 1);
        
        if(gravar){
          montaTxt(i,0);
        }
        
      }
   }
   
  }
  if(animaLed){
    Leds(i);
  }
  
 
 }

 if(animaLed){
   if(coluna == 7){
      coluna = 0;
      
    }else{
      coluna ++;
    }
  }
 
}

void zeraMillis(){
  noInterrupts();
  timer0_millis = 0;
  interrupts();
}

void montaTxt(int indice, int on){
  txt +="=";
  txt += millis() - simulTempo;
  botaOn[indice] = on;
  txt += "-";
  txt += indice;
  txt += ":";
  txt += botaOn[indice];
  txt +="\n";
  
}

void VerificaColuna(){
  digitalWrite(linhasLed[0], fimStatus[coluna]);
}

void Leds(int l){
  
  if(l == 7){//liga colunas.;) ok!
      Apagar();
      digitalWrite(colunasLed[coluna],HIGH);
    }
  
  VerificaColuna();
  
  if((millis()- tempoLinha) >= linhasLedTempo[linhaAtual]){//liga linhas
    
      fimStatus[indice[linhaAtual]] = modo[linhaAtual];
       
      if(linhaAtual == indAtual){
         tempoLinha = millis();
         linhaAtual = 0;
          
      }else{
         linhaAtual++;
      }
         
    }
    
   
   
   //animaçao futura
      
   
          
         // Serial.println(linhaAtual);
         // Serial.println(indAtual);
           if((linhaAtual == indAtual)&&(!anStatus[indice[0]])){
               if(modo[indice[0]]){
                  futuraColuna = indice[0];
                  anTempo[futuraColuna] = linhasLedTempo[0]/7;
                  anFutura[futuraColuna] = futuraColuna;
                  anColuna[futuraColuna] = 7;
                  anStatus[futuraColuna] = true;
                  anTemporaria[futuraColuna] = millis();
               }
              //Serial.println("I");
            }else{
              
                if((!modo[linhaAtual])&&(!anStatus[indice[linhaAtual]])){

                  if(modo[linhaAtual +1]){
                    
                    futuraColuna = indice[linhaAtual+1];
                    anFutura[futuraColuna] = indice[linhaAtual + 1];
                    anTempo[futuraColuna] = (linhasLedTempo[linhaAtual + 1] - linhasLedTempo[linhaAtual])/7;
                   // Serial.println(linhasLedTempo[linhaAtual]);
                    //Serial.println(linhasLedTempo[linhaAtual + 1]);
                    //Serial.println((linhasLedTempo[linhaAtual + 1] - linhasLedTempo[linhaAtual])/7);
                    anColuna[futuraColuna] = 7;
                    anStatus[futuraColuna] = true;
                    anTemporaria[futuraColuna] = millis();
                    //Serial.println("T");
                    if(modo[linhaAtual +2]){
                      futuraColuna = indice[linhaAtual+2];
                      anFutura[futuraColuna] = indice[linhaAtual + 2];
                      anTempo[futuraColuna] = (linhasLedTempo[linhaAtual + 1] - linhasLedTempo[linhaAtual])/7;
                      anColuna[futuraColuna] = 7;
                      anStatus[futuraColuna] = true;
                      anTemporaria[futuraColuna] = millis();
                     // Serial.println("T2");
                        if(modo[linhaAtual +3]){
                          futuraColuna = indice[linhaAtual+3];
                          anFutura[futuraColuna] = indice[linhaAtual + 3];
                          anTempo[futuraColuna] = (linhasLedTempo[linhaAtual + 1] - linhasLedTempo[linhaAtual])/7;
                          anColuna[futuraColuna] = 7;
                          anStatus[futuraColuna] = true;
                          anTemporaria[futuraColuna] = millis();
                          
                            if(modo[linhaAtual +4]){
                              futuraColuna = indice[linhaAtual+4];
                              anFutura[futuraColuna] = indice[linhaAtual + 4];
                              anTempo[futuraColuna] = (linhasLedTempo[linhaAtual + 1] - linhasLedTempo[linhaAtual])/7;
                              anColuna[futuraColuna] = 7;
                              anStatus[futuraColuna] = true;
                              anTemporaria[futuraColuna] = millis();
                            }
                          }
                       }
                    
                    }
                      
                }
             
            }
            
        animacao();
}

void animacao(){
  
    digitalWrite(linhasLed[anColuna[coluna]], anStatus[coluna]);
     // Serial.println(anTempo[coluna]);
    if(((millis() - anTemporaria[coluna]) >= anTempo[coluna])&&(anStatus[coluna])){
      //Serial.println(coluna);
      anTemporaria[coluna] = millis();
     
      if(anColuna[coluna] > 1){
         anColuna[coluna]--;
         
      }else{
        anStatus[coluna] = false;
        anTempo[coluna] = 0;
        anColuna[coluna] = 0;
      }
    }
      //Serial.println(millis() - anTemporaria[coluna]);
     // Serial.println(anTempo[coluna]);
    
  
}

void limpar(){
  linhaAtual = 0;
  coluna = 0;
  for(int i = 0; i < 100; i++){
    indice[i] = 0;
    modo[i] = false;
    linhasLedTempo[i] = 0;
  }
}

void Apagar(){
  for(int i = 0; i < numBotoes; i++){
    digitalWrite(colunasLed[i],LOW);
    digitalWrite(linhasLed[i],LOW);
  }
}

void ApagarNovaMusica(){
  for(int i = 0; i < numBotoes; i++){
    anStatus[i] = false;
    fimStatus[i] = false;
  }
}

void manipulaTxt(){
  boolean tempoTx = false, botaoTx = false, acaoTx = false;
  String str = "", mod = "", ind = "";
  int x = 0;

  limpar();
  
  for(int i = 0; i < txtGravado.length(); i++){
    
    if(botaoTx){
      
      botaoTx = false;
      ind ="";
      ind += txtGravado.charAt(i);
      indice[x] = ind.toInt();
     // Serial.println(indice[x]);
      
    }else if(acaoTx){
      
      acaoTx = false;
      mod ="";
      mod += txtGravado.charAt(i);
      
      if(mod.toInt()){
        modo[x] = true;
      }else{
        modo[x] = false;
      }
      
     // Serial.println(modo[x]);
      x++;
      
    }else if(tempoTx){
      
      str.concat(txtGravado.charAt(i));
      
      if(txtGravado.charAt(i + 1) == '-'){
        tempoTx = false;
        linhasLedTempo[x] = str.toInt();//pega o tempo
      //  Serial.println(linhasLedTempo[x]);
        str = "";
      }
      
    }
    
    if(txtGravado.charAt(i) == '-'){
      botaoTx = true;
    }

    if(txtGravado.charAt(i) == ':'){
      acaoTx = true;
    }

    if(txtGravado.charAt(i) == '='){
      tempoTx = true;
    }
    
  }
  indAtual = x - 1;
  Serial.println(indAtual);
}



void previa(){
  
  for(int i = 0; i < numBotoes; i++){//linha
    
    for(int j = 0; j < numBotoes; j++){//coluna
      
      if((j+i)%2 == 0){
        colunasLedAtivo[i][j] = true;
      }else{
        colunasLedAtivo[i][j] = false;
      }
      
    }
  }

int ad = 0;

 for(int x = 0; x < numBotoes; x++){
  
  if(x%2 == 0){
    linhasLedTempo[x] = 500 + ad;
  }else{
    linhasLedTempo[x] = ad;
  }
  ad = 150 + ad;
 }
  
}

void Proxima(){
  
    boolean alvo = false;
    
    for(int i = 0; i < numBotoes; i++){//linha 
    
    for(int j = 0; j < numBotoes; j++){//coluna
      
        if(j == 0){
          alvo = colunasLedAtivo[i][j];
        }
        
        if(j < (numBotoes - 1)){
          colunasLedAtivo[i][j] = colunasLedAtivo[i][j+1];
        }else{
          colunasLedAtivo[i][j] = alvo;
        }
        
    }
  }
}

void GravarSd(String arquivo, String texto){
  
  if (!SD.begin(53)) {
    while (1);
  }
  
   myFile = SD.open(arquivo, FILE_WRITE);

  if (myFile) {
    myFile.println(texto);
    myFile.close();
  } else {
    //Serial.println("error opening ultimo.txt");
  }
   
}

String ultimoTxt(){

  int ultimo = 0;
  String str = "";
  
  if (!SD.begin(53)) {
    while (1);
  }
  
   myFile = SD.open("ultimo.txt");
  if (myFile) {
  
    while (myFile.available()) {
      char c = myFile.read();
      str.concat(c);
      
      if(c =='\n'){
        ultimo++;
      } 
    }
    
    myFile.close();
  } else {
    
    Serial.println("error opening ultimo.txt");
  }
  
  Serial.println(str);
  String novo = "musica";
  novo += ultimo;
  novo += ".txt";
  GravarSd("ultimo.txt",novo);

  return novo;
}

String leitura(String mTxt){
  //Serial.println(mTxt);
  int ultimo = 0;
  String str = "";
  
  if (!SD.begin(53)) {
    while (1);
  }
  
   myFile = SD.open(mTxt);
  if (myFile) {
  
    while (myFile.available()) {
      char c = myFile.read();
      str.concat(c);
      
      if(c =='\n'){
        ultimo++;
      } 
    }
    
    myFile.close();
  } else {
    
    //Serial.println("error opening ultimo.txt");
  }
  
 // Serial.println(str);
  
  return str;
}

String leituraTxts(int posicao){

  int ultimo = 0;
  String str = "";
  
  if (!SD.begin(53)) {
    while (1);
  }
  
   myFile = SD.open("ultimo.txt");
  if (myFile) {
  
    while (myFile.available()) {
      char c = myFile.read();
      
      if(c =='\n'){
        
        if(posicao == ultimo){
          str.remove(str.length()-1);
          Serial.println(str);
        //  Serial.println(str.length());
          
          return str;
        }
        ultimo++;
        str = "";
      }else{
         str += c;
      }
    }
    
    myFile.close();
  } else {
    
    //Serial.println("error opening ultimo.txt");
  }
  
  musicaGrav = 0;
  return leituraTxts(0);
}
