#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

enum class State {
    Normal,
    Single,
    Double
};

std::vector<std::string> parse(const std::string& s) {
    std::vector<std::string> arguments;
    std::string current_argument;
    bool in_escape = false;
    State state = State::Normal;

    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];

        switch (state) {
            case State::Normal:
                if (!in_escape && c == '\'') {
                    state = State::Single;
                } else if (!in_escape && c == '"') {
                    state = State::Double;
                } else if (!in_escape && c == '\\') {
                    in_escape = true;
                } else if (!in_escape && c == ' ') {
                    if (!current_argument.empty()) {
                        arguments.push_back(current_argument);
                        current_argument.clear();
                    }
                } else {
                    current_argument += c;
                }
                break;

            case State::Single:
                if (!in_escape && c == '\'') {
                    state = State::Normal;
                } else if (!in_escape && c == '\\') {
                    in_escape = true;
                } else {
                    current_argument += c;
                }
                break;

            case State::Double:
                if (!in_escape && c == '"') {
                    state = State::Normal;
                } else if (!in_escape && c == '\\') {
                    in_escape = true;
                } else {
                    current_argument += c;
                }
                break;
        }

        if (in_escape) {
            // The next character is treated as a literal character
            current_argument += c;
            in_escape = false;
        }
    }

    if (!current_argument.empty()) {
        arguments.push_back(current_argument);
    }

    if (in_escape) {
        throw std::invalid_argument("Incomplete escape sequence");
    }

    if (state == State::Single) {
        throw std::invalid_argument("Un-terminated single quotation");
    }

    if (state == State::Double) {
        throw std::invalid_argument("Un-terminated double quotation");
    }

    return arguments;
}

int main() {
    std::string input = "echo \"Hello\\ World\" 'This is\\ a test' \"Another\\ Test\"";
    try {
        std::vector<std::string> result = parse(input);
        for (const auto& arg : result) {
            std::cout << "[" << arg << "]\n";
        }
    } catch (const std::invalid_argument& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }

    return 0;
}
