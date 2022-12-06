
#include "SebisSerialInterpreter.h"

/**
 * @brief Construct a new Sebis Serial Interpreter:: Sebis Serial Interpreter object
 *
 * @param hardwareSerial HardwareSerial - The serial input you want to use
 */
SebisSerialInterpreter::SebisSerialInterpreter(HardwareSerial *hardwareSerial)
{
    this->hSerial = hardwareSerial;
    this->lfp = &(this->fEmpty);
}
/**
 * @brief A funcion to add functions to this class which should be called if a code was found
 *
 * @param code String - If serial finds this code, your function will be called with the value
 * @param function void - A function you want to call if the defined code was found in your serial connection
 */
FunctionList SebisSerialInterpreter::createFunctionList(String code, ListFunction func)
{
    FunctionList fNew = {func, code, NULL};
    return fNew;
}
/**
 * @brief A funcion to add functions to this class which should be called if a code was found
 *
 * @param code String - If serial finds this code, your function will be called with the value
 * @param function void - A function you want to call if the defined code was found in your serial connection
 */
void SebisSerialInterpreter::addFunctionList(FunctionList *fNew)
{
    removeFunctionList(fNew->code);
    FunctionList *fl = this->lfp;
    while (fl->next != NULL)
    {
        fl = fl->next;
    }
    fl->next = fNew;
}
/**
 * @brief A funcion to remove previously added functions out of this class 
 *
 * @param code String - Remove all fnctions definded with this code
 */
void SebisSerialInterpreter::removeFunctionList(String code)
{
    FunctionList *fl = this->lfp;
    while (fl->next != NULL)
    {
        FunctionList* fNew = fl->next;
        if (!fNew->code.compareTo(code)) { //code is equal to this function
            fl->next = fNew->next;
            free(fNew);
        }
        fl = fl->next;
    }
}
/**
 * @brief Set a function which is called if a new line was detected
 *
 * @param FinalFunction (void *)() - A function which should be called if a new line was detected
 */
void SebisSerialInterpreter::setFinalFunction(FinalFunction func) {
  this->finalFunction = func;
  }
/**
 * @brief A funcion to run functions which were stored added by a code
 *
 * @param code String - If serial finds this code, your function will be called with the value
 * @param double double - The value which the interpreter interpreted
 * @param function void - A function you want to call if the defined code was found in your serial connection
 */
void SebisSerialInterpreter::runFunction(String code, double value)
{
    FunctionList *fl = this->lfp;
    while (fl->next != NULL)
    {
        FunctionList* fNew = fl->next;
        if (!fNew->code.compareTo(code)) { //code is equal to this function
            fNew->func(code, value);
            break;
        }
        fl = fl->next;
    }
}
/**
 * @brief Read serial values and interpret them
 * 
 * @param endlessLoop boolean - Run your program endless
 * @param blockingBehavior boolean - Enable or disable blocking behavior while reading from serial
 */
void SebisSerialInterpreter::serialRead(boolean endlessLoop, boolean blockingBehavior)
{
    do {
        while (this->hSerial->available() <= 0 && endlessLoop);
        
        if (this->hSerial->available() > 0) {
            do {
                char c = (char)this->hSerial->read();
                this->codeToInterpret += c;
                if (c == 13 || c == 10) {
                  this->interpret();
                  this->finalFunction();
                }
            } while (this->hSerial->available() > 0 && blockingBehavior);
        }

    } while (endlessLoop);
}
/**
 * @brief An interpreter which will get the codes out of the readed String (Format: "[a-zA-Z]*[0-9]*([\.]?)([0-9]*?)")
 * 
 */
void SebisSerialInterpreter::interpret() {
    if (this->codeToInterpret.length() > 0) {
      String code = "";
      String codeVal = "";
      bool codeFound = false;
      bool codeValueFound = false;
      bool run = true;
      char c = this->codeToInterpret.charAt(0);
      do {
          code = "";
          codeVal = "";
          codeFound = false;
          codeValueFound = false;
          while((!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) && run) {
              this->codeToInterpret = this->codeToInterpret.substring(1);
              if (this->codeToInterpret.length() > 0)
                c = this->codeToInterpret.charAt(0);
              else
                run = false;
          }
          while(((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))&& run) {
              codeFound = true;
              code += c;
              this->codeToInterpret = this->codeToInterpret.substring(1);
              if (this->codeToInterpret.length() > 0)
                c = this->codeToInterpret.charAt(0);
              else
                run = false;
          }
          if (c == '-') {
            codeVal += c;
            this->codeToInterpret = this->codeToInterpret.substring(1);
            if (this->codeToInterpret.length() > 0)
              c = this->codeToInterpret.charAt(0);
            else
              run = false;
          }
          while(((c >= '0' && c <= '9') || c == '.' || c == ',')  && run) {
              if (c == ',') c = '.';
              codeValueFound = true;
              codeVal += c;
              this->codeToInterpret = this->codeToInterpret.substring(1);
              if (this->codeToInterpret.length() > 0)
                c = this->codeToInterpret.charAt(0);
              else
                run = false;
          }
          if (codeFound && codeValueFound) {
              this->runFunction(code, codeVal.toDouble());
          }
      } while(run);
    }
}
