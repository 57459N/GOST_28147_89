#include <iostream>
#include <fstream>
#include <cstdint>
#include <array>
#include "bitset"

class Kzu {
    std::array<uint32_t, 8> data{};

public:
    Kzu(const char *filepath) {
        read_kzu(filepath);
    }

    uint32_t operator[](size_t idx) const {
        return data[idx];
    }

private:
    void read_kzu(const std::string &filepath) {
        std::ifstream fkzu(filepath, std::ios::binary);
        if (fkzu.is_open()) {
            for (int i = 0; i < 8; ++i) {
                fkzu.read((char *) (&data[i]), sizeof(data[i]));
                std::bitset<32> a(data[i]);
                std::cout << a << " : " << (int) data[i] << "\n";
            }

        } else {
            std::cout << "Didn't read Kzu";
            throw;
        }
        fkzu.close();
    }
};

class Key {
    std::array<uint64_t, 8> data{};
public:
    Key(const char *filepath) {
        read_key(filepath);
    }

    uint8_t operator[](size_t i, size_t block) const { // returns block with length of 4 bits
        return (data[i] >> (block << 2)) & 0b1111;
    }

private:
    void read_key(const char *filepath) {
        std::ifstream fkey("../k.bin", std::ios::binary);

        if (fkey.is_open()) {
            for (int i = 0; i < 8; ++i) {
                fkey.read((char *) (&data[i]), sizeof(data[i]));
//                std::bitset<64> a(data[i]);
//                std::cout << a << " : " << (int) data[i] << "\n";
            }

        } else {
            std::cout << "K didn't open!\n";
        }
        fkey.close();
    }
};

void
inner_cycle(uint32_t &N1, uint32_t &N2, const Kzu &kzu, const Key &k, bool verbose = false, bool reversed = false) {
    for (int i = reversed ? 7 : 0; reversed ? i >= 0 : i < 8; reversed ? --i : ++i) {
        // Xi mod sum 32 N1
        uint32_t cm1 = kzu[i] + N1;

        // table transformation
        uint32_t r1 = 0;
        for (int j = 7; j >= 0; --j) {
            size_t block = (cm1 >> (j * 4)) & 0b1111;
            r1 = (r1 << 4) + k[j, block];
        }

        // left roll 11
        uint32_t r2 = (r1 << 11) + (r1 >> (32 - 11));


        // r2 xor n2
        uint32_t cm2 = r2 ^ N2;

        if (verbose) {
            std::cout << "KZU : " << std::bitset<32>(kzu[i]) << "\n";
            std::cout << "CM1 : " << std::bitset<32>(cm1) << "\n";
            std::cout << "R1 : " << std::bitset<32>(r1) << "\n";
            std::cout << "R2 : " << std::bitset<32>(r2) << "\n";
            std::cout << "CM2 : " << std::bitset<32>(cm2) << "\n\n";
            std::cout.flush();
        }

        N2 = N1;
        N1 = cm2;
    }
}

void encrypt(uint32_t &N1, uint32_t &N2, const Kzu &kzu, const Key &k, bool verbose = false, size_t cycles = 3) {
    for (int z = 0; z < cycles; ++z) {
        inner_cycle(N1, N2, kzu, k, verbose);
    }
    inner_cycle(N1, N2, kzu, k, verbose, true);
}

void decrypt(uint32_t &N1, uint32_t &N2, const Kzu &kzu, const Key &k, bool verbose = false, size_t cycles = 3) {
    inner_cycle(N1, N2, kzu, k, verbose);
    for (int z = 0; z < cycles; ++z) {
        inner_cycle(N1, N2, kzu, k, verbose, true);
    }
}


int main() {
    Kzu kzu("../kzu.bin");
    Key k("../k.bin");


    std::ifstream file("../encoded.bin", std::ios::binary);
    std::ofstream of("../decoded.bin", std::ios::binary);

    uint32_t N1;
    file.read((char *) (&N1), sizeof(N1));
    uint32_t N2;
    file.read((char *) (&N2), sizeof(N2));

    decrypt(N1, N2, kzu, k, false);

    of.write((char *) (&N2), sizeof(N2));
    of.write((char *) (&N1), sizeof(N1));

    of.close();
    file.close();

    return 0;
}

