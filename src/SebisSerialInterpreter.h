/*
  SebisSerialInterpreter - A library to run functions by your g-code similar codes
  Created by Sebastian G. Winterthur, May 01, 2022.
*/
#ifndef SEBISSERIALINTERPRETER_H
#define SEBISSERIALINTERPRETER_H

#include <stdio.h>
#include <stdlib.h>
#include <Arduino.h>


typedef void (*ListFunction)(String, double);
typedef void (*FinalFunction)();
struct FunctionList { //A list of functions
  ListFunction func;
  String code;
  struct FunctionList *next;
};
typedef FunctionList* FunctionListPtr; //Pointer definition to the functionlist

class SebisSerialInterpreter {
  public:
    SebisSerialInterpreter(HardwareSerial *hardwareSerial);
    FunctionList createFunctionList(String code, ListFunction func);
    void addFunctionList(FunctionList *fNew);
    void removeFunctionList(String code);
    void runFunction(String code, double value);
    void setFinalFunction(FinalFunction func);
    void serialRead(boolean endlessLoop);

  private:
    void interpret();
    HardwareSerial *hSerial; //Your Serial object
    FunctionListPtr lfp; //The first pointer of the list which is unused
    FunctionList fEmpty = {0}; //Just an empty FunctionList
    String codeToInterpret = ""; //The code which will be interpreted
    FinalFunction finalFunction = NULL;
};

#endif
