#include <iostream>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <fstream>
#include <filesystem>

using namespace std;

std::string WORKING_DIR = std::filesystem::current_path().string();

std::vector<std::string> split_string(const std::string &s) {
    std::vector<std::string> tokens;
    std::string token;
    bool in_single_quotes = false;
    bool in_double_quotes = false;

    for (size_t i = 0; i < s.length(); ++i) {
        char c = s[i];

        if (c == '\'' && !in_double_quotes) {
            in_single_quotes = !in_single_quotes; // Toggle single quotes
            if (!in_single_quotes) {
                tokens.push_back(token);
                token.clear();
            }
            continue;
        }

        if (c == '\"' && !in_single_quotes) {
            in_double_quotes = !in_double_quotes; // Toggle double quotes
            if (!in_double_quotes) {
                tokens.push_back(token);
                token.clear();
            }
            continue;
        }

        if (c == '\\') {
            if (in_double_quotes) {
                // If in double quotes, escape the next character
                if (i + 1 < s.length()) {
                    token += s[++i]; // Add the escaped character
                }
            } else {
                // If outside quotes, treat backslash as a regular character
                token += c;
            }
            continue;
        }

        if (c == ' ' && !in_single_quotes && !in_double_quotes) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += c; // Add character to the current token
        }
    }

    // Add the last token if it exists
    if (!token.empty()) {
        tokens.push_back(token);
    }

    return tokens;
}

void handleCd(const std::string& argument) {
    std::filesystem::path new_path;

    if (argument == "~") {
        const char* home = std::getenv("HOME");
        if (home) {
            new_path = home;
        } else {
            std::cout << "HOME environment variable is not set.\n";
            return;
        }
    } else {
        new_path = argument[0] == '/' ? argument : WORKING_DIR + '/' + argument;
    }

    if (std::filesystem::exists(new_path) && std::filesystem::is_directory(new_path)) {
        std::filesystem::current_path(new_path);
        WORKING_DIR = std::filesystem::current_path().string();
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
    std::vector<std::string> path = split_string(path_string);

    while (true) {
        std::cout << "$ ";
        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) continue;

        std::vector<std::string> args = split_string(input);

        if (args.empty()) continue;

        if (args[0] == "exit" && args.size() >  1 && args[1] == "0") break;

        if (args[0] == "echo") {
            for (size_t i = 1; i < args.size(); ++i) {
                std::cout << args[i] << (i == args.size() - 1 ? "\n" : " ");
            }
        } else if (args[0] == "pwd") {
            std::cout << WORKING_DIR << "\n";
        } else if (args[0] == "cd" && args.size() > 1) {
            handleCd(args[1]);
        } else if (args[0] == "type" && args.size() > 1) {
            handle_type_command(args, path);
        } else {
            std::string command = args[0];
            for (size_t i = 1; i < args.size(); ++i) {
                command += " " + args[i]; // Rebuild the command with arguments
            }
            int result = std::system(command.c_str()); // Execute the command
            if (result == -1) {
                std::cout << command << ": command not found\n";
            } else if (result != 0) {
                std::cout << command << ": execution failed with error code " << result << "\n";
            }
        }
    }

    return 0;
}