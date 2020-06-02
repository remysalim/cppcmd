#pragma once

#include <algorithm>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

#include "Parser.hpp"
#include "Value.hpp"

namespace cppshell {

namespace detail {
template<typename T>
struct dependent_false : std::false_type {};
} // namespace detail

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

template<typename T, typename Text>
static constexpr T as(const Text&& text) {
    return as<T>(text);
}

template<typename T, typename Text>
static constexpr T as(const Text& text) {
    if constexpr (std::is_same_v<std::decay_t<Text>, T>)
        return text;
    else if constexpr (std::is_same_v<bool, T>)
        return bool_parser(text);
    else if constexpr (std::is_integral_v<T>)
        return integer_parser<T>(text);
    else
#ifdef CPPSHELL_HAS_STREAM_OPERATORS
        return stringstream_parser<T>(text);
#else
        static_assert(detail::dependent_false<T>::value, "don't know how to parse");
#endif
}

using DefaultExpression = Expression<std::string, std::vector<values::ValueString>, ' '>;
} // namespace cppshell