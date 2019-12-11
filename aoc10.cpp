#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <functional>
#include <cmath>
#include <algorithm>
#include <iomanip>

using namespace std;

static double are_close(const double a, const double b) {
    return abs(a-b) < 5e-10;
}

#define M_2PI (M_PI * 2)

class Asteroid {
private:
    int _x, _y;
    bool _destroyed;
public:
    Asteroid() : _x(0), _y(0), _destroyed(false) {
    }
    Asteroid(int x, int y): _x(x), _y(y), _destroyed(false) {
    }
    int x() const { return _x; }
    int y() const { return _y; }
    void destroy() {
        _destroyed = true;
    }
    bool is_destroyed() const {
        return _destroyed;
    }
    const double angle_to(const Asteroid &asteroid) const {
        return fmod(atan2(asteroid._y - _y, asteroid._x - _x) + M_PI_2 + M_2PI, M_2PI);
    }
    const double distance_to(const Asteroid &asteroid) const {
        return sqrt(pow(abs(asteroid._y - _y), 2) + pow(abs(asteroid._x - _x), 2));
    }
    ostream &operator<<(std::ostream &os) { 
        return os << _x << "," << _y;
    }
    bool operator==(const Asteroid &a) const {
        return a._x == _x && a._y == _y && a._destroyed == _destroyed;
    }
    bool operator!=(const Asteroid &a) const {
        return !(a == *this);
    }

};

std::ostream &operator<<(std::ostream &os, Asteroid const &asteroid) { 
    return os << asteroid.x() << "," << asteroid.y();
}
class AsteroidField {
private:
    vector<bool> _asteroidfield;
    vector<Asteroid> _asteroids; 
    int _width = 0, _height = 0;
public:
    bool get(int x, int y) const {
        return _asteroidfield[(y*_width)+x];
    }
    int width() const {
        return _width;
    }
    int height() const {
        return _height;
    }
    const vector<Asteroid> &asteroids() const {
        return _asteroids;
    }
    void sweep(const Asteroid &asteroid, vector<Asteroid> &others) {
        copy_if(_asteroids.begin(), _asteroids.end(), back_inserter(others), [&asteroid](const Asteroid &a) {
            return a != asteroid;
        });
        sort(others.begin(), others.end(), [&asteroid](const Asteroid &a, const Asteroid &b) {
            const auto angle_a = asteroid.angle_to(a), angle_b = asteroid.angle_to(b);
            if(are_close(angle_a, angle_b)) {
                return asteroid.distance_to(a) < asteroid.distance_to(b); 
            } else {
                return angle_a < angle_b;
            }
        });
    }
    AsteroidField(const string filename) {
        auto file = ifstream(filename);
        string line;
        _asteroidfield = vector<bool>();
        _asteroids = vector<Asteroid>();
        while(getline(file, line)) {
            int x = 0;
            for(const auto &c: line) {
                auto is_asteroid = c == '#';
                _asteroidfield.push_back(is_asteroid);
                if(is_asteroid) {
                    _asteroids.push_back(Asteroid(x, _height));
                }
                x++;
            }
            _width = line.length();
            _height++;
        }
    }
};


int main(int argc, char *argv[]) {
    AsteroidField sf("aoc10.txt");

    Asteroid best_asteroid;
    int best_asteroid_seen = -1;
    optional<pair<Asteroid,int>> best_asteroid_count = {};
    for(const auto &asteroid: sf.asteroids()) {
        vector<Asteroid> others;
        sf.sweep(asteroid, others);
        double last_angle = -1;
        int seen = 0;
        for(const auto &relative: others) {
            const auto angle = asteroid.angle_to(relative);
            if(!are_close(angle, last_angle)) {
                seen++;
            }
            last_angle = angle;
        }
        if(seen > best_asteroid_seen) {
            best_asteroid = asteroid;
            best_asteroid_seen = seen;
        }
    }

    cout << "Best asteroid at " << best_asteroid << " with " 
        << best_asteroid_seen << " others seen" << endl ;

    vector<Asteroid> others;
    sf.sweep(best_asteroid, others);
    int number_destroyed = 0;
    while(number_destroyed < 200) {
        double last_angle = -1;
        for(auto &relative: others) {
            if(!relative.is_destroyed()) {
                const auto angle = best_asteroid.angle_to(relative);
                if(!are_close(angle, last_angle)) {
                    relative.destroy();
                    number_destroyed++;
                    if(number_destroyed == 200) {
                        cout << "no " << number_destroyed << " is " << relative << endl;
                    }
                }
                last_angle = angle;
            }
        }
    }    

    cout << "Done!" << endl;
    return 0;
};