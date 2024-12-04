#include <iostream>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <fstream>
#include <filesystem>

using namespace std;

std::string WORKING_DIR = std::filesystem::current_path().string();

enum class State {
    Normal,
    Single,
    Double
};

std::vector<std::string> split_string(const std::string &s) {
    std::vector<std::string> tokens;
    std::string current_token;
    bool in_escape = false;
    State state = State::Normal;

    for (char c : s) {
        switch (state) {
            case State::Normal:
                if (in_escape) {
                    current_token += c; // Add the escaped character
                    in_escape = false;
                } else if (c == '\\') {
                    in_escape = true; // Prepare to escape the next character
                } else if (c == '\'') {
                    state = State::Single; // Enter single quote mode
                } else if (c == '"') {
                    state = State::Double; // Enter double quote mode
                } else if (c == ' ') {
                    if (!current_token.empty()) {
                        tokens.push_back(current_token);
                        current_token.clear();
                    }
                } else {
                    current_token += c; // Add character to the current token
                }
                break;

            case State::Single:
                if (in_escape) {
                    current_token += c; // Add the escaped character
                    in_escape = false;
                } else if (c == '\\') {
                    in_escape = true; // Prepare to escape the next character
                } else if (c == '\'') {
                    state = State::Normal; // Exit single quote mode
                } else {
                    current_token += c; // Add character to the current token
                }
                break;

            case State::Double:
                if (in_escape) {
                    if (c != '$' && c != '`' && c != '"' && c != '\\' && c != '\n') {
                        current_token += '\\'; // Preserve backslash for other escaped characters
                    }
                    current_token += c; // Add the escaped character
                    in_escape = false;
                } else if (c == '\\') {
                    in_escape = true; // Prepare to escape the next character
                } else if (c == '"') {
                    state = State::Normal; // Exit double quote mode
                } else {
                    current_token += c; // Add character to the current token
                }
                break;
        }
    }

    // Add the last token if it exists
    if (!current_token.empty()) {
        tokens.push_back(current_token);
    }

    // Check for unclosed quotes
    if (state == State::Single) {
        throw std::runtime_error("Un-terminated single quotation");
    }
    if (state == State::Double) {
        throw std::runtime_error("Un-terminated double quotation");
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
        return; }
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
    std::cout << std::unitbuf;
    std::string path_string = std::getenv("PATH");
    std::vector<std::string> path = split_string(path_string);

    while (true) {
        std::cout << "$ ";
        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) continue;

        std::vector<std::string> args = split_string(input);

        if (args[0] == "exit" && args.size() > 1 && args[1] == "0") break;

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
            std::string executable = args[0];
            
            for (const auto &dir : path) {
                std::string filepath = dir + '/' + executable;
                if (std::ifstream(filepath).good()) {
                    std::string command = filepath + input.substr(executable.length());
                    std::system(command.c_str());
                    goto next_command;
                }
            }
            std::cout << executable << ": command not found\n";
        }
    next_command:;
    }

    return 0;
}