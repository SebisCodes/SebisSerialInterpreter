#include <math.h>
#include <SebisSerialInterpreter.h>

//Define and call the constructor
SebisSerialInterpreter ssi(&Serial);

//The local position of your machine
double position[] = {0,0,0};

//Create a struct to store your temporary values properly
struct TemmpVars {
  double x;
  double y;
  double z;
  int g;
  int m;
  double f;
};
//Create a struct to store your functions properly
struct TemmpFunctions {
  FunctionList x;
  FunctionList y;
  FunctionList z;
  FunctionList g;
  FunctionList m;
  FunctionList f;
};
//Create a new struct var
TemmpVars tvar = {0.0,0.0,0.0,-1,-1,0.0};
TemmpFunctions tfunc = {0};

//Functions to store the values
void setX(String code, double value) {tvar.x = value;}
void setY(String code, double value) {tvar.y = value;}
void setZ(String code, double value) {tvar.z = value;}
void setG(String code, double value) {tvar.g = round(value);}
void setM(String code, double value) {tvar.m = round(value);}
void setF(String code, double value) {tvar.f = value;}

//A function which should be called if the line is finished
void runFinal() {
  if (tvar.g >= 0) {
    switch(tvar.g) {
      case 0:
        position[0] += tvar.x;
        position[1] += tvar.y;
        position[2] += tvar.z;
        Serial.print("New position: X:");
        Serial.print(position[0]);
        Serial.print(" Y:");
        Serial.print(position[1]);
        Serial.print(" Z:");
        Serial.println(position[2]);
        break;
      case 1:
        break;
      case 2:
        break;
      //case ...:
    }  
  } else if(tvar.m >= 0){
    switch(tvar.m) {
      case 0:
        break;
      case 1:
        break;
      case 2:
        break;
      //case ...:
    }  
  }
  resetValues();  
}

//Reset all values to run a new g-code line
void resetValues() {
  setX("",0);
  setY("",0);
  setZ("",0);
  setG("",-1);
  setM("",-1);
  setF("",0);
}

//Define FunctionList vars - MUST be global!
FunctionList fSetX;
FunctionList fSetY;
FunctionList fSetZ;
FunctionList fSetG;
FunctionList fSetM;
FunctionList fSetF;


void setup() {
  Serial.begin(115200);
  //initialize all FunctionList vars
  fSetX =  ssi.createFunctionList("X", &setX);
  fSetY =  ssi.createFunctionList("Y", &setY);
  fSetZ =  ssi.createFunctionList("Z", &setZ);
  fSetG =  ssi.createFunctionList("G", &setG);
  fSetM =  ssi.createFunctionList("M", &setM);
  fSetF =  ssi.createFunctionList("F", &setF);
  //add your functions to ssi class
  ssi.addFunctionList(&fSetX);
  ssi.addFunctionList(&fSetY);
  ssi.addFunctionList(&fSetZ);
  ssi.addFunctionList(&fSetG);
  ssi.addFunctionList(&fSetM);
  ssi.addFunctionList(&fSetF);
  //define your function which should be called if a new line was detected
  ssi.setFinalFunction(&runFinal);
  
  Serial.println("Setup is finished!");
  Serial.println("Please make sure you use a new line after sending your codes!");

  //Get and interpret g-codes
  ssi.serialRead(true);
}

void loop() {
  //Not needed if ssi.serialRead() was called with true in setup
}
