#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <map>
#include <cmath>

std::vector<std::string> getStrings() {
    std::vector<std::string> strings;
    std::string line;

    std::ifstream input_file("input.txt");
    if (!input_file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return {};
    }

    while (getline(input_file, line)) {
        strings.push_back(line);
    }

    input_file.close();
    return strings;
}

std::vector<int> getInts() {
    std::vector<int> ints;
    std::string line;

    std::ifstream input_file("input.txt");
    if (!input_file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return {};
    }

    while (getline(input_file, line)) {
        ints.push_back(std::stoi(line));
    }

    input_file.close();
    return ints;
}

std::map<char,std::string> lookupTable = {{'0',"0000"},{'1',"0001"},{'2',"0010"},{'3',"0011"},{'4',"0100"},{'5',"0101"},{'6',"0110"},{'7',"0111"},{'8',"1000"},{'9',"1001"},{'A',"1010"},{'B',"1011"},{'C',"1100"},{'D',"1101"},{'E',"1110"},{'F',"1111"}};

std::string hexToBin(std::string s) {
    std::string toReturn;
    for (char c : s) {
        s += lookupTable[c];
    }
    return toReturn;
}

struct Packet {
    std::string _data;
    unsigned int version : 3;
    unsigned int type : 3;
    int bitLength = 6;
    Packet() {

    }
    void assignData(std::string data) {
        this->_data = hexToBin(data);
        this->version = std::stoi(_data.substr(0, 3), nullptr, 2);
        this->type = std::stoi(_data.substr(3, 3), nullptr, 2);
    }
    virtual std::tuple<Packet*, int> getPacket(); // returns packet and # of hex bits used
};

struct LiteralValuePacket : Packet {
    unsigned int version : 3;
    unsigned int type : 3;
    unsigned long long literalValue;
    LiteralValuePacket() {

    }
    LiteralValuePacket(Packet *packet) {
        this->_data = packet->_data;
        this->version = packet->version;
        this->type = packet->type;
        this->bitLength = packet->bitLength;

        int i = 6;
        bool lastValue = false;
        std::string strLiteralValue;
        while (!lastValue) {
            std::string temp = _data.substr(i, 5);
            bitLength += 5;
            switch (temp[0]) {
                case '1':
                    strLiteralValue += _data.substr(i+1, 4);
                    i += 5;
                    break;
                case '0':
                    strLiteralValue += _data.substr(i+1, 4);
                    lastValue = true;
                    break;
                default:
                    break;
            }
        }
        literalValue = std::stoull(strLiteralValue, nullptr, 2);
    }
    int getLengthInHexBits() {
        return ceil((double)bitLength / 4.);
    }
};

std::tuple<Packet*, int> Packet::getPacket() {
    Packet *packet;
    switch (this->type) {
        case 4:
            packet = new LiteralValuePacket(this);
            return {packet, ((LiteralValuePacket*)packet)->getLengthInHexBits()}; // TODO REPLACE 99999 WITH ACTUAL LENGTH
            break;
        default:
            return {nullptr, 0};
    }
}

int main(int argc, char** argv) {
    // Insert code here! Use snippets -- "strs" = strings, "ints" = ints, "oth" = other
    return 0;
}