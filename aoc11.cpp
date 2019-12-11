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
    HALT
};

class IntcodeComputer {
private:
    enum OPCODE {
        OP_ADD = 1, // 3 params
        OP_MUL = 2, // 3 params
        OP_IN = 3,  // 1 param
        OP_OUT = 4, // 1 param
        OP_JT = 5,  // 2 params
        OP_JF = 6,  // 2 params
        OP_LT = 7,  // 3 params
        OP_EQ = 8,  // 3 params
        OP_SREL = 9, // 1 param
        OP_HALT = 99 // 0 params
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
        if(addr >= _memory.size()) {
            _memory.resize(addr + 1, 0);
        }
    }

    template<MODE mode, int i>
    inline long long read() {
        const auto p = _memory[_pc+i];
        if(mode == IMMEDIATE) {
            return p;
        }
        const auto addr = mode == RELATIVE ? _rel+p : p;
        ensure_memory(addr);
        return _memory[addr];        
    }

    template<MODE mode, int i>
    inline void write(long long v) {
        const auto p = _memory[_pc+i];
        const auto addr = mode == RELATIVE ? _rel+p : p;
        ensure_memory(addr);
        _memory[addr] = v;
    }

    template<MODE mode1, MODE mode2, MODE mode3>
    void op_add() {
        write<mode3, 3>(read<mode1, 1>() + read<mode2, 2>());
        _pc += 4;
    }

    template<MODE mode1, MODE mode2, MODE mode3>
    void op_mul() {
        write<mode3, 3>(read<mode1, 1>() * read<mode2, 2>());
        _pc += 4;
    }

    template<MODE mode>
    void op_in() {
        if(_input.empty()) {
            _state = WAIT_FOR_INPUT;
        } else {
            write<mode, 1>(_input.front());
            _input.pop_front();
            _pc += 2;
        }
    }

    template<MODE mode>
    void op_out() {
        _output.push_back(read<mode, 1>());
        _pc += 2;
    }

    template<MODE mode1, MODE mode2>
    void op_jt() {
        if(read<mode1, 1>() != 0) {
            _pc = read<mode2, 2>();
        } else {
            _pc += 3;
        }
    }

    template<MODE mode1, MODE mode2>
    void op_jf() {
        if(read<mode1, 1>() == 0) {
            _pc = read<mode2, 2>();
        } else {
            _pc += 3;
        }
    }

    template<MODE mode1, MODE mode2, MODE mode3>
    void op_lt() {
        write<mode3, 3>(read<mode1, 1>() < read<mode2, 2>() ? 1 : 0);
        _pc += 4;
    }

    template<MODE mode1, MODE mode2, MODE mode3>
    void op_eq() {
        write<mode3, 3>(read<mode1, 1>() == read<mode2, 2>() ? 1 : 0);
        _pc += 4;
    }

    template<MODE mode>
    void op_srel() {
        _rel += read<mode, 1>();
        _pc += 2;
    }

    void op_halt() {
        _state = HALT;
        _pc = 0;
    }

    STATE _state;
    vector<long long> _program, _memory;
    deque<long long> _input, _output;
    int _pc, _rel;

public:
    IntcodeComputer() {
    }

    IntcodeComputer(vector<long long> &program) : _program(program) {
        reset();
    }

    void load(const string filename) {
        load_program(filename, _program);
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
        for(auto const& i: _memory) {
            cout << i << ",";
        }
        cout << endl;
    }

    void dump_output() const {
        for(auto const &o: _output) {
            cout << o << ",";
        }
        cout << endl;
    }

    void write(long long value) {
        _input.push_back(value);
    }

    bool can_read() const {
        return _output.size() > 0;
    }

    long long read() {
        if(_output.size() == 0) {
            cout << "WARNING: No output to read" << endl;
            return 0;
        }
        int value = _output.front();
        _output.pop_front();
        return value;
    }
    void reset() {
        _memory = _program;
        _output = deque<long long>();
        _input = deque<long long>();
        _pc = _rel = 0;
        _state = READY;
    }
    STATE state() const { 
        return _state;
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
        _state = RUN;
        while(_state == RUN) {
            switch(_memory[_pc]) {
                CASE_INSTR_RRW (OP_ADD, op_add)
                CASE_INSTR_RRW (OP_MUL, op_mul)
                CASE_INSTR_W   (OP_IN, op_in)
                CASE_INSTR_R   (OP_OUT, op_out)
                CASE_INSTR_RR  (OP_JT, op_jt)
                CASE_INSTR_RR  (OP_JF, op_jf)
                CASE_INSTR_RRW (OP_LT, op_lt)
                CASE_INSTR_RRW (OP_EQ, op_eq)
                CASE_INSTR_R   (OP_SREL, op_srel);
                CASE_INSTR     (OP_HALT, op_halt)

                default:
                    cout << "ILLEGAL INSTRUCTION " << _memory[_pc] << endl;
                    _state = EXCEPTION;
                    return;
            }
        }
    }
};

enum Direction {
    DirectionUp,
    DirectionRight,
    DirectionDown,
    DirectionLeft
};

int main(int argc, char *argv[]) {
    map<pair<int,int>,bool> hull = {
        {make_pair(0,0), true}
    };
    pair<int,int> position = {};
    Direction direction = {};

    auto computer = IntcodeComputer();
    computer.load("aoc11.txt");    
    computer.run();
    while(computer.state() == WAIT_FOR_INPUT) {
        bool color = false;
        if(hull.count(position) == 0) {
            hull.insert({position, color});
        } else {
            color = hull[position];
        }

        computer.write(color ? 1 : 0);
        computer.run();

        hull[position] = computer.read() == 1;
        auto newdirection = computer.read();
        direction = (Direction)((direction + (newdirection ? 1 : 3)) % 4);
        switch(direction) {
            case DirectionUp: position = make_pair(position.first, position.second - 1); break;
            case DirectionRight: position = make_pair(position.first + 1, position.second); break;
            case DirectionDown: position = make_pair(position.first, position.second + 1); break;
            case DirectionLeft: position = make_pair(position.first - 1, position.second); break;
        }
    }

    cout << "total painted:" << hull.size() << endl;

    int offsetx = INT_MAX, offsety = INT_MAX, limitx = INT_MIN, limity = INT_MIN;
    for(const auto &p: hull) {
        offsetx = min(p.first.first, offsetx);
        offsety = min(p.first.second, offsety);
        limitx = max(p.first.first, limitx);
        limity = max(p.first.second, limity);
    }

    for(int y=offsety-1;y<limity+2;y++) {
        for(int x=offsetx-1;x<limitx+2;x++) {
            cout << ((hull.count(make_pair(x,y)) > 0 && hull[make_pair(x,y)]) ? "X" : " ");
        }
        cout << endl;
    }

    cout << "Done!" << endl;
    return 0;
};
