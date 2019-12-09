#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <functional>

using namespace std;

class Object {
public:
    Object(string name, string parent): name(name), parent(parent) {

    }
    string name;
    string parent;
};

int depth(shared_ptr<Object> obj, map<string,shared_ptr<Object>> &objects) {
    if(obj->parent == "COM") {
        return 1;
    }
    return 1 + depth(objects[obj->parent], objects);
}

int find_in_path(shared_ptr<Object> source, map<string,shared_ptr<Object>> &objects, string name) {
    if(source->parent == name) {
        return 0;
    }
    if(source->parent == "COM") {
        return -1;
    }
    int depth = find_in_path(objects[source->parent], objects, name);
    return depth == -1 ? -1 : depth + 1;
}

int main(int argc, char *argv[]) {
    auto objectmap = map<string,shared_ptr<Object>>();
    auto orbitmap = ifstream("aoc6.txt");
    string orbit;
    while(getline(orbitmap, orbit)) {
        auto object = make_shared<Object>(orbit.substr(4), orbit.substr(0, 3));
        objectmap[object->name] = object;
    }

    auto you = objectmap["YOU"];
    auto san = objectmap["SAN"];

    auto obj = you;
    int mydepth = 0;
    while(obj->parent != "COM") {
        auto depth = find_in_path(san, objectmap, obj->parent);
        if(depth != -1) {
            cout << "Found common parent " << obj->parent << " @ total depth " << depth + mydepth << endl;
            break;
        }
        obj = objectmap[obj->parent];
        mydepth++;
    }

    cout << "Done!" << endl;
    return 0;
};
