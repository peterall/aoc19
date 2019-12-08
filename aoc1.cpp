#include <iostream>
#include <fstream>

using namespace std;

long requiredfuel(long mass) {
    return max((mass / 3l) - 2l, 0l);
}

int main(int argc, char *argv[]) {

    auto input = ifstream("aoc1_input.txt");
    string line;
    auto totalfuel = 0l;
    while(getline(input, line)) {
        auto modulefuel = requiredfuel(stol(line));

        auto fuelforfuel = requiredfuel(modulefuel);
        while(fuelforfuel > 0) {
            totalfuel += fuelforfuel;
            fuelforfuel = requiredfuel(fuelforfuel);
        }
        totalfuel += modulefuel + fuelforfuel;
    }
    cout << "Total fuel needed: " << totalfuel << endl;

    cout << "Done!" << endl;
    return 0;
}