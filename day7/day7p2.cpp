#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>

std::vector<int> getInts() {
    std::vector<int> ints;
    std::string line;

    std::ifstream input_file("day7.txt");
    if (!input_file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return {};
    }

    while (getline(input_file, line, ',')) {
        ints.push_back(std::stoi(line));
    }

    input_file.close();
    return ints;
}

__int128_t f(__int128_t n){
    __int128_t res = 0;
    for (__int128_t i = 1; i <= n; i++) {
        res += i;
    }
    return res;
}

std::ostream&
operator<<( std::ostream& dest, __int128_t value )
{
    std::ostream::sentry s( dest );
    if ( s ) {
        __uint128_t tmp = value < 0 ? -value : value;
        char buffer[ 128 ];
        char* d = std::end( buffer );
        do
        {
            -- d;
            *d = "0123456789"[ tmp % 10 ];
            tmp /= 10;
        } while ( tmp != 0 );
        if ( value < 0 ) {
            -- d;
            *d = '-';
        }
        int len = std::end( buffer ) - d;
        if ( dest.rdbuf()->sputn( d, len ) != len ) {
            dest.setstate( std::ios_base::badbit );
        }
    }
    return dest;
}

int main(int argc, char** argv) {
    // part 2: distance is factorial
    std::vector<int> ints = getInts();

    __int128_t minDistance = std::numeric_limits<__int128_t>::max();
    // unsigned long long distance = 0;
    for (int pos = *std::min_element(ints.begin(), ints.end()); pos < *std::max_element(ints.begin(), ints.end()); pos++) {
        __int128_t distance = 0; // forgot to initialize before... :)
        for (int i : ints) {
            distance += f(abs(i - pos));
        }
        if (distance < minDistance) {
            minDistance = distance;
        }
    }
    std::cout << minDistance << std::endl;

    return 0;
}