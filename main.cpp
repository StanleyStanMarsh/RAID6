#include <iostream>
#include "Disk/Disk.h"

int main() {
    Disk d0("disk0");
    std::cout << d0.read(3);
    return 0;
}
