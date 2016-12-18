
String inputString = "";
boolean stringComplete = false;

String alphabet[26];

void setup() {
  Serial.begin(9600); //szeregowy monitor ON
  inputString.reserve(200);
  
  fillAlphabet();
  //Serial.println("First char will determine translation type.\nLetter will transalte to morse. '-' or '.' will transalte to alphabet.");
}

// the loop routine runs over and over again forever:
void loop() {  
  if (stringComplete) {
    Serial.println(transalteMorse2Letter(inputString));

    //Serial.println(inputString);
    
    inputString = "";
    stringComplete = false;
  }
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

String transalteMorse2Letter(String code)
{ 
  for (int i = 0; i<sizeof(alphabet); i++){
    if (alphabet[i] == code) {
      char letter = (char)(i + 65);
      return String(letter);
    }
  }  
  return "";
}

String transalteLetter2Morse(String letter)
{
  letter.toUpperCase();
  
  int key = (int) letter[0] - 65;
  
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
