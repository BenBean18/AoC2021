#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <map>
#include <cmath>
#include <chrono>
#include <thread>
#include <memory>
#include <algorithm>

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

class Packet {
public:
    std::string _data;
    unsigned int version; // 3 bits
    unsigned int type; // 3 bits
    unsigned long long value;
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
    }
    virtual Packet* getPacket(); // returns packet
    int getLengthInHexBits() {
        return ceil((double)bitLength / 4.);
    }
    int getLengthInBits() {
        return bitLength;
    }
};

class LiteralValuePacket : public Packet {
public:
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
        value = std::stoull(strLiteralValue, nullptr, 2);
    }
    int getLengthInHexBits() {
        return ceil((double)bitLength / 4.);
    }
    int getLengthInBits() {
        return bitLength;
    }
};

class OperatorPacket : public Packet {
public:
    unsigned int lengthTypeID; // 0 = 15 bits, 1 = 11 bits
    std::vector<Packet*> subPackets;
    OperatorPacket(Packet *packet) {
        this->_data = packet->_data;
        this->version = packet->version;
        this->type = packet->type;
        this->bitLength = packet->bitLength;
        this->value = packet->value;
        this->lengthTypeID = std::stoi(_data.substr(6, 1), nullptr, 2);
        unsigned long totalLength;
        unsigned long totalSubpacketLength;
        std::string newData;
        switch (this->lengthTypeID) {
            case 0:
                // this is the length in bits of the sub packets (15 bits)
                totalSubpacketLength = std::stoi(_data.substr(7, 15), nullptr, 2);
                totalLength = 22;
                newData = _data.substr(totalLength);
                while (totalLength < totalSubpacketLength + 22) {
                    Packet subpacket;
                    subpacket.assignData(newData, true); // true means is binary
                    auto packetPtr = subpacket.getPacket();
                    subPackets.push_back(packetPtr);
                    totalLength += packetPtr->bitLength;
                    newData = _data.substr(totalLength);
                }
                this->bitLength = totalLength;
                break;
            case 1:
                // this is the number of subpackets (11 bits)
                unsigned long subpacketNum = std::stoi(_data.substr(7, 11), nullptr, 2);
                totalLength = 18;
                newData = _data.substr(totalLength);
                for (int _ = 0; _ < subpacketNum; _++) {
                    Packet subpacket;
                    subpacket.assignData(newData, true); // true means is binary
                    Packet *packetPtr = subpacket.getPacket();
                    subPackets.push_back(packetPtr);
                    totalLength += packetPtr->bitLength;
                    newData = _data.substr(totalLength);
                }
                this->bitLength = totalLength;
                break;
        }
    }
};

class SumPacket : public OperatorPacket {
public:
    SumPacket(Packet *packet)
    : OperatorPacket(packet) {
        this->value = 0;
        for (Packet *p : this->subPackets) {
            this->value += p->value;
        }
    }
};

class ProductPacket : public OperatorPacket {
public:
    ProductPacket(Packet *packet)
    : OperatorPacket(packet) {
        this->value = 1;
        for (Packet *p : this->subPackets) {
            this->value *= p->value;
        }
    }
};

class MinimumPacket : public OperatorPacket {
public:
    MinimumPacket(Packet *packet)
    : OperatorPacket(packet) {
        this->value = (*std::min_element(this->subPackets.begin(), this->subPackets.end(), [](Packet *a, Packet *b){ return a->value < b->value; }))->value;
    }
};

class MaximumPacket : public OperatorPacket {
public:
    MaximumPacket(Packet *packet)
    : OperatorPacket(packet) {
        this->value = (*std::max_element(this->subPackets.begin(), this->subPackets.end(), [](Packet *a, Packet *b){ return a->value < b->value; }))->value;
    }
};

class GreaterThanPacket : public OperatorPacket {
public:
    GreaterThanPacket(Packet *packet)
    : OperatorPacket(packet) {
        this->value = this->subPackets[0]->value > this->subPackets[1]->value;
    }
};

class LessThanPacket : public OperatorPacket {
public:
    LessThanPacket(Packet *packet)
    : OperatorPacket(packet) {
        this->value = this->subPackets[0]->value < this->subPackets[1]->value;
    }
};

class EqualToPacket : public OperatorPacket {
public:
    EqualToPacket(Packet *packet)
    : OperatorPacket(packet) {
        this->value = this->subPackets[0]->value == this->subPackets[1]->value;
    }
};

Packet* Packet::getPacket() {
    unsigned int type = this->type;
    if (type == 0) {
        SumPacket* packet;
        packet = new SumPacket(this);
        return packet;
    } else if (type == 1) {
        ProductPacket* packet;
        packet = new ProductPacket(this);
        return packet;
    } else if (type == 2) {
        MinimumPacket* packet;
        packet = new MinimumPacket(this);
        return packet;
    } else if (type == 3) {
        MaximumPacket* packet;
        packet = new MaximumPacket(this);
        return packet;
    } else if (type == 4) {
        LiteralValuePacket* packet;
        packet = new LiteralValuePacket(this);
        return packet;
    } else if (type == 5) {
        GreaterThanPacket* packet;
        packet = new GreaterThanPacket(this);
        return packet;
    } else if (type == 6) {
        LessThanPacket* packet;
        packet = new LessThanPacket(this);
        return packet;
    } else if (type == 7) {
        EqualToPacket* packet;
        packet = new EqualToPacket(this);
        return packet;
    } else {
        return nullptr;
    }
}

// https://stackoverflow.com/questions/307765/how-do-i-check-if-an-objects-type-is-a-particular-subclass-in-c
template <class DstType, class SrcType>
bool isType(const SrcType* src) {
  return dynamic_cast<const DstType*>(src) != nullptr;
}

void sumVersions(unsigned long &versionSum, Packet *packetPtr) {
    Packet *currentPacket = packetPtr;
    if (currentPacket->type != 4 && ((OperatorPacket*)currentPacket)->subPackets.size() != 0) {
        for (Packet* p : ((OperatorPacket*)packetPtr)->subPackets) {
            currentPacket = p;
            versionSum += currentPacket->version;
            sumVersions(versionSum, currentPacket);
        }
    }
}

int main(int argc, char** argv) {
    // Insert code here! Use snippets -- "strs" = strings, "ints" = ints, "oth" = other
    Packet packet;
    std::vector<std::string> strings = getStrings();
    packet.assignData(strings[0]);
    auto actualPacket = packet.getPacket();
    Packet *packetPtr = actualPacket;
    unsigned long sum = 0;
    if (packetPtr->type == 4) {
        std::cout << "lvp with value ";
        std::cout << ((LiteralValuePacket*)packetPtr)->value << std::endl;
    } else {
        sum += packetPtr->version;
        sumVersions(sum, packetPtr);
    }
    std::cout << "Part 1: " << sum << std::endl;
    std::cout << "Part 2: " << packetPtr->value;
    
    return 0;
}