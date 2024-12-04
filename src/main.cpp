#include <iostream>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <fstream>
#include <filesystem>

using namespace std;

std::string WORKING_DIR = std::filesystem::current_path().string();

std::vector<std::string> split_string(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string current_token;
    bool in_single_quote = false;
    bool in_double_quote = false;

    for (char c : s) {
        if (c == '\'' && !in_double_quote) {
            // Toggle single quote mode
            in_single_quote = !in_single_quote;
            
            // Only add quote delimiter if not first/last character
            if (!current_token.empty() && in_single_quote) {
                current_token += c;
            }
        }
        else if (c == delimiter && !in_single_quote && !in_double_quote) {
            // Split only when not in quotes
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
        }
        else {
            // Always add character when in quotes or not a delimiter
            current_token += c;
        }
    }

    // Add last token
    if (!current_token.empty()) {
        tokens.push_back(current_token);
    }

    // Remove quotes from tokens
    for (auto &token : tokens) {
        if (!token.empty() && token.front() == '\'' && token.back() == '\'') {
            token = token.substr(1, token.length() - 2);
        }
    }

    return tokens;
}

void handleCd(const std::string& argument) {
    std::filesystem::path new_path;

    if (argument == "~") {
        // Change to the user's home directory
        const char* home = std::getenv("HOME");
        if (home) {
            new_path = home;
        } else {
            std::cout << "HOME environment variable is not set.\n";
            return;
        }
    } else {
        // Handle absolute and relative paths
        new_path = argument[0] == '/' ? argument : WORKING_DIR + '/' + argument;
    }

    if (std::filesystem::exists(new_path) && std::filesystem::is_directory(new_path)) {
        std::filesystem::current_path(new_path);
        WORKING_DIR = std::filesystem::current_path().string(); // Update the working directory variable
    } else {
        std::cout << argument << ": No such file or directory\n";
    }
}


void handle_type_command(const std::vector<std::string> &args, const std::vector<std::string> &path) {
    if (args[1] == "echo" || args[1] == "exit" || args[1] == "type" || args[1] == "pwd") {
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
        } else if (args[0] == "pwd") {
            std::cout << WORKING_DIR << "\n"; // Use WORKING_DIR to print the current directory
        } else if (args[0] == "cd" && args.size() > 1) {
            handleCd(args[1]);
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