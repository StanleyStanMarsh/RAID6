#include "menu.h"
#include <set>

const std::set<char> hex_digits = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

std::string help_text = "Use:\n"
                        "'<address> write <data (14 bytes)>' to write data\n"
                        "or\n"
                        "'<address> read' to read data\n";

// 00 write abc123cd3ef128
// 00 write 00000142634000
void menu() {
    std::cout << "RAID6 implementation application\n"
    << "Type 'help' for help\n";
    RAID6 raid_arr;
    std::string input;
    while (true) {
        std::getline(std::cin, input);

        auto command = split(input, ' ');

        if (command.size() < 2 || command.size() > 3) {
            if (command[0] == "help") {
                std::cout << help_text;
                continue;
            }
            std::cerr << "Invalid command\n" << std::endl;
            std::cout << help_text;
            continue;
        }

        short addr;
        try {
            addr = std::stoi(RemoveLeadingZeros(command[0]));
        } catch(std::invalid_argument const& ex) {
            std::cerr << "Invalid address. Use only numbers" << std::endl;
            continue;
        } catch(std::out_of_range const& ex) {
            std::cerr << "Invalid address. Address can't be so big (address must be in range 0 - 63)" << std::endl;
            continue;
        }

        if (addr < 0 || addr >= DISK_CAPACITY) {
            std::cerr << "Invalid address. Address can't be so big (address must be in range 0 - 63)" << std::endl;
            continue;
        }

        std::string command_name = command[1];
        std::string data;
        if (command.size() == 3) {
            data = command[2];
            if (data.size() != 14) {
                std::cerr << "Invalid data. Data must be 14 bytes long" << std::endl;
                continue;
            }
//            if (RemoveLeadingZeros(data)!= data) {
//                std::cerr << "Invalid data. Data must be in hexadecimal format without leading zeros" << std::endl;
//                continue;
//            }
            bool ok = true;
            for (auto ch: data) {
                if (hex_digits.count(ch) == 0) {
                    std::cerr << "Invalid data. Data must be in hexadecimal format" << std::endl;
                    ok = false;
                    break;
                }
            }
            if (!ok) {
                continue;
            }
        }
        if (command_name == "write") {
            if (raid_arr.write(addr, data)) std::cout << "Data has been written\n";
            else std::clog << "Data has not been written\n";
        }
        else if (command_name == "read") {
            error_read e;
            std::string result = raid_arr.read(addr, e);
            switch (e) {
                case error_read::NoError:
                    std::cout << "Data: " << result << "\n";
                    break;
                case error_read::DataEmpty:
                    std::cerr << "Data is empty\n";
                    break;
                case error_read::ServiceValue:
                    std::cerr << "Service value\n";
                    break;
                case error_read::DamagedTooManyDisks:
                    std::cerr << "Unable to read: damaged too many disks\n";
                    break;
            }
        }
        else {
            std::cerr << "Invalid command\n" << std::endl;
            std::cout << help_text;
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
