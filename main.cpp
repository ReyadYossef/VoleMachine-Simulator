#include"VoleMachine.h"
int main() {
    Machine myMachine;
    myMachine.loadProgramFile("program.txt"); // تأكد من وجود ملف التعليمات
    myMachine.run(); // تشغيل البرنامج
    myMachine.outputState(myMachine.getRegisters(), myMachine.getMemory());
    return 0;
}
