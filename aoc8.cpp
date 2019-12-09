#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <functional>
#include <algorithm>

using namespace std;

static const int width = 25;
static const int height = 6;
constexpr static const int pixels_per_layer() {
    return width * height;
};

void read(vector<string> &pixels) {
    auto imagefile = ifstream("aoc8.txt");
    string imageline;
    if(getline(imagefile, imageline)) {
        for(int i=0;i<imageline.length();i++) {
            pixels.push_back(imageline.substr(i, 1));
        }
    }
}

int main(int argc, char *argv[]) {
    auto pixels = vector<string>();
    read(pixels);

    const int layers = pixels.size() / pixels_per_layer();
    int least_zeroes_count = INT_MAX, 
        least_zeroes_layer = -1, 
        least_zeroes_ones = 0, 
        least_zeroes_twos = 0;

    for(int layer=0;layer<layers;layer++) {
        const auto offset = layer * pixels_per_layer();
        int zeroes = 0, ones = 0, twos = 0;
        for(int i=0;i<pixels_per_layer();i++) {
            const auto s = pixels[offset+i];
            zeroes += count(s.begin(), s.end(), '0');
            ones += count(s.begin(), s.end(), '1');
            twos += count(s.begin(), s.end(), '2');
        }
        if(zeroes < least_zeroes_count) {
            least_zeroes_count = zeroes;
            least_zeroes_layer = layer;
            least_zeroes_ones = ones;
            least_zeroes_twos = twos;
        }
    }

    cout << least_zeroes_ones * least_zeroes_twos << endl;

   for(int y=0;y<height;y++) {
        for(int x=0;x<width;x++) {
            for(int layer=0;layer<layers;layer++) {
                const auto pixel = pixels[(layer*pixels_per_layer()) + ((y *width)+x)];
                if(pixel != "2") {
                    cout << (pixel == "0" ? 'X' : ' ');
                    break;
                }
            }
        }
        cout << endl;
    }



    cout << "Done!" << endl;
    return 0;
};
