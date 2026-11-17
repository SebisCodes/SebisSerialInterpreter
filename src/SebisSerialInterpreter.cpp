#include "SebisSerialInterpreter.h"

SebisSerialInterpreter::SebisSerialInterpreter(Stream *stream)
{
    this->hSerial = stream;
    this->lfp = &(this->fEmpty);
}
FunctionList SebisSerialInterpreter::createFunctionList(String code, ListFunction func)
{
    FunctionList fNew = {func, code, NULL};
    return fNew;
}
void SebisSerialInterpreter::addFunctionList(FunctionList *fNew)
{
    if (fNew == NULL) return;
    removeFunctionList(fNew->code);
    fNew->next = NULL; //Never take over links from another list
    FunctionList *fl = this->lfp;
    while (fl->next != NULL)
    {
        fl = fl->next;
    }
    fl->next = fNew;
}
void SebisSerialInterpreter::removeFunctionList(String code)
{
    FunctionList *fl = this->lfp;
    while (fl->next != NULL)
    {
        FunctionList* fNew = fl->next;
        if (!fNew->code.compareTo(code)) {
            //The caller owns the entry: unlink it, do not free it
            fl->next = fNew->next;
            fNew->next = NULL;
        } else {
            fl = fNew; //Only advance if nothing was removed, so no entry is skipped
        }
    }
}
void SebisSerialInterpreter::setFinalFunction(FinalFunction func) {
  this->finalFunction = func;
}
void SebisSerialInterpreter::runFunction(String code, double value)
{
    FunctionList *fl = this->lfp;
    while (fl->next != NULL)
    {
        FunctionList* fNew = fl->next;
        if (!fNew->code.compareTo(code)) {
            if (fNew->func != NULL) fNew->func(code, value);
            break;
        }
        fl = fl->next;
    }
}
void SebisSerialInterpreter::serialRead(boolean endlessLoop, boolean blockingBehavior)
{
    do {
        while (this->hSerial->available() <= 0 && endlessLoop);
        
        if (this->hSerial->available() > 0) {
            do {
                char c = (char)this->hSerial->read();
                if (c == '\r' || c == '\n') {
                  bool secondHalfOfCRLF = (c == '\n' && this->lastWasCR);
                  this->lastWasCR = (c == '\r');
                  if (!secondHalfOfCRLF) { //"\r\n" ends only one line
                    if (!this->lineTooLong) this->interpret();
                    this->codeToInterpret = "";
                    this->lineTooLong = false;
                    if (this->finalFunction != NULL) this->finalFunction();
                  }
                } else {
                  this->lastWasCR = false;
                  //Drop the whole line at its end if it is too long or does not fit into RAM
                  if (this->lineTooLong || this->codeToInterpret.length() >= SSI_MAX_LINE_LENGTH
                      || !this->codeToInterpret.concat(c))
                    this->lineTooLong = true;
                }
            } while (this->hSerial->available() > 0 && blockingBehavior);
        }

    } while (endlessLoop);
}
//Consumes codeToInterpret; the format is described in the header
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
              this->runFunction(code, strtod(codeVal.c_str(), NULL)); //not toDouble(): missing on some cores
          }
      } while(run);
    }
}
