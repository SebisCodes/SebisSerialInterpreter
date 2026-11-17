# SebisSerialInterpreter

Calls your functions for letter/number codes read from `Serial` or any other `Stream`:
`G1 X10.5` calls the function for `G` with 1 and the one for `X` with 10.5.

```cpp
#include <SebisSerialInterpreter.h>

SebisSerialInterpreter ssi(&Serial);
FunctionList fX;  // must stay alive while registered: global or static

void onX(String code, double value) { Serial.println(value); }

void setup() {
  Serial.begin(115200);
  fX = ssi.createFunctionList("X", &onX);
  ssi.addFunctionList(&fX);
}

void loop() { ssi.serialRead(false, true); }
```

- Lines end with LF, CR or CR LF. `setFinalFunction()` sets a function called once per line.
- Codes are case-sensitive letters followed directly by a number: `Y-2.5`, `Z1,5`. Register `"G"`, not `"G0"`.
- Numbers are not checked: `X1.2.3` gives 1.2, `X.` gives 0, a code without a number is ignored.
- Lines over 128 characters are dropped. G-code comments are not skipped.

[Examples](https://github.com/SebisCodes/SebisSerialInterpreter/blob/main/examples) · API reference: `docs/html/index.html` (rebuild with `doxygen`) · [MIT License](https://github.com/SebisCodes/SebisSerialInterpreter/blob/main/LICENSE)
