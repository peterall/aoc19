#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <functional>
#include <algorithm>

using namespace std;

class Starfield {
private:
    vector<bool> starfield;
    vector<pair<int,int>> stars; 
    int _width = 0, _height = 0;
public:
    bool get(int x, int y) const {
        return starfield[(y*_width)+x];
    }
    int width() const {
        return _width;
    }
    int height() const {
        return _height;
    }
    const vector<pair<int,int>> &stars() const {
        return stars;
    }
    Starfield(const string filename) {
        auto file = ifstream(filename);
        string line;
        starfield = vector<bool>();
        while(getline(file, line)) {
            int x = 0;
            for(const auto &c: line) {
                auto is_star = c == '#';
                starfield.push_back(is_star);
                if(is_star) {
                    stars.push_back(make_pair(x, _height));
                }
                x++;
            }
            _width = line.length();
            _height++;
        }
    }
};


int main(int argc, char *argv[]) {
    Starfield sf("aoc10.txt");

    for(int y=0;y<sf.height();y++) {
        for(int x=0;x<sf.width();x++) {
            cout << (sf.get(x,y) ? "#" : ".");
        }
        cout << endl;
    }


    cout << "Done!" << endl;
    return 0;
};