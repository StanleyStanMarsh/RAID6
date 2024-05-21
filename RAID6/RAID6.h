#pragma once
#include "../Disk/Disk.h"
#include <algorithm>
#include <iostream>


const int NUM_OF_DISKS = 5;

const short FULL_DATA_SIZE = 14;

const std::vector<std::vector<int>> EXTRA_ALLOC = { {3, 4},
                                                 {2, 3},
                                                 {1, 2},
                                                 {0, 1},
                                                 {0, 4} };

class RAID6 {
    std::vector<Disk> disks_array;

public:
    RAID6();
    bool write(const short &address, const std::string &data);
    std::string read(const short &address);

private:
    int PolySum1(std::vector<std::string> &data_str);
    int PolySum2(std::vector<std::string> &data_str);
    std::string ReadDirect(const short &address, const std::vector<int> &extra_disks);
    std::string ReadWithOneRecovery(const short &address, const std::vector<int> &extra_disks,
                                  const std::vector<int> &broken_disks, const std::vector<int> &active_data_disks,
                                  const int &target_extra_disk);
    std::string ReadWithTwoRecovery(const short &address, const std::vector<int> &extra_disks,
                                    const std::vector<int> &active_data_disks);
};

std::string RemoveLeadingZeros(const std::string& str);