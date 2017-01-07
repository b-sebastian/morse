#include <EEPROM.h>
#include "EEPROMAnything.h"

String inputString = "";
boolean stringComplete = false;
String alphabet[26];
int pin = 3;
int tSpeed = 100;
int sTone = 1200;
int msgSize = 0;
String lastMsg = "";


void setup() {
  Serial.begin(9600); //szeregowy monitor ON
  inputString.reserve(200);
  
  pinMode(pin, OUTPUT);
  
  EEPROM_readAnything<int>(0, pin);
  EEPROM_readAnything<int>(4, tSpeed);
  EEPROM_readAnything<int>(8, sTone);
  //EEPROM_writeAnything<int>(0, pin); // first INT in eeprom is PIN
  //EEPROM_writeAnything<int>(4, tSpeed); // tSpeed INT
  //EEPROM_writeAnything<int>(8, sTone); // sTone INT  
  // last translate size is INT
  // translate text STRING (each letter takes 1 byte)
  fillAlphabet();
  
  while(!Serial);
  Serial.println(F("AT CTTM=<TEXT> - Translate TEXT to morse code"));
  Serial.println(F("AT CTTA=<CODE> - Translate CODE to alphabet text"));
  Serial.println(F("AT CSSP=<PIN> - Set Speaker Pin"));
  Serial.println(F("AT CSST=<SPEED> - Set Speed Transmission (Higher value means longer sound play for each sign)"));
  Serial.println(F("AT CSSST=<TONE> - Set Speaker Sound Tone"));
  Serial.println(F("AT CRLT=<REPEAT,DELAY> - Repeat last message with delay."));
  
  Serial.println(F("AT <COMMAND>=? - Variables requirements"));
  Serial.println(F("AT <COMMAND>? - Show current set value"));
}

// the loop routine runs over and over again forever:
void loop() {  
  if (stringComplete) {
    inputString.toUpperCase();
    
    while (inputString[0] == ' '){
      inputString = inputString.substring(1,inputString.length());
    }
    
    catchCommand(inputString);
    inputString = "";
    stringComplete = false;
  }
}

void writeStringToEEPROM(int start, String text){
  EEPROM_writeAnything<int>(12, text.length()-1);
  for (int i = start; i <= start + text.length()-1; i++){
    EEPROM.write(i, text[i-start]);
  }  
}

String readStringFromEEPROM(int start, int length){
  String txt = "";
  for (int i = start; i <= start + length; i++){
    txt += (char) EEPROM.read(i);
  }
  
  return txt;
}


void catchCommand(String text)
{
  char afterSplit[200];
  text.toCharArray(afterSplit,sizeof(afterSplit));
  String command = String(strtok(afterSplit, "="));
  
  if (command.substring(0,4) != "AT C" || command.length() == text.length()){
    command = String(strtok(afterSplit, "?"));
    
    if (command.substring(0,4) != "AT C" || command.length() != text.length()){
      String comm = command.substring(4,command.length());
      showCommandVal(comm);
      return;
    }
    
    Serial.println(F("UNKNOWN COMMAND"));
    return;
  }
  
  String comm = command.substring(4,command.length());
  String val = String(strtok(NULL, "\n"));
  
  doSetCommand(comm, val);     
}

void showCommandVal(String command)
{
  if (command == "SSP"){
    Serial.print(F("PIN: "));
    Serial.println(pin);
    return;
  }
  
  if (command == "SST"){
    Serial.print(F("SPEED: "));
    Serial.println(tSpeed);  
    return;
  }
  
  if (command == "SSST"){
    Serial.print(F("TONE: "));
    Serial.println(sTone);
    return;
  }
}

void showRequirements(String* command)
{
  if (*command == "RLT"){
    Serial.println(F("REPEAT and DELAY should contain only INTEGERs"));
    return;
  }
  
  if (*command == "TTA"){
    Serial.println(F("Code should contain only dots \".\", dashes \"-\", spaces and pipes \"|\""));
    return;
  }
  
  if (*command == "TTM"){
    Serial.println(F("TEXT should contain only letters and spaces."));
    return;
  }
    
  if (*command == "SSP"){
    Serial.println(F("PIN should be an INTEGER. Range 2..13"));
    return;
  }
    
  if (*command == "SST"){
    Serial.println(F("SPEED should be an INTEGER. Range 0..MAX_INT"));  
    return;
  }
    
  if (*command == "SSST"){
    Serial.println(F("TONE should be an INTEGER. Range 31..65535"));
    return;
  }
}

void doSetCommand(String command, String val)
{
  if (val == ""){
    Serial.println(F("ARGUMENT MISSING"));
    return;  
  }
  
  if (val == "?"){
    showRequirements(&command);
    return;
  }
  
  if (command == "RLT"){
    int dotIndex = val.indexOf(",");
    if(dotIndex <= 0){
      Serial.println(F("ERROR"));
      return;
    }
    
    int val1 = (val.substring(0,dotIndex)).toInt();
    int val2 = (val.substring(dotIndex+1)).toInt();
    
    EEPROM_readAnything<int>(12, msgSize);
    String text = readStringFromEEPROM(16, msgSize);
    
    for (int i = 0; i < val1; i++){
      if(text[0] == '.' || text[0] == '-'){
        traslateMorseCode2Text(text);
      } else {
        traslateText2MorseCode(text);
      }
      
      if(i < (val1 -1)) {
        delay(val2);
      }
    }
   return; 
  }
  
  if (command == "TTA"){
    if(val[0] != '.' && val[0] != '-'){
      Serial.println(F("ERROR"));
      return;
    }
    writeStringToEEPROM(16, val);
    traslateMorseCode2Text(val);
    return;
  }
  
  if (command == "TTM"){
    if('A' > val[0] || val[0] > 'Z' ){
      Serial.println(F("ERROR"));
      return;
    }
    writeStringToEEPROM(16, val);
    traslateText2MorseCode(val);
    return;
  }
  
  if (command == "SSP"){
    if (val.toInt() < 2 || val.toInt() > 13){
      Serial.println(F("Wrong PIN number!"));
      return;
    }
    pin = val.toInt();
    EEPROM_writeAnything<int>(0, pin);
    pinMode(pin, OUTPUT);
    Serial.print(F("PIN set to "));
    Serial.println(val);
    return;
  }
  
  if (command == "SST"){
    if (val.toInt() < 0 || val.toInt() > 2147483647){
      Serial.println(F("Wrong Speed value!"));
      return;
    }
    tSpeed = val.toInt();
    EEPROM_writeAnything<int>(4, tSpeed);
    Serial.print(F("Speed Transmission set to "));
    Serial.println(val);
    return;
  }
  
  if (command == "SSST"){
    if (val.toInt() < 31 || val.toInt() > 65535){
      Serial.println(F("Wrong Tone frequency!"));
      return;
    }
    sTone = val.toInt();
    EEPROM_writeAnything<int>(8, sTone);
    Serial.print(F("Speaker Sound Tone set to "));
    Serial.println(val);
    return;
  }
  
  Serial.println(F("UNKNOWN COMMAND"));  
}

void traslateMorseCode2Text(String code)
{
  char afterSplit[200];
  code.toCharArray(afterSplit,sizeof(afterSplit));
  String letter = String(strtok(afterSplit, " "));
  
  while (letter != NULL) {
    if (letter[0] == '|'){
      Serial.print(" ");
      tone(pin, sTone, 7*tSpeed);
      delay(7*tSpeed);
      letter = String(strtok(NULL, " "));
      continue;
    }
    Serial.print(translateMorse2Letter(letter));
    delay(3*tSpeed);
    letter = String(strtok(NULL, " "));
  }
  Serial.println("");
}

void traslateText2MorseCode(String text)
{
  for (int i = 0; i < text.length(); i++){
    if(text[i] == ' '){
      Serial.print("| ");
      tone(pin, sTone, 7*tSpeed);
      delay(7*tSpeed);
      continue;
    }
    String letter = String(text[i]);
    Serial.print(translateLetter2Morse(letter) + " ");
    delay(3*tSpeed);
  }
  Serial.println("");
}

String translateMorse2Letter(String code)
{ 
  for (int i = 0; i<sizeof(alphabet); i++){
    if (alphabet[i] == code) {
      playCodeSound(code);
      char letter = (char)(i + 65);
      return String(letter);
    }
  }  
  return "ERROR";
}

String translateLetter2Morse(String letter)
{
  letter.toUpperCase();
  
  int key = (int) letter[0] - 65;

  if(0>key || key>=sizeof(alphabet)){
   return "ERROR"; 
  }
  
  playCodeSound(alphabet[key]);
  
  return alphabet[key];
}

void playCodeSound(String code)
{
  int duration;
  
  for (int i = 0; i < code.length(); i++) {
    char sign = code[i];
    if(sign == '.'){
      duration = tSpeed;
    } else {
      duration = 3*tSpeed;
    }
    tone(pin, sTone, duration);
    delay(duration + tSpeed);
  }
}

void fillAlphabet()
{
  alphabet[0] = ".-";
  alphabet[1] = "-...";
  alphabet[2] = "-.-.";
  alphabet[3] = "-..";
  alphabet[4] = ".";
  alphabet[5] = "..-.";
  alphabet[6] = "--.";
  alphabet[7] = "....";
  alphabet[8] = "..";
  alphabet[9] = ".---";
  alphabet[10] = "-.-";
  alphabet[11] = ".-..";
  alphabet[12] = "--";
  alphabet[13] = "-.";
  alphabet[14] = "---";
  alphabet[15] = ".--.";
  alphabet[16] = "--.-";
  alphabet[17] = ".-.";
  alphabet[18] = "...";
  alphabet[19] = "-";
  alphabet[20] = "..-";
  alphabet[21] = "...-";
  alphabet[22] = ".--";
  alphabet[23] = "-..-";
  alphabet[24] = "-.--";
  alphabet[25] = "--..";
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      // add it to the inputString:
      inputString += inChar;
    }    
  }
}

