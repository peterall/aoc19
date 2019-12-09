#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <functional>
#include <algorithm>

using namespace std;

enum STATE {
    INIT,
    READY,
    RUN,
    WAIT_FOR_INPUT,
    EXCEPTION,
    HALTED
};

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
        SREL = 9, // 1 param
        HALT = 99 // 0 params
    };

    enum MODE {
        POSITION = 0,
        IMMEDIATE = 1,
        RELATIVE = 2
    };
    
    template<OPCODE op, MODE mode1 = POSITION, MODE mode2 = POSITION, MODE mode3 = POSITION>
    constexpr static int make_instr() {
        return (int)op
            + (int)mode1 * 100
            + (int)mode2 * 1000
            + (int)mode3 * 10000;
    }

    inline void ensure_memory(const int addr) {
        if(addr >= memory.size()) {
            memory.resize(addr + 1, 0);
        }
    }

    template<MODE mode, int i>
    inline long long read() {
        const auto p = memory[pc+i];
        if(mode == IMMEDIATE) {
            return p;
        }
        const auto addr = mode == RELATIVE ? rel+p : p;
        ensure_memory(addr);
        return memory[addr];        
    }

    template<MODE mode, int i>
    inline void write(long long v) {
        const auto p = memory[pc+i];
        const auto addr = mode == RELATIVE ? rel+p : p;
        ensure_memory(addr);
        memory[addr] = v;
    }

    template<MODE mode1, MODE mode2, MODE mode3>
    void op_add() {
        write<mode3, 3>(read<mode1, 1>() + read<mode2, 2>());
        pc += 4;
    }

    template<MODE mode1, MODE mode2, MODE mode3>
    void op_mul() {
        write<mode3, 3>(read<mode1, 1>() * read<mode2, 2>());
        pc += 4;
    }

    template<MODE mode>
    void op_in() {
        if(input.empty()) {
            state = WAIT_FOR_INPUT;
        } else {
            write<mode, 1>(input.front());
            input.pop_front();
            pc += 2;
        }
    }

    template<MODE mode>
    void op_out() {
        output.push_back(read<mode, 1>());
        pc += 2;
    }

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

    template<MODE mode1, MODE mode2, MODE mode3>
    void op_lt() {
        write<mode3, 3>(read<mode1, 1>() < read<mode2, 2>() ? 1 : 0);
        pc += 4;
    }

    template<MODE mode1, MODE mode2, MODE mode3>
    void op_eq() {
        write<mode3, 3>(read<mode1, 1>() == read<mode2, 2>() ? 1 : 0);
        pc += 4;
    }

    template<MODE mode>
    void op_srel() {
        rel += read<mode, 1>();
        pc += 2;
    }

    void op_halt() {
        state = HALTED;
        pc = 0;
    }

    STATE state;
    vector<long long> program, memory;
    deque<long long> input, output;
    int pc, rel;

public:
    ShipComputer() {
    }

    ShipComputer(vector<long long> &program) : program(program) {
        reset();
    }

    void load(const string filename) {
        load_program(filename, program);
        reset();
    }

    static void load_program(const string filename, vector<long long> &program) {
        auto programfile = ifstream(filename);
        string instruction;
        while(getline(programfile, instruction, ',')) {
            if(!instruction.empty()) {
                program.push_back(stoll(instruction));
            }
        }
    }

    void dump_memory() const {
        for(auto const& i: memory) {
            cout << i << ",";
        }
        cout << endl;
    }

    void dump_output() const {
        for(auto const &o: output) {
            cout << o << ",";
        }
        cout << endl;
    }

    void write(long long value) {
        input.push_back(value);
    }

    bool can_read() const {
        return output.size() > 0;
    }

    long long read() {
        int value = output.front();
        output.pop_front();
        return value;
    }
    void reset() {
        memory = program;
        output = deque<long long>();
        input = deque<long long>();
        pc = rel = 0;
        state = READY;
    }
    STATE get_state() const { 
        return state;
    }

#define CASE_INSTR(name, fn) \
    case make_instr<name>(): fn(); break;

#define CASE_INSTR_R(name, fn) \
    case make_instr<name, POSITION>(): fn<POSITION>(); break; \
    case make_instr<name, IMMEDIATE>(): fn<IMMEDIATE>(); break; \
    case make_instr<name, RELATIVE>(): fn<RELATIVE>(); break;

#define CASE_INSTR_W(name, fn) \
    case make_instr<name, POSITION>(): fn<POSITION>(); break; \
    case make_instr<name, RELATIVE>(): fn<RELATIVE>(); break;

#define _CASE_INSTR_RR_2(name, fn, p1mode) \
    case make_instr<name, p1mode, POSITION>(): fn<p1mode, POSITION>(); break; \
    case make_instr<name, p1mode, IMMEDIATE>(): fn<p1mode, IMMEDIATE>(); break; \
    case make_instr<name, p1mode, RELATIVE>(): fn<p1mode, RELATIVE>(); break;

#define CASE_INSTR_RR(name, fn) \
    _CASE_INSTR_RR_2(name, fn, POSITION) \
    _CASE_INSTR_RR_2(name, fn, IMMEDIATE) \
    _CASE_INSTR_RR_2(name, fn, RELATIVE)

#define _CASE_INSTR_RRW_3(name, fn, p1mode, p2mode) \
    case make_instr<name, p1mode, p2mode, POSITION>(): fn<p1mode, p2mode, POSITION>(); break; \
    case make_instr<name, p1mode, p2mode, RELATIVE>(): fn<p1mode ,p2mode, RELATIVE>(); break; \

#define _CASE_INSTR_RRW_2(name, fn, p1mode) \
    _CASE_INSTR_RRW_3(name, fn, p1mode, POSITION) \
    _CASE_INSTR_RRW_3(name, fn, p1mode, IMMEDIATE) \
    _CASE_INSTR_RRW_3(name, fn, p1mode, RELATIVE)

#define CASE_INSTR_RRW(name, fn) \
    _CASE_INSTR_RRW_2(name, fn, POSITION) \
    _CASE_INSTR_RRW_2(name, fn, IMMEDIATE) \
    _CASE_INSTR_RRW_2(name, fn, RELATIVE)

    void run() {
        state = RUN;
        while(state == RUN) {
            switch(memory[pc]) {
                CASE_INSTR_RRW(ADD, op_add)
                CASE_INSTR_RRW(MUL, op_mul)
                CASE_INSTR_W(IN, op_in)
                CASE_INSTR_R(OUT, op_out)
                CASE_INSTR_RR(JT, op_jt)
                CASE_INSTR_RR(JF, op_jf)
                CASE_INSTR_RRW(LT, op_lt)
                CASE_INSTR_RRW(EQ, op_eq)
                CASE_INSTR_R(SREL, op_srel);
                CASE_INSTR(HALT, op_halt)

                default:
                    cout << "ILLEGAL INSTRUCTION " << memory[pc] << endl;
                    state = EXCEPTION;
                    return;
            }
        }
    }
};


int main(int argc, char *argv[]) {
    auto computer = ShipComputer();
    computer.load("aoc9.txt");

    computer.write(2);
    computer.run();

    computer.dump_output();
    
    cout << "Done!" << endl;
    return 0;
};
