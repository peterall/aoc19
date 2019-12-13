#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <functional>
#include <algorithm>
#include <tuple>
#include <regex>
#include <numeric>
#include <array>

using namespace std;

// struct Vector3 {
//     int x, y, z;
//     int energy() const {
//         return abs(x) + abs(y) + abs(z);
//     }
//     bool operator==(const Vector3 &other) const {
//         return x == other.x && y == other.y && z == other.z;
//     }
// };

class Moon {
    int gravity(const int &first, const int &second) {
        if(first == second) {
            return 0;
        } else {
            return first > second ? -1 : 1;
        }
    }
    array<int, 3> _position, _velocity;
public:
    Moon(int x, int y, int z) : _position({x,y,z}), _velocity({}) {
    }
    const array<int, 3> &position() const{
        return _position;
    }
    const array<int, 3> &velocity() const {
        return _velocity;
    }
    void apply_gravity(const Moon &other) {
        for(int i=0; i<3; i++) {
            _velocity[i] += gravity(_position[i], other._position[i]);
        }
    }
    void step() {
        for(int i=0; i<3; i++) {
            _position[i] += _velocity[i];
        }
    }
    int energy() const {
        int potential = 0, kinetic = 0;
        for(int i=0; i<3; i++) {
            potential += abs(_position[i]);
            kinetic += abs(_velocity[i]);
        }
        return potential * kinetic;
    }
    bool operator==(const Moon &other) const {
        return _position == other._position && _velocity == other._velocity;
    }
};

ostream &operator<<(ostream &str, const array<int, 3> &v) {
    str << "<x=" << setw(3) << v[0] << ", y=" << setw(3) << v[1] << ", z=" << setw(3) << v[2] << ">";
    return str;
}

ostream &operator<<(ostream &str, const Moon &moon) {
    str << "pos=" << moon.position() << ", vel=" << moon.velocity();
    return str;
}

static void load(const string filename, vector<Moon> &moons) {
    auto file = ifstream(filename);
    string line;
    regex pattern("<x=([-0-9]+), y=([-0-9]+), z=([-0-9]+)>");
    while(getline(file, line)) {
        smatch matches;
        if(regex_match(line, matches, pattern)) {
            moons.push_back(Moon(stoi(matches[1]), stoi(matches[2]), stoi(matches[3])));    
        }
    }
}

int energy(const vector<Moon> &moons) {
    return reduce(moons.begin(), moons.end(), 0, [](int sum, const Moon &moon) {
        return sum + moon.energy();
    });
}


void dump(const vector<Moon> &moons, const long &step) {
    cout << "After " << step << " steps:" << endl;
    for(const auto &moon: moons) {
        cout << moon << endl;
    }
    cout << "Energy: " << energy(moons) << endl << endl;
}

int main(int argc, char *argv[]) {
    vector<Moon> moons = {};
    load("aoc12.txt", moons);
    auto initial = moons;
    array<long long, 3> cycles = {-1, -1, -1};
    // const array<string, 3> dims = {"x", "y", "z"};
    const long max_steps = 3000;

    // dump(moons, 0);
    for(long step=0; cycles[0] == -1 || cycles[1] == -1 || cycles[2] == -1; step++) {
        for(int first=0; first<moons.size(); first++) {
            for(int second=first+1; second<moons.size(); second++) {
                moons[first].apply_gravity(moons[second]);
                moons[second].apply_gravity(moons[first]);
            }
        }
        for(auto &moon: moons) {
            moon.step();
        }

        // dump(moons, step+1);
        array<bool, 3> has_cycle = {true, true, true};
        for(int moon=0; moon<moons.size(); moon++) {
            for(int i=0; i<3; i++) {
                has_cycle[i] = has_cycle[i] 
                    && initial[moon].position()[i] == moons[moon].position()[i] 
                    && moons[moon].velocity()[i] == 0;
            }
        }
        for(int i=0; i<3; i++) {
            if(has_cycle[i]) {
                cycles[i] = (cycles[i] == -1 ? step+1 : cycles[i]);
            }
        }
    }

    cout << "cycles=<x=" << cycles[0] << ", y=" << cycles[1] << ", z=" << cycles[2] << ">" << endl;
    cout << "comon cycle at: " << lcm(lcm(cycles[0], cycles[1]), lcm(cycles[1], cycles[2])) << endl;

    cout << "Done!" << endl;
    return 0;
};