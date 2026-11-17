/*
  SebisSerialInterpreter - A library to run functions by your g-code similar codes
  Created by Sebastian G. Winterthur, May 01, 2022.
*/
/**
 * @file SebisSerialInterpreter.h
 * @brief Calls your functions for letter/number codes read from a Stream.
 */
#ifndef SEBISSERIALINTERPRETER_H
#define SEBISSERIALINTERPRETER_H

#include <stdio.h>
#include <stdlib.h>
#include <Arduino.h>

/**
 * @brief Maximum line length without the line ending.
 *
 * A longer line, or one that does not fit into the free RAM, is dropped completely.
 * Change the value here: a @c \#define in the sketch has no effect, the library is compiled separately.
 */
#ifndef SSI_MAX_LINE_LENGTH
#define SSI_MAX_LINE_LENGTH 128
#endif

/// Function called with the code found (e.g. "X") and its value.
typedef void (*ListFunction)(String, double);

/// Function called once per line, after the functions for its codes.
typedef void (*FinalFunction)();

/**
 * @brief A code and its function.
 *
 * The interpreter keeps a pointer to it, so it must stay alive while registered (global or static).
 */
struct FunctionList {
  ListFunction func;          ///< Function to run when the code is found
  String code;                ///< The code, letters only and case-sensitive (e.g. "X" or "MGC")
  struct FunctionList *next;  ///< Next entry in the list, managed by the interpreter
};

typedef FunctionList* FunctionListPtr;

/**
 * @brief Reads lines from a Stream and calls the function of every registered code in them, in input order.
 *
 * A line ends with LF, CR or CR LF (counts once). A code is case-sensitive letters followed directly by
 * @c -?[0-9.,]+, e.g. @c X10, @c Y-2.5, <tt>Z1,5</tt>; anything else separates codes.
 * A comma counts as decimal point and the longest valid number at the start is used:
 * @c X1.2.3 gives 1.2, @c X. gives 0. A code without a number is ignored.
 * No leading @c +, no exponents (@c X1e3 is @c X1 and @c e3), G-code comments are not skipped.
 */
class SebisSerialInterpreter {
  public:
    /**
     * @brief Creates an interpreter.
     * @param stream A Stream, e.g. @c &Serial.
     */
    SebisSerialInterpreter(Stream *stream);

    /// Not copyable: a copy would share the list of the original.
    SebisSerialInterpreter(const SebisSerialInterpreter &) = delete;
    SebisSerialInterpreter &operator=(const SebisSerialInterpreter &) = delete;

    /**
     * @brief Creates an entry; register it with addFunctionList().
     * @param code Letters only, case-sensitive. Register "G", not "G0": "G0" is read as "G" with value 0.
     * @param func Function for this code.
     * @return The entry.
     */
    FunctionList createFunctionList(String code, ListFunction func);

    /**
     * @brief Registers an entry and replaces one with the same code.
     * @param fNew The entry (NULL does nothing). Register it with one interpreter only.
     */
    void addFunctionList(FunctionList *fNew);

    /**
     * @brief Unregisters a code, if registered. The entry is not freed, only its @c next is cleared.
     * @param code Code to unregister.
     */
    void removeFunctionList(String code);

    /**
     * @brief Calls the function of a code, if registered.
     * @param code Code, e.g. "X".
     * @param value Value passed to the function.
     */
    void runFunction(String code, double value);

    /**
     * @brief Sets the function called once per line, also for empty and dropped lines.
     * @param func Function, or NULL for none (default).
     */
    void setFinalFunction(FinalFunction func);

    /**
     * @brief Reads input and handles every complete line.
     * @param endlessLoop true: never returns.
     * @param blockingBehavior If endlessLoop is false: true reads all available input,
     *                         false at most one character.
     */
    void serialRead(boolean endlessLoop, boolean blockingBehavior);

  private:
    void interpret();
    Stream *hSerial;
    FunctionListPtr lfp; //Points to fEmpty
    FunctionList fEmpty = {0}; //Unused first entry of the list
    String codeToInterpret = ""; //Current line without the line ending
    FinalFunction finalFunction = NULL;
    bool lastWasCR = false; //Used to treat "\r\n" as one line ending
    bool lineTooLong = false; //Drop the current line: too long or out of RAM
};

#endif
