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
    bool escape_next = false;

    for (size_t i = 0; i < s.length(); ++i) {
        char c = s[i];

        if (in_double_quote && escape_next) {
            // In double quotes, handle escaped characters
            if (c == '\\' || c == '$' || c == '"' || c == '\n') {
                current_token += c;
            } else {
                current_token += '\\';
                current_token += c;
            }
            escape_next = false;
            continue;
        }

        if (c == '\\') {
            // Prepare to escape the next character
            if (in_double_quote) {
                escape_next = true;
            } else if (in_single_quote) {
                // In single quotes, backslash is literal
                current_token += c;
            } else {
                // Outside quotes, backslash escapes next character
                escape_next = true;
            }
            continue;
        }

        if (c == '\'' && !in_double_quote) {
            // Toggle single quote mode
            in_single_quote = !in_single_quote;
            current_token += c;
            continue;
        }
        else if (c == '"' && !in_single_quote) {
            // Toggle double quote mode
            in_double_quote = !in_double_quote;
            if (!in_double_quote) {
                // When closing double quote, don't add the quote
                continue;
            }
            current_token += c;
            continue;
        }
        else if (c == delimiter && !in_single_quote && !in_double_quote) {
            // Split only when not in quotes
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
            continue;
        }
        else {
            // Always add character when in quotes or not a delimiter
            if (escape_next) {
                current_token += c;
                escape_next = false;
            } else {
                current_token += c;
            }
        }
    }

    // Add last token
    if (!current_token.empty()) {
        tokens.push_back(current_token);
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
        // Handle absolute and relative paths, preserving backslashes
        new_path = argument[0] == '/' ? argument : WORKING_DIR + '/' + argument;
    }

    try {
        if (std::filesystem::exists(new_path) && std::filesystem::is_directory(new_path)) {
            std::filesystem::current_path(new_path);
            WORKING_DIR = std::filesystem::current_path().string();
        } else {
            std::cout << argument << ": No such file or directory\n";
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cout << "Error changing directory: " << e.what() << "\n";
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
    std::cout << std::unitbuf;
    std::string path_string = std::getenv("PATH");
    std::vector<std::string> path = split_string(path_string, ':');

    while (true) {
        std::cout << "$ ";
        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) continue;

        std::vector<std::string> args = split_string(input, ' ');
        if (args.empty()) continue;

        if (args[0] == "exit" && args.size() > 1 && args[1] == "0") break;

        if (args[0] == "echo") {
            for (size_t i = 1; i < args.size(); ++i) {
                // Remove surrounding quotes if they exist
                std::string arg = args[i];
                if ((arg.front() == '"' && arg.back() == '"') || 
                    (arg.front() == '\'' && arg.back() == '\'')) {
                    arg = arg.substr(1, arg.length() - 2);
                }
                std::cout << arg << (i == args.size() - 1 ? "\n" : " ");
            }
        } else if (args[0] == "pwd") {
            std::cout << WORKING_DIR << "\n";
        } else if (args[0] == "cd" && args.size() > 1) {
            handleCd(args[1]);
        } else if (args[0] == "type" && args.size() > 1) {
            handle_type_command(args, path);
        } else {
            // Check if first token is a quoted executable
            std::string executable = args[0];
            
            // Remove surrounding quotes from executable
            if ((executable.front() == '"' && executable.back() == '"') || 
                (executable.front() == '\'' && executable.back() == '\'')) {
                executable = executable.substr(1, executable.length() - 2);
            }

            for (const auto &dir : path) {
                std::string filepath = dir + '/' + executable;
                if (std::ifstream(filepath).good()) {
                    // Reconstruct command with original spaces and quotes
                    std::string command = filepath + input.substr(args[0].length());
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