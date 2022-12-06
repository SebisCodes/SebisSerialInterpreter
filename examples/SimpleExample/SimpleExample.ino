#include <SebisSerialInterpreter.h>

//Define and call the constructor
SebisSerialInterpreter ssi(&Serial);

//Define any function you need
void myFunction(String code, double value) {
  Serial.print("Run code \"");
  Serial.print(code);
  Serial.print("\" with value ");
  Serial.println(String(value));
}
//Define any function you need
void myFinalFunction() {
  Serial.print("A new line has been detected!");
}
//Define a global variable to store your function and use it globally
FunctionList myFunctionStorage;

void setup() {
  Serial.begin(115200); //You can use any bauds, it doesn't have to be 115200

  myFunctionStorage = ssi.createFunctionList("MGC", &myFunction); //Initialize your function storages (f.e. at code MGC(MyGCode))
  ssi.addFunctionList(&myFunctionStorage); //Add your functionstorage to ssi class
  ssi.setFinalFunction(&myFinalFunction); //Set a function which should be called at new line

  Serial.println("Setup is finished!");
  Serial.println("Please make sure you use a new line after sending your codes!");
  Serial.println("Type for example \"MGC100 MGC22.5\" in the serial monitor...");
  
  ssi.serialRead(true, true); //Get and interpret g-codes - comment this line to run functions in loop
}

void loop() {
  //ssi.serialRead(false, false); //uncomment this line to run loop while reading codes
}
