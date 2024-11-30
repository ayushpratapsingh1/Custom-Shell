#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <cstdlib>

using namespace std;

std::string get_path(std::string command) {
    // Get PATH environment variable
    const char* path_env = std::getenv("PATH");
    if (!path_env) {
        return "";  // Return empty if PATH is not set
    }

    std::stringstream ss(path_env);
    std::string path;
    while (std::getline(ss, path, ':')) {
        // Check for the existence of the command in the path directories
        std::string abs_path = path + "/" + command;
        if (std::filesystem::exists(abs_path) && std::filesystem::is_regular_file(abs_path)) {
            return abs_path;
        }
    }
    return "";
}

int main() {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    while (true) {
        // Prompt user for input
        std::cout << "$ ";
        std::string input;
        std::getline(std::cin, input);

        if (input.substr(0, 4) == "echo") {
            if (input.length() > 5 && input[4] == ' ')
                std::cout << input.substr(5) << "\n";
            else
                std::cout << "\n";
            continue;
        }

        if (input.substr(0, 4) == "type") {
            if (input.length() > 5 && input[4] == ' ') {
                std::string cmd = input.substr(5);
                // Check if the command is a shell builtin
                if (cmd == "echo" || cmd == "type" || cmd == "exit") {
                    std::cout << cmd << " is a shell builtin\n";
                } else {
                    // Check if the command exists in PATH
                    std::string path = get_path(cmd);
                    if (path.empty()) {
                        std::cout << cmd << ": not found\n";
                    } else {
                        std::cout << cmd << " is " << path << "\n";
                    }
                }
            } else {
                std::cout << ": not found\n";  // Handle the case with no command after 'type'
            }
            continue;
        }

        if (input == "exit 0") {
            exit(0);  // Exit the program
        }

        std::cout << input << ": command not found\n";  // Handle other unknown commands
    }
}
