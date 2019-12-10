#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <functional>
#include <cmath>
#include <algorithm>

using namespace std;

class Star {
private:
    int _x, _y;
public:
    Star(int x, int y): _x(x), _y(y) {
    }
    int x() const { return _x; }
    int y() const { return _y; }
    const double angle_to(const Star &star) const {
        return atan2(star._y - _y, star._x - _x);
    }
    const double distance_to(const Star &star) const {
        return sqrt(pow(abs(star._y - _y), 2) + pow(abs(star._x - _x), 2));
    }
    ostream &operator<<(std::ostream &os) { 
        return os << _x << "," << _y;
    }
};

std::ostream &operator<<(std::ostream &os, Star const &star) { 
    return os << star.x() << "," << star.y();
}
class Starfield {
private:
    vector<bool> _starfield;
    vector<Star> _stars; 
    int _width = 0, _height = 0;
public:
    bool get(int x, int y) const {
        return _starfield[(y*_width)+x];
    }
    int width() const {
        return _width;
    }
    int height() const {
        return _height;
    }
    const vector<Star> &stars() const {
        return _stars;
    }
    Starfield(const string filename) {
        auto file = ifstream(filename);
        string line;
        _starfield = vector<bool>();
        _stars = vector<Star>();
        while(getline(file, line)) {
            int x = 0;
            for(const auto &c: line) {
                auto is_star = c == '#';
                _starfield.push_back(is_star);
                if(is_star) {
                    _stars.push_back(Star(x, _height));
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

    for(const auto &star: sf.stars()) {
        auto others = sf.stars();
        sort(others.begin(), others.end(), [&star](const Star &a, const Star &b) {
            return star.distance_to(a) < star.distance_to(b);
        });
        for(const auto &relative: others) {
            cout << relative << " (" << star.distance_to(relative) << ", " << star.angle_to(relative) << ")" << endl;
        }
        
        break;
    }

/*
    for(int y=0;y<sf.height();y++) {
        for(int x=0;x<sf.width();x++) {
            cout << (sf.get(x,y) ? "#" : ".");
        }
        cout << endl;
    }
*/

    cout << "Done!" << endl;
    return 0;
};