#pragma once

#include <array>
#include <functional>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "Expression.hpp"
#include "Value.hpp"

namespace cppshell {
namespace detail {
class DevNull : public std::ostream {
public:
    DevNull() : std::ostream(&os) {}

private:
    class DevNullBuffer : public std::streambuf {
    public:
        int overflow(int c) { return c; }
    };
    DevNullBuffer os;
};
} // namespace detail
static inline detail::DevNull devNull{};

template<typename ExpressionType = DefaultExpression, char FrameEnd = '\n', size_t InputBufferSize = 256u,
         typename InputStream = std::istream, typename OutputStream = std::ostream, typename PromptStream = std::ostream>
class CommandInterpreter : ExpressionType {
public:
    using Command = typename ExpressionType::CommandType;
    using Args = typename ExpressionType::ArgsContainer;
    using HelpString = std::string;
    using Callback = std::function<void(Args, OutputStream&)>;
    using CommandValue = std::pair<Callback, HelpString>;

    CommandInterpreter(InputStream& is, OutputStream& os, PromptStream& tty = devNull, std::string_view ps1 = defaultPs1)
        : is(is), os(os), tty(tty), ps1(ps1) {
        registerCommand(
          "help",
          [this](const auto& args, auto& os) {
              if (args.empty()) {
                  os << "Available functions:\n";
                  for (const auto& command : commands) {
                      os << std::left << std::setw(10) << command.first;
                      os << command.second.second << '\n';
                  }
              }
              else {
                  if (auto c = getCallback(*args.begin()))
                      if (c->second.empty())
                          os << "no help available";
                      else
                          os << c->second;
                  else
                      os << "not a command";
                  os << '\n';
              }
          },
          "this help message");
    }

    void run(bool stopOnError = false) {
        std::array<typename InputStream::char_type, inputBufferSize> buffer{};

        while (1) {
            promptString();
            is.getline(buffer.data(), buffer.size(), frameEnd);
            if (is.eof() || is.bad())
                return;
            else if (is.fail()) {
                is.clear();
                is.ignore(std::numeric_limits<std::streamsize>::max(), frameEnd);
            }
            else if (buffer[0] != '\0') {
                auto expression = ExpressionType(buffer.data());
                try {
                    invoke(expression.command(), expression.args());
                }
                catch (const std::exception& e) {
                    if (stopOnError)
                        throw;
                    std::cerr << e.what() << std::endl;
                }
            }
        }
    }

    void registerCommand(const Command& command, const Callback& callback, HelpString helpString = "") {
        commands[command] = std::make_pair(callback, helpString);
    }

    void setPromptString(std::string promptString) { ps1 = promptString; }

private:
    static constexpr unsigned int inputBufferSize = InputBufferSize;
    static constexpr const char* defaultPs1 = ">>> ";
    static constexpr char frameEnd = FrameEnd;

    InputStream& is;
    OutputStream& os;
    PromptStream& tty;
    std::string ps1;
    std::unordered_map<Command, CommandValue> commands;

    void invoke(const Command& command, const Args& args) const {
        if (auto c = getCallback(command))
            c->first(args, os);
        else {
            os << "unknown command: " << command << '\n';
            invoke("help");
        }
    }

    void invoke(const Command& command) const {
        invoke(command, {});
    }

    /**
     * @brief Get the optional callback associated to a command
     * 
     * @param command command to get the callback from
     * @return std::optional<CommandValue> 
     */
    std::optional<CommandValue> getCallback(const Command& command) const {
        auto resolvedCommand = commands.find(command);
        if (resolvedCommand != commands.end())
            if (resolvedCommand->second.first)
                return resolvedCommand->second;

        return {};
    }

    void promptString() const { tty << ps1; };
};
} // namespace cppshell
