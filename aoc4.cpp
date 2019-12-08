#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <optional>

using namespace std;


int main(int argc, char *argv[]) {
    //264793-803935
    int matches = 0;
    for(int p = 264793;p<=803935;p++) {
        string s = to_string(p);
        // two adjacent
        if(!((s[0] == s[1] && s[1] != s[2])
            || (s[1] == s[2] && s[0] != s[1] && s[3] != s[1])
            || (s[2] == s[3] && s[1] != s[2] && s[4] != s[2])
            || (s[3] == s[4] && s[2] != s[3] && s[5] != s[3])
            || (s[4] == s[5] && s[3] != s[4])
            )) {
            continue;
        }
        if(s[5] < s[4] || s[4] < s[3] || s[3] < s[2] || s[2] < s[1] || s[1] < s[0]) {
            continue;
        }
        matches++;
    }

    cout << matches << endl;
    cout << "Done!" << endl;
    return 0;
}