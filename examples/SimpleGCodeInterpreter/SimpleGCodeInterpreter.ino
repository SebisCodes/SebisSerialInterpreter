#include <math.h>
#include <SebisSerialInterpreter.h>

SebisSerialInterpreter ssi(&Serial);

//Position of the machine; G0 adds the values of a line
double position[] = {0,0,0};

//Values collected from the current line
struct TempVars {
  double x;
  double y;
  double z;
  int g;
  int m;
  double f;
};
TempVars tvar = {0.0,0.0,0.0,-1,-1,0.0};

void setX(String code, double value) {tvar.x = value;}
void setY(String code, double value) {tvar.y = value;}
void setZ(String code, double value) {tvar.z = value;}
void setG(String code, double value) {tvar.g = round(value);}
void setM(String code, double value) {tvar.m = round(value);}
void setF(String code, double value) {tvar.f = value;}

//Runs the collected command after the line ends
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
    }  
  } else if(tvar.m >= 0){
    switch(tvar.m) {
      case 0:
        break;
      case 1:
        break;
      case 2:
        break;
    }  
  }
  resetValues();  
}

//Resets the values for the next line; -1 means no G or M code
void resetValues() {
  setX("",0);
  setY("",0);
  setZ("",0);
  setG("",-1);
  setM("",-1);
  setF("",0);
}

//Must stay alive while registered: global or static
FunctionList fSetX;
FunctionList fSetY;
FunctionList fSetZ;
FunctionList fSetG;
FunctionList fSetM;
FunctionList fSetF;


void setup() {
  Serial.begin(115200);
  fSetX =  ssi.createFunctionList("X", &setX);
  fSetY =  ssi.createFunctionList("Y", &setY);
  fSetZ =  ssi.createFunctionList("Z", &setZ);
  fSetG =  ssi.createFunctionList("G", &setG);
  fSetM =  ssi.createFunctionList("M", &setM);
  fSetF =  ssi.createFunctionList("F", &setF);
  ssi.addFunctionList(&fSetX);
  ssi.addFunctionList(&fSetY);
  ssi.addFunctionList(&fSetZ);
  ssi.addFunctionList(&fSetG);
  ssi.addFunctionList(&fSetM);
  ssi.addFunctionList(&fSetF);
  ssi.setFinalFunction(&runFinal);
  
  Serial.println("Setup is finished!");
  Serial.println("Please make sure you use a new line after sending your codes!");

  ssi.serialRead(true, true); //Never returns
}

void loop() {
  //Not used: serialRead(true, true) in setup() never returns
}
