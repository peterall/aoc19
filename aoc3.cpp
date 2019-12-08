#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <optional>

using namespace std;

class point {
public:
    int x, y;
    int manhattan_distance() const {
        return abs(x)+abs(y);
    }
    bool is_origin() const {
        return x == 0 && y == 0;
    }
};
enum direction {
    VERTICAL = 0,
    HORIZONTAL = 1
};

class line {
private:
public:
    line(point a, point b, int distance) : distance(distance) {
        if(a.x < b.x || a.y < b.y) {
            start = a;
            end = b;
            flipped = false;
        } else {
            start = b;
            end = a; 
            flipped = true;
        }
    }
    point start, end;
    int distance;
    bool flipped;
//    point start() const {
//        return start;
//    }
//    point end() const {
//        return end;
//    }
    direction direction() const {
        if(start.x == end.x) {
            return VERTICAL;
        } else {
            return HORIZONTAL;
        }
    }
    static bool intersects_hv(const line &h, const line &v, point &intersection, int &length) {
        // overlaps horizontally?
        if(v.start.x >= h.start.x && v.start.x <= h.end.x &&
                h.start.y >= v.start.y && h.start.y < v.end.y) {
                    auto x = v.start.x;
                    auto y = h.start.y;

                    intersection.x = x;
                    intersection.y = y;

                    auto hoffset = !h.flipped ? x - h.start.x : h.end.x - x;
                    auto voffset = !v.flipped ? y - v.start.y : v.end.y - y;

                    length =  h.distance + abs(hoffset) + v.distance + abs(voffset);

                    return true;
                }
        return false;
    }
    bool intersects(const line &line, point &intersection, int &length) const {
        auto dir1 = direction(), dir2 = line.direction();
        // let's assume lines never overlap going the same direction
        if(dir1 != dir2) {
            return intersects_hv(
                dir1 == HORIZONTAL ? *this : line, 
                dir2 == VERTICAL ? line : *this, 
                intersection,
                length);
        }
        return false;
    }
};

void parselines(vector<line> &lines, string input) {
    auto position = point();
    int totallength = 0;
    stringstream is(input);
    string command;
    while(getline(is, command, ',')) {
        char direction = command[0];
        int distance = stoi(command.substr(1));
        auto origin = position;
        switch(direction) {
            case 'U':
                position.y += distance;
                break;
            case 'D':
                position.y -= distance;
                break;
            case 'R':
                position.x += distance;
                break;
            case 'L':
                position.x -= distance;
                break;
        }
        lines.push_back(line(origin, position, totallength));
        totallength += distance;
    }
}

optional<int> findintersection(vector<line> &wire1, vector<line> &wire2) {
    auto shortestlength = optional<int>();
    for(auto const &seg1: wire1) {
        for(auto const &seg2: wire2) {
            point intersection;
            int length;
            if(seg1.intersects(seg2, intersection, length) && !intersection.is_origin()) {
                cout << "Intersection at " << intersection.x << "," <<intersection.y << endl;
                if(shortestlength.has_value()) {
                    shortestlength = min(shortestlength.value(), length);
                }
                else {
                    shortestlength = length;
                }
            }
        }
    }
    return shortestlength;
}

int main(int argc, char *argv[]) {
    auto file = ifstream("aoc3_easy.txt");
    string input1, input2;
    getline(file, input1);
    getline(file, input2);

    vector<line> wire1, wire2;
    parselines(wire1, input1);
    parselines(wire2, input2);

    auto steps = findintersection(wire1, wire2);
    if(steps.has_value()) {
        cout << "Minimum steps " << steps.value() << endl;
    } else {
        cout << "No intersection" << endl;
    }
    cout << "Done!" << endl;
    return 0;
}