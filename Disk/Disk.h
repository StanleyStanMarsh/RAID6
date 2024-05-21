#pragma once
#include <string>
#include <vector>
#include <fstream>


const short DISK_CAPACITY = 64;
const short DATA_SIZE = 5;

struct ByteVec {
    std::vector<unsigned int> data;
    ByteVec(const std::string &str_data);
    ByteVec(const ByteVec &other);
    int convert(const char &c);
    std::string to_string();
};

class Disk {
    friend struct ByteVec;
    std::string disk_name;
    std::vector<ByteVec> data_array;
public:
    Disk(const std::string &name);
    bool write(const short &address, const std::string &data);
    std::string read(const short &address);


private:
    void ReadFromFile();
    void WriteToFile();
};
