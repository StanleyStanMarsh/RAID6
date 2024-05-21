#include "menu.h"

// abc123cd3ef128
void menu() {
    RAID6 raid_arr;
    std::string input;
    while (true) {
        std::getline(std::cin, input);
        auto command = split(input, ' ');
        short addr = std::stoi(RemoveLeadingZeros(command[0]));
        std::string command_name = command[1];
        std::string data;
        if (command.size() == 3) data = command[2];
        if (command_name == "write") {
            if (raid_arr.write(addr, data)) std::cout << "Data has been written\n";
            else std::cout << "Data has not been written\n";
        }
        else if (command_name == "read") {
            std::cout << raid_arr.read(addr) << '\n';
        }
    }
}

std::vector<std::string> split(const std::string& str, char delim) {
    std::vector<std::string> tokens;
    std::string currentToken;
    for (auto ch: str) {
        if (ch == delim) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
        } else {
            currentToken += ch;
        }
    }
    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }
    return tokens;
}
