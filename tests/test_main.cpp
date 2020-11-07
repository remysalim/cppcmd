#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <list>
#include <sstream>
#include <string>
#include <type_traits>

#include <cppcmd/CommandInterpreter.hpp>
#include <cppcmd/Expression.hpp>
#include <cppcmd/Value.hpp>

using namespace std;
using namespace cppcmd;
using namespace cppcmd::values;

using ExpressionTypes = std::tuple<std::string, const char[], char[]>;
TEMPLATE_LIST_TEST_CASE("Expression parsing", "[parser][expression]", ExpressionTypes) {
    SECTION("With arguments") {
        TestType command{"command arg1 arg2"};
        auto exp = Expression<std::string, std::vector<std::string>>(command);
        REQUIRE(exp);
        REQUIRE(exp.command() == "command");
        REQUIRE(exp.args().size() == 2);
        REQUIRE(exp.args()[0] == "arg1");
        REQUIRE(exp.args()[1] == "arg2");
    }
    SECTION("Without arguments") {
        TestType command{"command"};
        auto exp = Expression<std::string, std::vector<std::string>>(command);
        REQUIRE(exp);
        REQUIRE(exp.command() == "command");
        REQUIRE(exp.args().size() == 0);
    }
    SECTION("Empty input") {
        TestType command{""};
        auto exp = Expression<std::string, std::vector<std::string>>(command);
        REQUIRE_FALSE(exp);
        REQUIRE(exp.command() == "");
        REQUIRE(exp.args().size() == 0);
    }
    SECTION("Value storage") {
        TestType command{"command arg1 true 10 -5"};
        auto exp = Expression<std::string, std::vector<values::ValueString>>(command);
        REQUIRE(exp);
        REQUIRE(exp.args().size() == 4);
        REQUIRE(exp.args()[0].as<string>() == "arg1");
        REQUIRE(exp.args()[1].as<bool>() == true);
        REQUIRE(exp.args()[2].as<uint8_t>() == 10);
        REQUIRE(exp.args()[3].as<int8_t>() == -5);
    }
    SECTION("Trimming") {
        SECTION("Leading") {
            TestType command{"      command arg"};
            auto exp = Expression<std::string, std::vector<std::string>>(command);
            REQUIRE(exp);
            REQUIRE(exp.command() == "command");
            REQUIRE(exp.args().size() == 1);
            REQUIRE(exp.args()[0] == "arg");
        }
        SECTION("Trailing") {
            TestType command{"command arg        "};
            auto exp = Expression<std::string, std::vector<std::string>>(command);
            REQUIRE(exp);
            REQUIRE(exp.command() == "command");
            REQUIRE(exp.args().size() == 1);
            REQUIRE(exp.args()[0] == "arg");
        }
        SECTION("Sparse") {
            TestType command{"   command  arg0 arg1    arg2   "};
            auto exp = Expression<std::string, std::vector<std::string>>(command);
            REQUIRE(exp.command() == "command");
            REQUIRE(exp.args().size() == 3);
            REQUIRE(exp.args()[0] == "arg0");
            REQUIRE(exp.args()[1] == "arg1");
            REQUIRE(exp.args()[2] == "arg2");
        }
    }
}

TEST_CASE("Argument parser", "[parser]") {
    STATIC_REQUIRE(is_same_v<decltype(as<uint16_t>("0xffff")), uint16_t>);

    SECTION("Parsing integers") {
        REQUIRE(as<int>("1") == 1);
        REQUIRE(as<uint8_t>("0xff") == 255);
        REQUIRE(as<int>("-10") == -10);
        REQUIRE(as<int>("-0x0a") == -0x0a);
    }

    SECTION("Parsing booleans") {
        REQUIRE(as<bool>("1") == true);
        REQUIRE(as<bool>("True") == true);
        REQUIRE(as<bool>("true") == true);
        REQUIRE(as<bool>("0") == false);
        REQUIRE(as<bool>("False") == false);
        REQUIRE(as<bool>("false") == false);
        REQUIRE_THROWS(as<bool>("foobar"));
    }

    SECTION("Dealing with edge cases") {
        // out of bound gets thrown
        REQUIRE_THROWS(as<uint8_t>("0xffff"));
        // trying to hold a signed value into an unsigned type throws
        REQUIRE_THROWS(as<unsigned int>("-1"));
        // string as string?
        string dummy{"dummy"};
        REQUIRE(as<string>(dummy) == dummy);
        // invalid string conversion throws
        REQUIRE_THROWS(as<int>("number"));

#ifdef CPPCMD_HAS_STREAM_OPERATORS
        // fallsback to stringstream_parser
        REQUIRE(as<float>("1.2") == 1.2f);
        REQUIRE_THROWS(as<float>("number"));
#else
        WARN("no support for stringstream_parser in this build");
#endif
    }
}

TEST_CASE("Registering commands", "[interpreter]") {
    unsigned int fooCalls = 0;
    unsigned int barCalls = 0;

    stringstream inputStream;
    auto interpreter = CommandInterpreter(inputStream, std::cout);
    interpreter.registerCommand("foo", [&](const auto&, auto&) { fooCalls++; });
    interpreter.registerCommand("bar", [&](const auto&, auto&) { barCalls++; });

    SECTION("Invoking without arguments") {
        inputStream << "foo\nbar\nundefined\nfoo\n";
        interpreter.run();

        REQUIRE(fooCalls == 2);
        REQUIRE(barCalls == 1);
    }

    SECTION("Invoking with arguments") {
        string arg1{}, arg2{};
        interpreter.registerCommand("dummy", [&](const auto& args, auto&) {
            REQUIRE(args.size() == 2);
            arg1 = args[0];
            arg2 = args[1]();
        });
        inputStream << "foo\ndummy lorem ipsum\nbar\nundefined\n";
        interpreter.run();

        REQUIRE(fooCalls == 1);
        REQUIRE(barCalls == 1);
        REQUIRE(arg1 == "lorem");
        REQUIRE(arg2 == "ipsum");
    }
}

TEST_CASE("More CommandInterpreter instances", "[interpreter]") {
    stringstream inputStream, outputStream, promptStream, expectedPrompt;
    SECTION("Default") {
        auto interpreter = CommandInterpreter(inputStream, outputStream);
        decltype(interpreter)::Callback fooFunction = [](const auto& args, auto& os) {
            for (const auto& arg : args)
                os << arg << '-';
        };
        interpreter.registerCommand("foo", fooFunction);
        inputStream << "foo never0gonna\nfoo give you up\n";
        interpreter.run();
        REQUIRE(outputStream.str() == "never0gonna-give-you-up-");
    }

    SECTION("Exotic") {
        using Command = std::string;
        using Args = std::list<std::string>;
        static constexpr char separator = ',';
        static constexpr string_view customPrompt = "$ ";

        auto interpreter =
          CommandInterpreter<Expression<Command, Args, separator>, '0'>(inputStream, outputStream, promptStream, customPrompt);
        decltype(interpreter)::Callback fooFunction = [](const auto& args, auto& os) {
            for (const auto& arg : args)
                os << arg << '-';
        };
        interpreter.registerCommand("foo", fooFunction);
        inputStream << "foo,never gonna0foo,let,you,down0";
        interpreter.run();

        expectedPrompt << customPrompt << customPrompt << customPrompt;
        REQUIRE(promptStream.str() == expectedPrompt.str());
        REQUIRE(outputStream.str() == "never gonna-let-you-down-");
    }
}
