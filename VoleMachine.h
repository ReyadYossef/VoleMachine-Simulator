#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <cctype>
#include <fstream>
#include<bitset>
#include<sstream>
#include<cmath>
#include<cstdlib>
#include<cstdint>
#include <iomanip>
using namespace std;


class Memory {
protected:
    static vector<string> memory;
public:
    string getcell(int address)const;
    void setcell(int address, const string& val);

};

class Register {
    static vector<string>Reg;
public:
    string getreg(int idx);
    void setreg(int idx, const string& val);

};



class ALU {
public:
    string hexToBinary(const string& hex);
    string binaryToHex(const string& bin);
    string twosComplementAdd(const string& bin1, const string& bin2);
    uint8_t hexToBinaryFP(const string& hex);
    string binaryToHexFP(uint8_t binary);
    double convertSEEEMMMMToDecimal(uint8_t seeemmmm);
    uint8_t encodeFloatingPoint(double decimal);
    string floatingPointAdd(const string& hex1, const string& hex2);
    void xorOperation(Register& reg, int reg1, int reg2, int regDest);
    void rotateRight(Register& reg, int regR, int X);
    void AndOperation(Register& reg, int reg1, int reg2, int regDest);
    void OROperation(Register& reg, int reg1, int reg2, int regDest);
    string  dec_to_hex(int num);
    int hexToSignedInt(const string& hex);

};

class CU {
public:
    void load(int idxReg, int memAddr, Register& reg, Memory& mem);
    void store(int idxReg, int memAddr, Register& reg, Memory& mem);
    void move(int idxRegSrc, int idxRegDest, Register& reg);
    void jump(int memAddr, int* programCounter);
    void copyToRegister(int regIdx, int memAddr, Register& reg, Memory& mem);
};

class CPU : public ALU {
    int* programCounter;
    string instructionRegister;
    ALU alu;
    CU cu;
    bool isHalted;
public:
    CPU(int* pc) : programCounter(pc), isHalted(false) {}
    virtual ~CPU() {}
    void fetchInstruction(const Memory& memory);
    void decodeExecute(Register& registers, Memory& memory);
    void halt();
    bool getIsHalted() const {
        return isHalted;
    }
};

class Machine {
private:
    CPU cpu;
    Memory memory;
    Register registers;
    int programCounter;
public:
    Machine() : cpu(&programCounter), programCounter(0) {}
    void loadProgramFile(const string& filename);
    void run();
    void outputState(Register& registers, Memory& memory);
    bool isValidInstruction(const string& instruction);
    Register& getRegisters() { return registers; }
    Memory& getMemory() { return memory; }
};