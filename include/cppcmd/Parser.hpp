#include <charconv>
#include <exception>
#include <regex>
#include <string_view>

namespace cppcmd {
namespace detail {
std::basic_regex<char> integer_pattern("(-)?(0x)?([0-9a-zA-Z]+)|((0x)?0)");
std::basic_regex<char> truthy_pattern("(t|T)(rue)?|1");
std::basic_regex<char> falsy_pattern("(f|F)(alse)?|0");
} // namespace detail

class parse_error : public std::exception {
public:
    parse_error() : message(errorString) {}
    explicit parse_error(const std::string& errorMessage) {
        std::stringstream errorStream;
        errorStream << errorString << ": " << errorMessage;
        message = errorStream.str();
    }
    const char* what() const noexcept override { return message.c_str(); }

private:
    static constexpr std::string_view errorString = "failed to parse";
    std::string message;
};

template<typename T>
T integer_parser(const std::string& text) {
    T value;
    std::smatch match;
    std::regex_match(text, match, detail::integer_pattern);

    if (match.length() == 0)
        throw parse_error("integer regex mismatch");

    const uint8_t base = match.length(2) > 0 ? 16 : 10;
    std::string valueString = match[1].str().append(match[3].str());

    auto [p, errorCode] = std::from_chars(valueString.data(), valueString.data() + valueString.size(), value, base);
    if (errorCode != std::errc()) {
        if (errorCode == std::errc::result_out_of_range)
            throw parse_error("out of range");
        throw parse_error();
    }

    return value;
}

#ifdef CPPCMD_HAS_STREAM_OPERATORS
template<typename T>
T stringstream_parser(const std::string& text) {
    T value;
    std::stringstream in(text);
    in >> value;
    if (!in)
        throw parse_error();
    return value;
}
#endif

bool bool_parser(const std::string& text) {
    std::smatch result;
    std::regex_match(text, result, detail::truthy_pattern);

    if (!result.empty())
        return true;

    std::regex_match(text, result, detail::falsy_pattern);
    if (!result.empty()) {
        return false;
    }

    throw parse_error("bool regex mismatch");
}
} // namespace cppcmd
