
String inputString = "";
boolean stringComplete = false;

String alphabet[26];

void setup() {
  Serial.begin(9600); //szeregowy monitor ON
  inputString.reserve(200);
  pinMode(3, OUTPUT);
  
  fillAlphabet();
  
  while(!Serial);
  Serial.println("First char will determine translation type. Letter will translate to morse. '-' or '.' will transalte to alphabet.");
}

// the loop routine runs over and over again forever:
void loop() {  
  if (stringComplete) {
    inputString.toUpperCase();
    
    while (inputString[0] == ' '){
      inputString = inputString.substring(1,inputString.length());
    }
    
    if(inputString[0] == '.' || inputString[0] == '-'){
      traslateMorseCode2Text(inputString);
    } else if('A' <= inputString[0] && inputString[0] <= 'Z' ){
      traslateText2MorseCode(inputString);
    } else {
      Serial.println("ERROR");
    }
    
    inputString = "";
    stringComplete = false;
  }
}

void traslateMorseCode2Text(String code)
{
  char afterSplit[200];
  code.toCharArray(afterSplit,sizeof(afterSplit));
  String letter = String(strtok(afterSplit, " "));
  
  while (letter != NULL) {
    if (letter[0] == '|'){
      Serial.print(" ");
      //tone(3, 1200, 700);
      //delay(700);
      letter = String(strtok(NULL, " "));
      continue;
    }
    Serial.print(translateMorse2Letter(letter));
    //delay(300);
    letter = String(strtok(NULL, " "));
  }
  Serial.println("");
}

void traslateText2MorseCode(String text)
{
  for (int i = 0; i < text.length(); i++){
    if(text[i] == ' '){
      Serial.print("| ");
      tone(3, 1200, 700);
      delay(700);
      continue;
    }
    String letter = String(text[i]);
    Serial.print(translateLetter2Morse(letter) + " ");
    delay(300);
  }
  Serial.println("");
}

String translateMorse2Letter(String code)
{ 
  for (int i = 0; i<sizeof(alphabet); i++){
    if (alphabet[i] == code) {
      char letter = (char)(i + 65);
      return String(letter);
    }
  }  
  return "ERROR";
}

String translateLetter2Morse(String letter)
{
  int duration;
  letter.toUpperCase();
  
  int key = (int) letter[0] - 65;

  if(0>key || key>=sizeof(alphabet)){
   return "ERROR"; 
  }
  
  for (int i = 0; i < String(alphabet[key]).length(); i++) {
    char sign = alphabet[key][i];
    if(sign == '.'){
      duration = 100;
    } else {
      duration = 300;
    }
    tone(3, 1200, duration);
    delay(duration + 100);
  }
  
  return alphabet[key];
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
