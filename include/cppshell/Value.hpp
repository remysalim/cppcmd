#pragma once
#include <string>

namespace cppshell {
namespace values {

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

class ValueString {
public:
    explicit ValueString(std::string& str) : text(str) {}

    template<typename T>
    T as() const {
        return ::cppshell::values::as<T>(text);
    }
    const std::string& operator()() const { return text; }

    operator std::string() const { return text; }
    friend std::ostream& operator<<(std::ostream& out, const ValueString& value) { return out << value(); }

private:
    std::string text;
};

} // namespace values
} // namespace cppshell