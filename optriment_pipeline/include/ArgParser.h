#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace optriment {
class ArgParser {
   public:
    ArgParser(int argc, char** argv) {
        // Tokenize the input arguments and store them in the vector
        for (int i = 0; i < argc; ++i) {
            tokens.push_back(std::string(argv[i]));
        }
        arguments = argv;
    }

    // Function to check if an argument exists in the tokens
    bool hasOption(const std::string& option) const {
        for (const auto& token : tokens) {
            if (token == option) {
                return true;
            }
        }
        return false;
    }

    char* getOptionValue(const std::string& option) const {
        for (size_t i = 0; i < tokens.size(); ++i) {
            if (tokens[i] == option && i + 1 < tokens.size()) {
                return arguments[i + 1];
            }
        }
        return nullptr;
    }

    // Function to get all positional arguments (arguments without flags)
    std::vector<std::string> getPositionalArgs() const {
        std::vector<std::string> positionalArgs;
        for (const auto& token : tokens) {
            if (token.front() != '-') {
                positionalArgs.push_back(token);
            }
        }
        return positionalArgs;
    }

   private:
    std::vector<std::string> tokens;
    // This class will store the input arguments and supposes this pointer will be valid until end of program.
    char** arguments;
};
}  // namespace optriment