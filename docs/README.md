# SebisSerialInterpreter documentation

Version 2.0.1 · [Install](#install) · [Input](#input) · [API](#api) · [Reading](#reading) · [Examples](#examples) · [Tests](#tests)

## Install

1. [Download the ZIP](https://github.com/SebisCodes/SebisSerialInterpreter/archive/refs/heads/main.zip).
2. Arduino IDE: **Sketch › Include Library › Add .ZIP Library**.
3. The examples are under **File › Examples › SebisSerialInterpreter**.

## Quick start

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

Send `X10` with a line ending and the sketch prints `10.00`.

## Input

| Part | Rule |
|---|---|
| **Line end** | LF, CR or CR LF. CR LF counts once. |
| **Code** | Letters followed directly by a number: `X10`, `MGC100`. Case-sensitive. Any other character separates codes. |
| **Number** | `-2.5`, `1,5` (comma = point). Malformed numbers are not rejected, the valid start counts: `X1.2.3` gives 1.2, `X.` gives 0, `X1-2` gives 1. |
| **Ignored** | A code without a number: `X`, `X 5`, `X-`. |
| **Comments** | Not skipped: in `X1 ; Y9` both codes are called. |
| **Length** | At most 128 characters without the line ending. A longer line is dropped. |
| **Order** | The functions run in the order of the codes in the line, then the final function. |

Register `"G"`, not `"G0"`: `G0` is read as code `G` with value 0.

## API

| Function | What it does |
|---|---|
| `SebisSerialInterpreter(Stream *stream)` | Takes a Stream. Tested with `&Serial`. |
| `FunctionList createFunctionList(String code, ListFunction func)` | Creates an entry. Keep it alive while registered (global or static). |
| `void addFunctionList(FunctionList *entry)` | Registers the entry and replaces one with the same code. `NULL` does nothing. One interpreter per entry. |
| `void removeFunctionList(String code)` | Unregisters a code. The entry is not freed and can be added again. |
| `void setFinalFunction(FinalFunction func)` | Function called once per line, also for empty and dropped lines. `NULL` for none. |
| `void runFunction(String code, double value)` | Calls the function of a registered code directly. |
| `void serialRead(boolean endlessLoop, boolean blockingBehavior)` | Reads input and handles every complete line, see [Reading](#reading). |

Function types:

```cpp
void listFunction(String code, double value);  // ListFunction
void finalFunction();                          // FinalFunction
```

The interpreter cannot be copied.

## Reading

| Call | Behavior |
|---|---|
| `serialRead(true, true)` in `setup()` | Never returns. |
| `serialRead(false, true)` in `loop()` | Reads all available input, then returns. |
| `serialRead(false, false)` in `loop()` | Reads one character per call. |

The 128-character limit is `SSI_MAX_LINE_LENGTH` in `src/SebisSerialInterpreter.h`.

## Examples

| Example | What it does |
|---|---|
| [SimpleExample](../examples/SimpleExample/SimpleExample.ino) | Registers the code `MGC` and prints every value it receives. |
| [SimpleGCodeInterpreter](../examples/SimpleGCodeInterpreter/SimpleGCodeInterpreter.ino) | Adds `X`, `Y` and `Z` of a `G0` line to a position and prints it. |

## Tests

Version 2.0.1 was tested on an Arduino Uno: 65 hardware tests in both `loop()` reading modes, and both examples. Both examples compile for Uno, Nano, Mega and Leonardo. Installing from the ZIP was tested.

## Changes in 2.0.1

- No crash when removing or re-adding a code, or without a final function.
- CR LF ends one line; too long lines are dropped.
- Takes a Stream instead of a HardwareSerial, so it compiles for the Leonardo.

---

[ssi.giit.ch](https://ssi.giit.ch) · [giit.ch](https://giit.ch) · [MIT License](../LICENSE)
