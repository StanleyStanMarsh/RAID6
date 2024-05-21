#include "Disk.h"

Disk::Disk(const std::string &name) {
    disk_name = name;
    data_array = std::vector<ByteVec>(DISK_CAPACITY,
                                      ByteVec(std::string("z")));
}


bool Disk::write(const short &address, const std::string &data) {
    if (address < 0 || address > 63) return false;

    ReadFromFile();

    std::string data_string = data;

    if (data.size() < DATA_SIZE) {
        while (data_string.size() < DATA_SIZE) {
            data_string = "0" + data_string;
        }
    }

    data_array[address] = ByteVec(data_string);

    WriteToFile();

    return true;
}

void Disk::ReadFromFile() {
    std::ifstream in(disk_name + ".txt");

    std::string line;
    int i = 0;
    while (std::getline(in, line)) {
        data_array[i++] = ByteVec(line);
    }

    in.close();
}

void Disk::WriteToFile() {
    std::ofstream out(disk_name + ".txt");

    for (int i = 0; i < DISK_CAPACITY; i++) {
        if (data_array[i].data[0] == -1) out << "z" << std::endl;
        else out << data_array[i].to_string() << std::endl;
    }

    out.close();
}

std::string Disk::read(const short &address) {
    if (address < 0 || address >= DISK_CAPACITY) return std::string{"error"};

    ReadFromFile();

    if (data_array[address].data[0] == -1) return std::string{"empty"};

    std::string data = data_array[address].to_string();

    if (data.size() != DATA_SIZE) return std::string{"service value"};

    return data;
}

state Disk::check() {
    if (std::ifstream(disk_name + ".txt"))
    {
        return state::Success;
    }
    return state::Failure;
}

ByteVec::ByteVec(const std::string &str_data) {
    if (str_data == "z") {
        data.push_back(-1);
        return;
    }
    for (int i = 0; i < str_data.size(); i++) {
        data.push_back(convert(str_data[i]));
    }
}

int ByteVec::convert(const char &c) {
    int res;

    char lower_c;
    if ('A' <= c && c <= 'F')
        lower_c = c + 32;
    else
        lower_c = c;

    switch (lower_c) {
        case 'a':
            res = 10;
            break;
        case 'b':
            res = 11;
            break;
        case 'c':
            res = 12;
            break;
        case 'd':
            res = 13;
            break;
        case 'e':
            res = 14;
            break;
        case 'f':
            res = 15;
            break;
        default:
            res = lower_c - '0';
            break;
    }

    return res;
}

ByteVec::ByteVec(const ByteVec &other) {
    this->data = other.data;
}

std::string ByteVec::to_string() {
    std::string res;

    for (const auto &c: data) {
        switch (c) {
            case 10:
                res += 'a';
                break;
            case 11:
                res += 'b';
                break;
            case 12:
                res += 'c';
                break;
            case 13:
                res += 'd';
                break;
            case 14:
                res += 'e';
                break;
            case 15:
                res += 'f';
                break;
            default:
                res += (c + '0');
        }
    }

    return res;
}
