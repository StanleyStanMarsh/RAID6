#include <iostream>
#include <fstream>
#include "menu/menu.h"

int main() {
    for (int i = 0; i < 5; i++) {
        std::ofstream out("disk" + std::to_string(i) + ".txt");
        for (int j = 0; j < 64; j++)
            out << 'z' << std::endl;
        out.close();
    }

    menu();

    return 0;
}
