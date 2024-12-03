#include <iostream>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <fstream>
#include <filesystem>
using namespace std;
std::vector<std::string> split_string(const std::string &s, char delimiter) {
    std::stringstream ss(s);
    std::vector<std::string> tokens;
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void handle_type_command(const std::vector<std::string> &args, const std::vector<std::string> &path) {
    if (args[1] == "echo" || args[1] == "exit" || args[1] == "type") {
        std::cout << args[1] << " is a shell builtin\n";
        return;
    }
    for (const auto &dir : path) {
        std::string filepath = dir + '/' + args[1];
        if (std::ifstream(filepath).good()) {
            std::cout << args[1] << " is " << filepath << "\n";
            return;
        }
    }
    std::cout << args[1] << ": not found\n";
}

int main() {
    std::cout << std::unitbuf; // Enable automatic flushing
    std::string path_string = std::getenv("PATH");
    std::vector<std::string> path = split_string(path_string, ':');

    while (true) {
        std::cout << "$ ";
        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) continue;

        std::vector<std::string> args = split_string(input, ' ');

        if (args[0] == "exit" && args.size() > 1 && args[1] == "0") break;

        if (args[0] == "echo") {
            for (size_t i = 1; i < args.size(); ++i) {
                std::cout << args[i] << (i == args.size() - 1 ? "\n" : " ");
            }
        } else if(command == "pwd"){
            std::string cwd = std::filesystem::current_path();
            std::string print_cwd = cwd.substr(0, cwd.length());
            std::cout << print_cwd << "\n";
        } else if (args[0] == "type" && args.size() > 1) {
            handle_type_command(args, path);
        } else {
            for (const auto &dir : path) {
                std::string filepath = dir + '/' + args[0];
                if (std::ifstream(filepath).good()) {
                    std::string command = filepath + input.substr(args[0].length());
                    std::system(command.c_str());
                    goto next_command;
                }
            }
            std::cout << args[0] << ": command not found\n";
        }
    next_command:;
    }

    return 0;
}
