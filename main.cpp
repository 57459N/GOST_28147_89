#include <iostream>
#include <fstream>
#include <cstdint>
#include <array>
#include "bitset"

class KZU {
    std::array<uint32_t, 8> kzu{};

public:
    KZU(const char *filepath) {
        read_kzu(filepath);
    }

    uint32_t operator[](size_t idx) {
        return kzu[idx];
    }

private:
    void read_kzu(const std::string &filepath) {
        std::ifstream fkzu(filepath, std::ios::binary);
        if (fkzu.is_open()) {
            for (int i = 0; i < 8; ++i) {

                fkzu.read((char *) (&kzu[i]), sizeof(kzu[i]));
//            std::bitset<32> a(kzu[i]);
//            std::cout <<  a << " : " << (int)kzu[i] << "\n";
            }

        } else {
            std::cout << "Didn't read KZU";
            throw;
        }
        fkzu.close();
    }
};

class KEY {
    std::array<uint64_t, 8> key{};
public:
    KEY(const char *filepath) {
        read_key(filepath);
    }

    uint8_t operator[](size_t i, size_t block) { // returns block with length of 4 bits
        return (key[i] >> (block << 2)) & 0b1111;
    }

private:
    void read_key(const char *filepath) {
        std::ifstream fkey("../k.bin", std::ios::binary);

        if (fkey.is_open()) {
            for (int i = 0; i < 8; ++i) {
                fkey.read((char *) (&key[i]), sizeof(key[i]));
//                std::bitset<64> a(key[i]);
//                std::cout << a << " : " << (int) key[i] << "\n";
            }

        } else {
            std::cout << "K didn't open!\n";
        }
    }
};

std::array<uint32_t, 8> read_kzu(const std::string &filepath) {
    std::ifstream fkzu(filepath, std::ios::binary);
    std::array<uint32_t, 8> kzu{};
    if (fkzu.is_open()) {
        for (int i = 0; i < 8; ++i) {

            fkzu.read((char *) (&kzu[i]), sizeof(kzu[i]));
//            std::bitset<32> a(kzu[i]);
//            std::cout <<  a << " : " << (int)kzu[i] << "\n";
        }

    } else {
        std::cout << "KZU didn't open!\n";
    }
    fkzu.close();
    return kzu;
}


int main() {
    KZU kzu("../kzu.bin");
    KEY k("../k.bin");

    // std::bitset<4> a(k[ki, block]);
    // std::cout << a << "\n";




    return 0;
}

