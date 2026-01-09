#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>

using namespace std;

/* =========================
   Instruction Definition
   ========================= */
struct Instruction {
    string opcode;
    int rd, rs1, rs2;
    int memAddr;

    Instruction() {
        opcode = "NOP";
        rd = rs1 = rs2 = memAddr = -1;
    }
};

/* =========================
   Pipeline Registers
   ========================= */
struct IF_ID { Instruction instr; };
struct ID_EX { Instruction instr; int val1 = 0, val2 = 0; };
struct EX_MEM { Instruction instr; int aluResult = 0, val2 = 0; };
struct MEM_WB { Instruction instr; int memData = 0, aluResult = 0; };

/* =========================
   CPU Pipeline Simulator
   ========================= */
class PipelineCPU {
private:
    vector<Instruction> instructionMemory;
    map<int, int> dataMemory;
    int registers[8];
    int pc, cycle;

    IF_ID if_id;
    ID_EX id_ex;
    EX_MEM ex_mem;
    MEM_WB mem_wb;

public:
    PipelineCPU() { resetCPU(); }

    /* =========================
       Reset CPU State
       ========================= */
    void resetCPU() {
        instructionMemory.clear();
        dataMemory.clear();
        pc = 0;
        cycle = 0;

        for (int i = 0; i < 8; i++)
            registers[i] = 0;

        if_id = IF_ID();
        id_ex = ID_EX();
        ex_mem = EX_MEM();
        mem_wb = MEM_WB();
    }

    /* =========================
       Parse Instruction
       ========================= */
    Instruction parseInstruction(const string& line) {
        stringstream ss(line);
        Instruction instr;
        ss >> instr.opcode;

        if (instr.opcode == "ADD" || instr.opcode == "SUB")
            ss >> instr.rd >> instr.rs1 >> instr.rs2;
        else if (instr.opcode == "LOAD")
            ss >> instr.rd >> instr.memAddr;
        else if (instr.opcode == "STORE")
            ss >> instr.rs1 >> instr.memAddr;
        else
            instr.opcode = "NOP";

        return instr;
    }

    /* =========================
       Memory Initialization
       ========================= */
    void initMemory() {
        int n;
        cout << "Enter number of memory locations to initialize: ";
        cin >> n;

        for (int i = 0; i < n; i++) {
            int addr, value;
            cout << "Address Value: ";
            cin >> addr >> value;
            dataMemory[addr] = value;
        }
    }

    /* =========================
       Load Program
       ========================= */
    void loadProgram() {
        int n;
        cout << "Enter number of instructions: ";
        cin >> n;
        cin.ignore();

        cout << "Enter instructions:\n";
        for (int i = 0; i < n; i++) {
            string line;
            getline(cin, line);
            instructionMemory.push_back(parseInstruction(line));
        }
    }

    /* =========================
       Load-Use Hazard Detection
       ========================= */
    bool hasLoadUseHazard() {
        if (id_ex.instr.opcode == "LOAD") {
            int loadReg = id_ex.instr.rd;

            if (if_id.instr.opcode == "ADD" || if_id.instr.opcode == "SUB") {
                return (if_id.instr.rs1 == loadReg ||
                        if_id.instr.rs2 == loadReg);
            }
            if (if_id.instr.opcode == "STORE") {
                return (if_id.instr.rs1 == loadReg);
            }
        }
        return false;
    }

    /* =========================
       Pipeline Stages
       ========================= */
    void fetch() {
        if (pc < instructionMemory.size())
            if_id.instr = instructionMemory[pc++];
        else
            if_id.instr = Instruction();
    }

    void decode() {
        if (hasLoadUseHazard()) {
            // Insert stall (bubble)
            id_ex.instr = Instruction();
            return;
        }

        id_ex.instr = if_id.instr;

        if (id_ex.instr.opcode == "ADD" || id_ex.instr.opcode == "SUB") {
            id_ex.val1 = registers[id_ex.instr.rs1];
            id_ex.val2 = registers[id_ex.instr.rs2];
        } else if (id_ex.instr.opcode == "STORE") {
            id_ex.val1 = registers[id_ex.instr.rs1];
        }
    }

    void execute() {
        ex_mem.instr = id_ex.instr;

        if (ex_mem.instr.opcode == "ADD")
            ex_mem.aluResult = id_ex.val1 + id_ex.val2;
        else if (ex_mem.instr.opcode == "SUB")
            ex_mem.aluResult = id_ex.val1 - id_ex.val2;
        else if (ex_mem.instr.opcode == "LOAD" || ex_mem.instr.opcode == "STORE") {
            ex_mem.aluResult = ex_mem.instr.memAddr;
            ex_mem.val2 = id_ex.val1;
        }
    }

    void memoryAccess() {
        mem_wb.instr = ex_mem.instr;

        if (mem_wb.instr.opcode == "LOAD")
            mem_wb.memData = dataMemory[ex_mem.aluResult];
        else if (mem_wb.instr.opcode == "STORE")
            dataMemory[ex_mem.aluResult] = ex_mem.val2;
        else
            mem_wb.aluResult = ex_mem.aluResult;
    }

    void writeBack() {
        if (mem_wb.instr.opcode == "ADD" || mem_wb.instr.opcode == "SUB")
            registers[mem_wb.instr.rd] = mem_wb.aluResult;
        else if (mem_wb.instr.opcode == "LOAD")
            registers[mem_wb.instr.rd] = mem_wb.memData;
    }

    /* =========================
       Run CPU
       ========================= */
    void run() {
        while (pc < instructionMemory.size() ||
               if_id.instr.opcode != "NOP" ||
               id_ex.instr.opcode != "NOP" ||
               ex_mem.instr.opcode != "NOP" ||
               mem_wb.instr.opcode != "NOP") {

            cycle++;
            writeBack();
            memoryAccess();
            execute();
            decode();
            fetch();
            printState();
        }
    }

    void printState() {
        cout << "\nCycle " << cycle << "\nRegisters: ";
        for (int i = 0; i < 8; i++)
            cout << "R" << i << "=" << registers[i] << " ";
        cout << "\n---------------------------\n";
    }
};

/* =========================
   Main Loop
   ========================= */
int main() {
    PipelineCPU cpu;
    char choice;

    do {
        cpu.resetCPU();
        cpu.initMemory();
        cpu.loadProgram();
        cpu.run();

        cout << "\nRun another program? (y/n): ";
        cin >> choice;
    } while (choice == 'y' || choice == 'Y');

    return 0;
}
