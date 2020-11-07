#include <bitset>
#include <iomanip>
#include <iostream>
#include <vector>

#include <cppcmd/CommandInterpreter.hpp>

using namespace std;
using namespace cppcmd;
using namespace cppcmd::values;

template<typename T, typename Os>
void tryAs(const values::ValueString& value, Os& os) {
    try {
        os << boolalpha << showbase << left;
        os << "  as " << setw(16) << typeid(T).name() << "[ " << value.as<T>() << " ]\n";
    }
    catch (const parse_error& e) {
        os << e.what() << " ]\n";
    }
}

int main() {
    auto interpreter = CommandInterpreter(cin, cout, cout);

    interpreter.registerCommand(
      "add",
      [](const auto& args, auto& os) {
          int result{0};
          for (const auto& value : args) result += as<int>(value);
          os << result << endl;
      },
      "adds integers (e.g: 'add 1 -5 0xab')");

    interpreter.registerCommand(
      "led",
      [](const auto& args, auto& os) {
          static bitset<3> ledState{};
          if (args.empty())
              os << "Led status: " << ledState.to_string() << endl;
          else if (args.size() == 2)
              ledState[as<size_t>(args[0])] = as<uint8_t>(args[1]);
      },
      "usage: led <index> <0|1>");

    interpreter.registerCommand(
      "parse",
      [](const auto& args, auto& os) {
          for (const auto& arg : args) {
              os << "parsing: '" << arg << "'\n";
              tryAs<int>(arg, os);
              tryAs<bool>(arg, os);
              tryAs<uint8_t>(arg, os);
              tryAs<uint32_t>(arg, os);
#ifdef CPPCMD_HAS_STREAM_OPERATORS
              tryAs<double>(arg, os);
              tryAs<float>(arg, os);
#endif
          }
      },
      "parse arguments to several types");

    interpreter.registerCommand(
      "ps", [&](const auto& args, auto&) { interpreter.setPromptString(*args.begin()); }, "change prompt string");

    interpreter.run();
    return 0;
}
