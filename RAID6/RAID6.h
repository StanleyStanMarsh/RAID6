#pragma once
#include "../Disk/Disk.h"


const int NUM_OF_DISKS = 5;

class RAID6 {
    std::vector<Disk> disks_array;
};
