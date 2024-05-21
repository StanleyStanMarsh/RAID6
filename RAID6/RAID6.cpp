#include <sstream>
#include "RAID6.h"


RAID6::RAID6() {
    for (int i = 0; i < NUM_OF_DISKS; i++) {
        disks_array.emplace_back(Disk("disk" + std::to_string(i)));
    }
}

bool RAID6::write(const short &address, const std::string &data) {
    if (address < 0 || address >= DISK_CAPACITY) return false;
    if (data.size() != FULL_DATA_SIZE) return false;

    for (int i = 0; i < NUM_OF_DISKS; i++) {
        if (disks_array[i].check() == state::Failure) return false;
    }

    std::vector<std::string> data_str;
    std::string tmp;
    for (int i = 0; i < FULL_DATA_SIZE; i++) {
        if (i % 5 == 0 && i != 0) {
            data_str.push_back(tmp);
            tmp = "";
        }
        tmp += data[i];
    }

    data_str.push_back(tmp);

    auto extra_disks = EXTRA_ALLOC[address % NUM_OF_DISKS];

    int j = 0;
    for (int i = 0; i < NUM_OF_DISKS; i++) {
        if (std::find(extra_disks.begin(), extra_disks.end(), i) == extra_disks.end()) {
            disks_array[i].write(address, data_str[j++]);
        }
    }

    bool f = false;
    for (auto &disk : extra_disks) {
        std::stringstream stream;
        if (!f) {
            stream << std::hex << PolySum1(data_str);
            disks_array[disk].write(address, stream.str());
            f = true;
        }
        else {
            stream << std::hex << PolySum2(data_str);
            disks_array[disk].write(address, stream.str());
        }
    }

    return true;
}

int RAID6::PolySum1(std::vector<std::string> &data_str) {
    int res = 0;
    for (int i = 0; i < data_str.size(); i++) {
        res += std::stoi(data_str[i], 0, 16);
    }
    return res;
}

int RAID6::PolySum2(std::vector<std::string> &data_str) {
    int res = 0;
    int j = 1;
    for (int i = 0; i < data_str.size(); i++) {
        res += (j++) * std::stoi(data_str[i], 0, 16);
    }
    return res;
}

std::string RAID6::read(const short &address) {
    if (address < 0 || address >= DISK_CAPACITY) return "error1";

    std::vector<int> broken_disks;
    for (int i = 0; i < NUM_OF_DISKS; i++) {
        if (disks_array[i].check() == state::Failure)
            broken_disks.push_back(i);
    }

    auto extra_disks = EXTRA_ALLOC[address % NUM_OF_DISKS];

    if (broken_disks.empty()) {
        return ReadDirect(address, extra_disks);
    }

    std::vector<int> active_disks;
    for (int i = 0; i < NUM_OF_DISKS; i++) {
        if (std::find(broken_disks.begin(), broken_disks.end(), i) == broken_disks.end()) {
            active_disks.push_back(i);
        }
    }

    std::vector<int> active_data_disks;
    for (auto &disk : active_disks) {
        if (std::find(extra_disks.begin(), extra_disks.end(), disk) == extra_disks.end()) {
            active_data_disks.push_back(disk);
        }
    }

    if (broken_disks.size() == 1) {

        if (std::find(extra_disks.begin(), extra_disks.end(), broken_disks[0]) != extra_disks.end()) {
            return ReadDirect(address, extra_disks);
        }

        return ReadWithOneRecovery(address, extra_disks, broken_disks, active_data_disks, 0);

//        std::string res;

//        for (int i = 0; i < NUM_OF_DISKS; i++) {
//            if (std::find(extra_disks.begin(), extra_disks.end(), i) == extra_disks.end()) {
//
//                if (i == broken_disks[0]) {
//                    int data_int = std::stoi(disks_array[extra_disks[0]].read(address), 0, 16);
//                    for (auto &disk : active_data_disks) {
//                        data_int -= std::stoi(disks_array[disk].read(address), 0, 16);
//                    }
//                    std::stringstream stream;
//                    stream << std::hex << data_int;
//                    res += stream.str();
//                }
//
//                else {
//                    res += disks_array[i].read(address);
//                }
//
//            }
//        }

//        return res;
    }

    if (broken_disks.size() == 2) {
        if (std::find(extra_disks.begin(), extra_disks.end(), broken_disks[0]) != extra_disks.end() &&
        std::find(extra_disks.begin(), extra_disks.end(), broken_disks[1])!= extra_disks.end()) {
            return ReadDirect(address, extra_disks);
        }

        else if (std::find(extra_disks.begin(), extra_disks.end(), broken_disks[0]) != extra_disks.end()) {
            return ReadWithOneRecovery(address, extra_disks, broken_disks, active_data_disks, 1);
        }

        else if (std::find(extra_disks.begin(), extra_disks.end(), broken_disks[1]) != extra_disks.end()) {
            return ReadWithOneRecovery(address, extra_disks, broken_disks, active_data_disks, 0);
        }

        return ReadWithTwoRecovery(address, extra_disks, active_data_disks);
    }

    return std::string{"error2"};
}

std::string RAID6::ReadDirect(const short &address, const std::vector<int> &extra_disks) {
    std::string res;

    for (int i = 0; i < NUM_OF_DISKS; i++) {
        if (std::find(extra_disks.begin(), extra_disks.end(), i) == extra_disks.end()) {
            if (disks_array[i].read(address) == "empty")
                return "empty";

            res += RemoveLeadingZeros(disks_array[i].read(address));
        }
    }

    return res;
}

std::string RAID6::ReadWithOneRecovery(const short &address, const std::vector<int> &extra_disks,
                                     const std::vector<int> &broken_disks, const std::vector<int> &active_data_disks,
                                     const int &target_extra_disk) {
    std::string res;

    for (int i = 0; i < NUM_OF_DISKS; i++) {
        if (std::find(extra_disks.begin(), extra_disks.end(), i) == extra_disks.end()) {

            if (i == broken_disks[0]) {
                int data_int = std::stoi(disks_array[extra_disks[target_extra_disk]].read(address), 0, 16);
                for (auto &disk : active_data_disks) {
                    data_int -= std::stoi(disks_array[disk].read(address), 0, 16);
                }
                std::stringstream stream;
                stream << std::hex << data_int;
                res += stream.str();
            }

            else {
                res += disks_array[i].read(address);
            }

        }
    }

    return res;
}

std::string RAID6::ReadWithTwoRecovery(const short &address, const std::vector<int> &extra_disks,
                                       const std::vector<int> &active_data_disks) {
    std::string res;
    int data_x = std::stoi(disks_array[extra_disks[0]].read(address), 0, 16);
    int data_y = std::stoi(disks_array[extra_disks[1]].read(address), 0, 16);
    int data_target_1 = 0, data_target_2 = 0;
    std::stringstream stream1;
    std::stringstream stream2;
    std::string data_var_str = disks_array[active_data_disks[0]].read(address);
    int data_var = std::stoi(data_var_str, 0, 16);
    switch (active_data_disks[0]) {
        case 0:
            // a
            res += data_var_str;

            data_target_1 = 3 * data_x - 2 * data_var - data_y;
            data_target_2 = (data_y - data_var - 2 * data_target_1) / 3;

            stream1 << std::hex << data_target_1;
            stream2 << std::hex << data_target_2;

            // b
            res += stream1.str();
            // c
            res += stream2.str();

            return res;
        case 1:
            data_target_2 = (data_y - data_var - data_x) / 2;
            data_target_1 = data_x - data_var - data_target_2;

            stream1 << std::hex << data_target_1;
            stream2 << std::hex << data_target_2;

            // a
            res += stream1.str();
            // b
            res += data_var_str;
            // c
            res += stream2.str();

            return res;
        case 2:
            data_target_2 = data_y - 2 * data_var - data_x;
            data_target_1 = data_x - data_var - data_target_2;

            stream1 << std::hex << data_target_1;
            stream2 << std::hex << data_target_2;

            // a
            res += stream1.str();
            // b
            res += stream2.str();
            // c
            res += data_var_str;

            return res;
    }
}

std::string RemoveLeadingZeros(const std::string& str) {
    // Находим первый символ, который не является '0'
    size_t start_index = str.find_first_not_of('0');

    // Если строка не содержит других символов, кроме '0', возвращаем пустую строку
    if (start_index == std::string::npos) {
        return "0";
    }

    // Возвращаем подстроку, начиная с первого не нулевого символа
    return str.substr(start_index);
}