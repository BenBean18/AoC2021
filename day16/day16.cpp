#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <map>
#include <cmath>
#include <chrono>
#include <thread>

void delay(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

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
        toReturn += lookupTable[c];
    }
    return toReturn;
}

struct Packet {
    std::string _data;
    unsigned int version : 3; // 3 bits
    unsigned int type : 3; // 3 bits
    int bitLength = 6;
    Packet() {

    }
    void assignData(std::string data, bool isBin = false) {
        // std::cout << "Assigning data";
        if (!isBin) {
            this->_data = hexToBin(data);
        } else {
            this->_data = data;
        }
        this->version = std::stoi(_data.substr(0, 3), nullptr, 2);
        this->type = std::stoi(_data.substr(3, 3), nullptr, 2);
        // std::cout << " Data assigned, type = " << this->type << ", version = " << this->version << std::endl;
    }
    virtual std::tuple<Packet*, int, int> getPacket(); // returns packet, # of hex bits used, and type
    int getLengthInHexBits() {
        return ceil((double)bitLength / 4.);
    }
    int getLengthInBits() {
        return bitLength;
    }
};

struct LiteralValuePacket : Packet {
    unsigned int version : 3; // 3 bits
    unsigned int type : 3; // 3 bits
    unsigned long long literalValue;
    LiteralValuePacket() {

    }
    LiteralValuePacket(Packet *packet) {
        this->_data = packet->_data;
        this->version = packet->version;
        this->type = packet->type;
        this->bitLength = packet->bitLength;
        std::cout << this->version << "+";
        // std::cout << "version:" << this->version;

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
    int getLengthInBits() {
        return bitLength;
    }
};

struct OperatorPacket : Packet {
    unsigned int version : 3; // 3 bits
    unsigned int type : 3; // 3 bits
    unsigned int lengthTypeID : 1; // 0 = 15 bits, 1 = 11 bits
    std::vector<Packet*> subPackets;
    OperatorPacket() {

    }
    OperatorPacket(Packet *packet) {
        this->_data = packet->_data;
        this->version = packet->version;
        this->type = packet->type;
        this->bitLength = packet->bitLength;
        this->lengthTypeID = std::stoi(_data.substr(6, 1), nullptr, 2);
        std::cout << this->version << "+";
        unsigned long totalLength;
        unsigned long totalSubpacketLength;
        std::string newData;
        switch (this->lengthTypeID) {
            case 0:
                // this is the length of bits of the sub packets
                totalSubpacketLength = std::stoi(_data.substr(7, 15), nullptr, 2);
                // std::cout << "Total subpacket length is " << totalSubpacketLength << std::endl;
                totalLength = 22;
                newData = _data.substr(totalLength);
                while (totalLength < totalSubpacketLength + 22) {
                    Packet packet;
                    packet.assignData(newData, true); // true means is binary
                    auto actualPacket = packet.getPacket();
                    Packet *packetPtr;
                    int length;
                    int type;
                    std::tie(packetPtr, length, type) = actualPacket;
                    // std::cout << packet.version << "+"; // this shows version...?
                    // Tuple isn't returning data
                    // std::cout << "version is " << packetPtr->version;
                    subPackets.push_back(packetPtr);
                    totalLength += length;
                    newData = _data.substr(totalLength);
                }
                this->bitLength = totalLength;
                break;
            case 1:
                // this is the number of subpackets (11 bits)
                unsigned long subpacketNum = std::stoi(_data.substr(7, 11), nullptr, 2);
                // std::cout << "Total subpacket number is " << subpacketNum << std::endl;
                totalLength = 18;
                newData = _data.substr(totalLength);
                for (int _ = 0; _ < subpacketNum; _++) {
                    // std::cout << "length: " << totalLength << " " << newData << std::endl;
                    Packet packet;
                    packet.assignData(newData, true); // true means is binary
                    auto actualPacket = packet.getPacket();
                    // std::cout << packet.version << "+"; // this shows version...?
                    Packet *packetPtr;
                    int length;
                    int type;
                    std::tie(packetPtr, length, type) = actualPacket;
                    // std::cout << "version is " << packetPtr->version;
                    subPackets.push_back(packetPtr);
                    totalLength += length;
                    newData = _data.substr(totalLength);
                    // std::cout << std::endl << "Packet length was " << length << std::endl;
                }
                this->bitLength = totalLength;
                break;
        }
    }
};

std::tuple<Packet*, int, int> Packet::getPacket() {
    Packet *packet;
    unsigned int type = this->type;
    switch (type) {
        case 4:
            packet = new LiteralValuePacket(this);
            // std::cout << "packet version is " << this->version << std::endl;
            return {packet, ((LiteralValuePacket*)packet)->getLengthInBits(), type};
            break;
        default:
            packet = new OperatorPacket(this);
            return {packet, ((OperatorPacket*)packet)->getLengthInBits(), type};
            break;
    }
}

// https://stackoverflow.com/questions/307765/how-do-i-check-if-an-objects-type-is-a-particular-subclass-in-c
template <class DstType, class SrcType>
bool isType(const SrcType* src) {
  return dynamic_cast<const DstType*>(src) != nullptr;
}

int main(int argc, char** argv) {
    // Insert code here! Use snippets -- "strs" = strings, "ints" = ints, "oth" = other
    Packet packet;
    packet.assignData("A0016C880162017C3686B18A3D4780");
    auto actualPacket = packet.getPacket();
    Packet *packetPtr;
    int length;
    int type;
    std::tie(packetPtr, length, type) = actualPacket;
    if (type == 4) {
        std::cout << "lvp with value ";
        std::cout << ((LiteralValuePacket*)packetPtr)->literalValue << std::endl;
    } else {
        for (Packet* p : ((OperatorPacket*)packetPtr)->subPackets) {
            // std::cout << p->version;
        }
    }
    
    return 0;
}