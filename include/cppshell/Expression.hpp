#pragma once

#include <algorithm>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

#include "Parser.hpp"
#include "Value.hpp"

namespace cppshell {

template<typename T, typename K, char Separator = ' '>
class Expression {
public:
    using CommandType = T;
    using ArgsContainer = K;

public:
    Expression() = default;

    template<typename Line>
    Expression(Line line) {
        std::istringstream stream(line);
        std::string token{};

        // trim leading separators input
        while (!stream.eof() && _command.empty())
            std::getline(stream, _command, separator);

        while (std::getline(stream, token, separator))
            if (!token.empty() && token[0] != separator)
                _args.emplace_back(token);
    }

    const CommandType& command() const { return _command; }
    const ArgsContainer& args() const { return _args; }
    operator bool() const { return !_command.empty(); }

private:
    static constexpr char separator = Separator;
    CommandType _command;
    ArgsContainer _args;
};

using DefaultExpression = Expression<std::string, std::vector<values::ValueString>, ' '>;
} // namespace cppshell