#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <functional>

using namespace std;

class ShipComputer {
private:
    enum OPCODE {
        ADD = 1, // 3 params
        MUL = 2, // 3 params
        IN = 3,  // 1 param
        OUT = 4, // 1 param
        JT = 5,  // 2 params
        JF = 6,  // 2 params
        LT = 7,  // 3 params
        EQ = 8,  // 3 params
        HALT = 99 // 0 params
    };
    enum MODE {
        POSITION = 0,
        IMMEDIATE = 1
    };
    
    template<OPCODE op, MODE mode1 = POSITION, MODE mode2 = POSITION, MODE mode3 = POSITION>
    constexpr static int make_instr() {
        int instr = (int)op;
        if(mode1 == IMMEDIATE) {
            instr += 100;
        }
        if(mode2 == IMMEDIATE) {
            instr += 1000;
        }
        if(mode3 == IMMEDIATE) {
            instr += 10000;
        }
        return instr;
    };

    template<MODE mode, int i>
    inline int read() {
        int p = memory[pc+i];
        return mode == POSITION ? memory[p] : p;
    }
    template<int i>
    inline void write(int v) {
        int p = memory[pc+i];
        memory[p] = v;
    }

    template<MODE mode1, MODE mode2>
    void op_add() {
        write<3>(read<mode1, 1>() + read<mode2, 2>());
        pc += 4;
    };

    template<MODE mode1, MODE mode2>
    void op_mul() {
        write<3>(read<mode1, 1>() * read<mode2, 2>());
        pc += 4;
    };

    void op_in() {
        int in;
        cout << "INPUT: ";
        cin >> in;
        write<1>(in);
        pc += 2;
    };

    template<MODE mode>
    void op_out() {
        cout << read<mode, 1>() << endl;
        pc += 2;
    };

    template<MODE mode1, MODE mode2>
    void op_jt() {
        if(read<mode1, 1>() != 0) {
            pc = read<mode2, 2>();
        } else {
            pc += 3;
        }
    }

    template<MODE mode1, MODE mode2>
    void op_jf() {
        if(read<mode1, 1>() == 0) {
            pc = read<mode2, 2>();
        } else {
            pc += 3;
        }
    }

    template<MODE mode1, MODE mode2>
    void op_lt() {
        write<3>(read<mode1, 1>() < read<mode2, 2>() ? 1 : 0);
        pc += 4;
    }

    template<MODE mode1, MODE mode2>
    void op_eq() {
        write<3>(read<mode1, 1>() == read<mode2, 2>() ? 1 : 0);
        pc += 4;
    }

    void op_halt() {
        pc = -1;
    };

    vector<int> memory;
    int pc;

public:
    ShipComputer() {
    }
    void load(const string filename) {
        memory.clear();
        auto programfile = ifstream(filename);
        string instruction;
        while(getline(programfile, instruction, ',')) {
            if(!instruction.empty()) {
                memory.push_back(stoi(instruction));
            }
        }
    }
    void dump() const {
        for(auto const& i: memory) {
            cout << i << ",";
        }
        cout << endl;
    }

#define CASE_INSTR_0(name, fn) \
    case make_instr<name>(): fn(); break;

#define CASE_INSTR_1(name, fn) \
    case make_instr<name, POSITION>(): fn<POSITION>(); break; \
    case make_instr<name, IMMEDIATE>(): fn<IMMEDIATE>(); break;

#define CASE_INSTR_2(name, fn) \
    case make_instr<name, POSITION, POSITION>(): fn<POSITION,POSITION>(); break; \
    case make_instr<name, IMMEDIATE, POSITION>(): fn<IMMEDIATE,POSITION>(); break; \
    case make_instr<name, POSITION, IMMEDIATE>(): fn<POSITION,IMMEDIATE>(); break; \
    case make_instr<name, IMMEDIATE, IMMEDIATE>(): fn<IMMEDIATE,IMMEDIATE>(); break;

    void run() {
        pc = 0;
        while(pc != -1) {
            //dump();
            switch(memory[pc]) {
                CASE_INSTR_2(ADD, op_add)
                CASE_INSTR_2(MUL, op_mul)
                CASE_INSTR_0(IN, op_in)
                CASE_INSTR_1(OUT, op_out)
                CASE_INSTR_2(JT, op_jt)
                CASE_INSTR_2(JF, op_jf)
                CASE_INSTR_2(LT, op_lt)
                CASE_INSTR_2(EQ, op_eq) 
                CASE_INSTR_0(HALT, op_halt)

                default:
                    cout << "ILLEGAL INSTRUCTION " << memory[pc] << endl;
                    return;
            }
        }
    }
};

int main(int argc, char *argv[]) {
    ShipComputer computer;
    computer.load("aoc5_program.txt");
    computer.run();

    cout << "Done!" << endl;
    return 0;
};
