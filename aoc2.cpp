#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

void dump(vector<int> &memory) {
    for(auto const& i: memory) {
        cout << i << ",";
    }
    cout << endl;
}

void readprogram(vector<int> &memory, const string filename) {
    auto programfile = ifstream(filename);
    string instruction;
    while(getline(programfile, instruction, ',')) {
        if(!instruction.empty()) {
            memory.push_back(stoi(instruction));
        }
    }
}

enum OPCODE {
    ADD = 1,
    MUL = 2,
    HALT = 99
};

void run(vector<int> &m) {
    int pc = 0;
    while(true) {
        auto instr = (OPCODE)m[pc++];
        switch(instr) {
            case ADD: {
                int p0 = m[pc++];
                int p1 = m[pc++];
                int dest = m[pc++];
                m[dest] = m[p0] + m[p1];
                break;
            }
            case MUL:{
                int p0 = m[pc++];
                int p1 = m[pc++];
                int dest = m[pc++];
                m[dest] = m[p0] * m[p1];
                break;
            }
            case HALT:
                return;
            default:
                cout << "UNKNOWN INSTRUCTION " << instr << endl;
                return;
        }
    }
}

int main(int argc, char *argv[]) {
    auto program = vector<int>();
    readprogram(program, "aoc2_program.txt");

    for(int noun = 0;noun <= 99; noun++) {
        for(int verb = 0;verb <= 99; verb++) {
            auto memory = program;
            memory[1] = noun;
            memory[2] = verb;
            run(memory);
            if(memory[0] == 19690720) {
                cout << "NOUN " << noun << ", VERB " << verb << endl;
                return 1;
            }
        }
    }

    cout << "Done!" << endl;
    return 0;
}