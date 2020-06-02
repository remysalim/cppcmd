#pragma once
#include <string>

namespace cppshell {
namespace values {
class ValueString {
public:
    explicit ValueString(std::string& str) : text(str) {}

    template<typename T>
    T as() const {
        return ::cppshell::as<T>(text);
    }
    const std::string& operator()() const { return text; }

    operator std::string() const { return text; }
    friend std::ostream& operator<<(std::ostream& out, const ValueString& value) { return out << value(); }

private:
    std::string text;
};

} // namespace values
} // namespace cppshell