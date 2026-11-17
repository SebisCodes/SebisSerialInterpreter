#include <SebisSerialInterpreter.h>

SebisSerialInterpreter ssi(&Serial);

void myFunction(String code, double value) {
  Serial.print("Run code \"");
  Serial.print(code);
  Serial.print("\" with value ");
  Serial.println(value); //no temporary String
}
//Runs after the codes of each line
void myFinalFunction() {
  Serial.println("A new line has been detected!");
}
//Must stay alive while registered: global or static
FunctionList myFunctionStorage;

void setup() {
  Serial.begin(115200);

  myFunctionStorage = ssi.createFunctionList("MGC", &myFunction);
  ssi.addFunctionList(&myFunctionStorage);
  ssi.setFinalFunction(&myFinalFunction);

  Serial.println("Setup is finished!");
  Serial.println("Please make sure you use a new line after sending your codes!");
  Serial.println("Type for example \"MGC100 MGC22.5\" in the serial monitor...");
  
  ssi.serialRead(true, true); //Never returns; to use loop(), remove it and use the call below
}

void loop() {
  //ssi.serialRead(false, false); //Reads one character per call
}
