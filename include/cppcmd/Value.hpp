#pragma once
#include <string>

namespace cppcmd {
namespace values {
namespace detail {
template<typename T>
struct dependent_false : std::false_type {};
} // namespace detail

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
#ifdef CPPCMD_HAS_STREAM_OPERATORS
        return stringstream_parser<T>(text);
#else
        static_assert(detail::dependent_false<T>::value, "don't know how to parse");
#endif
}

class ValueString : public std::string {
public:
    explicit ValueString(std::string& str) : std::string(str) {}
    explicit ValueString(std::string&& str) : ValueString(str) {}

    template<typename T>
    T as() const {
        return ::cppcmd::values::as<T>(*this);
    }
    const std::string& operator()() const { return *this; }
};

} // namespace values
} // namespace cppcmd
