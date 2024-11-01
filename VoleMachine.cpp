#include "VoleMachine.h"
vector<string> Memory::memory(256, "00");
vector<string> Register::Reg(16, "00");
vector<vector<string>>v1(16, vector<string>(16, "00"));
string Memory::getcell(int address) const {
    if (address >= 0 && address < 256) {
        return memory[address];
    }
    else {
        cout << "Address out of bounds: " << address << endl;
        return "";
    }
}

void Memory::setcell(int address, const string& val) {
    if (address >= 0 && address < 255 && val.size() >= 4) {
        memory[address] = val.substr(0, 2);
        memory[address + 1] = val.substr(2, 4);
    }
    else {
        cout << "Invalid address or instruction: " << val << endl;
    }
}

string Register::getreg(int idx) {
    if (idx >= 0 && idx < 16) {
        return Reg[idx];
    }
    else {
        cerr << "Error: Attempted to access register " << idx << " which is out of bounds. Valid range is 0 to " << (
            Reg.size() - 1) << "." << endl;
        return "";
    }
}

void Register::setreg(int idx, const string& val) {
    if (idx >= 0 && idx < 16) {
        cout << "idx = " << idx << endl;
        Reg[idx] = val;
        cout << "has been regidted" << endl;
    }
    else {
        cerr << "Error: Attempted to set register " << idx << " which is out of bounds. Valid range is 0 to " << (
            Reg.size() - 1) << "." << endl;
    }
}

void CU::load(int idxReg, int memAddr, Register& reg, Memory& mem) {
    string value = mem.getcell(memAddr);
    reg.setreg(idxReg, value);
    cout << "Loaded value " << value << " from memory[" << memAddr << "] to register R[" << idxReg << "]." << endl;
}

void CU::store(int idxReg, int memAddr, Register& reg, Memory& mem) {
    string value = reg.getreg(idxReg);
    mem.setcell(memAddr, value);
    cout << "Stored value " << value << " from register R[" << idxReg << "] to memory[" << memAddr << "]." << endl;
}

void CU::move(int idxRegSrc, int idxRegDest, Register& reg) {
    string value = reg.getreg(idxRegSrc);
    reg.setreg(idxRegDest, value);
    cout << "Moved value " << value << " from R[" << idxRegSrc << "] to R[" << idxRegDest << "]." << endl;
}

void CU::jump(int memAddr, int* programCounter) {
    if (memAddr >= 0 && memAddr < 256 && programCounter) { // تأكد أن المؤشر ليس null
        *programCounter = memAddr;
        cout << "Jumped to address " << *programCounter << "." << endl;
    }
    else {
        cout << "Invalid jump address: " << memAddr << endl;
    }
}


void CU::copyToRegister(int regIdx, int memAddr, Register& reg, Memory& mem) {
    string value = mem.getcell(memAddr);
    reg.setreg(regIdx, value);
    cout << "Copied value " << value << " from memory[" << memAddr << "] to register R[" << regIdx << "]." << endl;
}

int stringToInt(const std::string& str) {
    int result;
    std::istringstream(str) >> std::hex >> result;  // Convert from hexadecimal string to int
    return result;
}


int ALU::hexToSignedInt(const string& hex) {
    int value = stoi(hex, nullptr, 16);
    if (value >= 0x80) { // If the most significant bit is set (indicating a negative number)
        value -= 0x100; // Convert to two's-complement negative value
    }
    return value;
}


void CPU::fetchInstruction(const Memory& memory) {
    // Check if the programCounter is out of bounds at the start
    if (*programCounter < 0 || *programCounter >= 256) {
        cout << "Program counter out of bounds: " << *programCounter << endl;
        return;
    }

    instructionRegister.clear(); // Clear previous instruction

    // Fetch current instruction from memory
    instructionRegister = memory.getcell(*programCounter);

    // Move programCounter to the next cell
    *programCounter += 1;

    // Fetch next instruction if within bounds
    if (*programCounter < 256) {
        instructionRegister += memory.getcell(*programCounter);
        *programCounter += 1;
    }
    else {
        cout << "Reached end of memory; next instruction not fetched." << endl;
    }

    // Cap programCounter to 255 if it goes out of bounds
    if (*programCounter >= 256) {
        cout << "Program counter out of bounds after fetch." << endl;
        *programCounter = 255; // Restrict programCounter to max address
    }
}

void Machine::loadProgramFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Machine File Not Found" << endl;
    }
    else {
        string code;
        int address = 0;
        while (file >> code && address < 256) {
            if (isValidInstruction(code)) {
                memory.setcell(address, code);
                address += 2;
            }
        }
        if (address >= 256) {
            cout << "Memory limit reached, some instructions may not be loaded." << endl;
        }
    }
    file.close();
}

void CPU::decodeExecute(Register& registers, Memory& memory) {
    if (instructionRegister.empty()) {
        cout << "No instruction to execute." << endl;
        return;
    }

    char opcode = instructionRegister[0];
    // Ensure the instruction string is long enough
    if (instructionRegister.size() < 4) {
        cout << "Instruction register is too short." << endl;
        return;
    }

    int regIndex = stoi(instructionRegister.substr(1, 1), nullptr, 16);
    int operand1 = stoi(instructionRegister.substr(2, 1), nullptr, 16);
    int operand2 = stoi(instructionRegister.substr(3, 1), nullptr, 16);
    int address = operand1 * 16 + operand2;
    // Declare variables outside the switch statement
    int regValue = 0;
    int reg0Value = 0;
    switch (opcode) {
    case '1':
        cu.load(regIndex, address, registers, memory);
        break;
    case '2':
        registers.setreg(regIndex, instructionRegister.substr(2, 2));
        cout << "Loaded immediate value " << instructionRegister.substr(2, 2)
            << " into register R[" << regIndex << "]." << endl;
        break;
    case '3':
        if (address == 0) {
            cout << "Output to screen: " << registers.getreg(regIndex) << endl;
        }
        else {
            cu.store(regIndex, address, registers, memory);
        }
        break;
    case '4':
        cu.move(regIndex, operand1, registers);
        break;
    case '5': {
        string hexS = registers.getreg(operand1);
        string hexT = registers.getreg(operand2);
        string binS = hexToBinary(hexS);
        string binT = hexToBinary(hexT);
        string result = twosComplementAdd(binS, binT);
        result = binaryToHex(result);
        registers.setreg(regIndex, result);
        break;
    }
    case '6': {
        string hexS = registers.getreg(operand1);
        string hexT = registers.getreg(operand2);
        registers.setreg(regIndex, floatingPointAdd(hexS, hexT));
        break;
    }
    case '7':

        alu.AndOperation(registers, operand1, operand2, regIndex);
        cout << "AND result stored in R[" << regIndex << "]: "
            << registers.getreg(regIndex) << endl;
        break;


    case '8':
    {
        alu.OROperation(registers, operand1, operand2, regIndex);
        cout << "OR result stored in R[" << regIndex << "]: "
            << registers.getreg(regIndex) << endl;
        break;

    }

    case '9':
        alu.xorOperation(registers, regIndex, operand1, operand2);
        cout << "XOR result stored in R[" << regIndex << "]: "
            << registers.getreg(regIndex) << endl;
        break;

    case 'A':
        alu.rotateRight(registers, regIndex, operand1); // Assuming operand1 is the number of positions to rotate
        cout << "Rotate Right result stored in R[" << regIndex << "]: "
            << registers.getreg(regIndex) << endl;
        break;

    case 'B':
        if (registers.getreg(regIndex) == registers.getreg(0)) {
            cu.jump(address, programCounter);
        }
        break;
    case 'C':
        halt();
        break;
    case 'D':
        cu.copyToRegister(regIndex, address, registers, memory);
        break;

    case 'E' :
        regValue = alu.hexToSignedInt(registers.getreg(regIndex));
        reg0Value = alu.hexToSignedInt(registers.getreg(0));
        cout << "R0 :" << reg0Value << endl ;
        cout << "R" << regIndex << " :" << regValue << endl ;
        if (regValue > reg0Value) {
            cu.jump(address, programCounter);
            cout << " because R[" << regIndex << "] > R[0]." << endl;
        }
        else {
            cout << "No Jump" << " because R[" << regIndex << "] < R[0]." << endl; ;
        }
        break;
        default:
        cout << "Unknown opcode: " << opcode << endl;
        break;
    }
}


void CPU::halt()
{
    cout << "Execution halted." << endl;
    isHalted = true;
}

void Machine::run() {
    while (programCounter < 256 && !cpu.getIsHalted()) { // تحقق من حالة التوقف
        cpu.fetchInstruction(memory);
        cpu.decodeExecute(registers, memory);
    }
}


bool Machine::isValidInstruction(const string& instruction) {
    if (instruction.length() != 4) return false;
    char o = instruction[0];
    char r = instruction[1];
    char op1 = instruction[2];
    char op2 = instruction[3];

    switch (o) {
        case '1':
        case '2':
        case '3':
            return isxdigit(r) && isxdigit(op1) && isxdigit(op2);
        case '4':
            return r == '0' && isxdigit(op1) && isxdigit(op2);
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 'B':
        case 'D':
        case 'F':
            return isxdigit(r) && isxdigit(op1) && isxdigit(op2);
        case 'C':
            return instruction == "C000";
        case 'E' :
            return isxdigit(r) && isxdigit(op1) && isxdigit(op2);
        default:
            return false;
    }
}


void Machine::outputState(Register& registers, Memory& memory) {
    for (size_t i = 0; i < 16; i++){
        for (size_t j = 0; j < 16; j++)
        {
            v1[i][j] = memory.getcell(i * 16 + j);

        }


    }
    cout << "Machine State:" << endl;
    cout << "Registers:\n " << endl;
    for (int i = 0; i < 16; i++) {
        cout << "R[" << i << "]: " << registers.getreg(i) <<endl;
    }
    cout << endl;
    cout << "Memory:\n " << endl;
    for (size_t i = 0; i < 16; i++) {
        for (size_t j = 0; j < 16; j++)
        {
            cout << v1[i][j] << " ";

        }
        cout << endl;


    }
}

string ALU::hexToBinary(const string& hex)
{

    int num = stoi(hex, nullptr, 16);
    return bitset<8>(num).to_string();
}

string ALU::binaryToHex(const string& bin)
{
    int num = stoi(bin, nullptr, 2);
    stringstream ss;
    ss << hex << uppercase << num;
    string hexResult = ss.str();
    return hexResult.size() == 1 ? "0" + hexResult : hexResult;
}

string ALU::twosComplementAdd(const string& bin1, const string& bin2)
{
    int carry = 0;
    string result = "";

    for (int i = 7; i >= 0; --i) {
        int bit1 = bin1[i] - '0';
        int bit2 = bin2[i] - '0';
        int sum = bit1 + bit2 + carry;
        result = to_string(sum % 2) + result;
        carry = sum / 2;
    }
    return result.substr(result.size() - 8);
}

uint8_t ALU::hexToBinaryFP(const string& hex)
{
    return static_cast<uint8_t>(stoi(hex, nullptr, 16));
}

string ALU::binaryToHexFP(uint8_t binary)
{
    stringstream ss;
    ss << uppercase << hex << (binary & 0xFF);
    return ss.str();
}

double ALU::convertSEEEMMMMToDecimal(uint8_t seeemmmm)
{
    uint8_t S = (seeemmmm >> 7) & 0x01;
    uint8_t EEE = (seeemmmm >> 4) & 0x07;
    uint8_t MMMM = seeemmmm & 0x0F;
    double sign = (S == 0) ? 1.0 : -1.0;
    int exp = EEE;
    int bias = 4;
    int actual_exp = exp - bias;
    double mantissa_value = 0.0;
    for (int i = 0; i < 4; ++i) {
        if (MMMM & (1 << (3 - i))) {
            mantissa_value += 1.0 / pow(2, i + 1);
        }
    }
    return sign * mantissa_value * pow(2, actual_exp);
}

uint8_t ALU::encodeFloatingPoint(double decimal)
{
    if (decimal == 0.0) return 0;
    uint8_t S = (decimal < 0) ? 1 : 0;
    decimal = fabs(decimal);
    int exponent = 0;
    while (decimal >= 1.0) {
        decimal /= 2.0;
        exponent++;
    }
    while (decimal < 0.5 && decimal != 0.0) {
        decimal *= 2.0;
        exponent--;
    }
    int biasedExponent = exponent + 4;
    if (biasedExponent < 0 || biasedExponent > 7) return 0;

    uint8_t mantissa = 0;
    for (int i = 0; i < 4; ++i) {
        decimal *= 2.0;
        if (decimal >= 1.0) {
            mantissa |= (1 << (3 - i));
            decimal -= 1.0;
        }
    }
    return (S << 7) | (biasedExponent << 4) | mantissa;
}

string ALU::floatingPointAdd(const string& hex1, const string& hex2)
{
    uint8_t binary1 = hexToBinaryFP(hex1);
    uint8_t binary2 = hexToBinaryFP(hex2);
    double value1 = convertSEEEMMMMToDecimal(binary1);
    double value2 = convertSEEEMMMMToDecimal(binary2);
    double result = value1 + value2;
    uint8_t resultBinary = encodeFloatingPoint(result);
    return binaryToHexFP(resultBinary);
}

void ALU::xorOperation(Register& reg, int regR, int regS, int regT) {
    string regSHex = reg.getreg(regS);
    string regTHex = reg.getreg(regT);

    int regSInt = stoi(regSHex, nullptr, 16);
    int regTInt = stoi(regTHex, nullptr, 16);

    int xorResult = regSInt ^ regTInt;

    stringstream ss;
    ss << hex << uppercase << xorResult;
    string xorResultHex = ss.str();

    if (xorResultHex.size() == 1) {
        xorResultHex = "0" + xorResultHex;
    }
    reg.setreg(regR, xorResultHex);
}

void ALU::rotateRight(Register& reg, int regR, int X) {
    const int bits = 8; // Number of bits in the register
    string regRHex = reg.getreg(regR); // Get the hexadecimal value from the register
    int regRInt = stoi(regRHex, nullptr, 16); // Convert hex to int

    X = X % bits;

    // right rotation
    int result = (regRInt >> X) | (regRInt << (bits - X));

    // Convert result back to hexadecimal
    stringstream ss;
    ss << hex << uppercase << (result & 0xFF); // Keep only the last 8 bits
    string resultHex = ss.str();

    if (resultHex.size() == 1) {
        resultHex = "0" + resultHex;
    }

    reg.setreg(regR, resultHex);
}

void ALU::AndOperation(Register& reg, int reg1, int reg2, int regDest)
{
    int hex1 = stoi(reg.getreg(reg1),nullptr,16);
    int hex2 = stoi(reg.getreg(reg2), nullptr, 16);
    int result = hex1 & hex2;
    string hex_result = dec_to_hex(result);
    reg.setreg(regDest, hex_result);
}

void ALU::OROperation(Register& reg, int reg1, int reg2, int regDest)
{
    int hex1 = stoi(reg.getreg(reg1), nullptr, 16);
    int hex2 = stoi(reg.getreg(reg2), nullptr, 16);
    int result = hex1 | hex2;
    string hex_result = dec_to_hex(result);
    reg.setreg(regDest, hex_result);
}

string ALU::dec_to_hex(int num)
{
    string result = "";
    char hex_char[] = "0123456789ABCDEF";
    while (num ) {
        int R = num % 16;
        result = hex_char[R]+result;
        num /= 16;
    }
    return result ;
}



