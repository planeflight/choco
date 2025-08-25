#include <fmt/base.h>
#include <gtest/gtest.h>

#include <string>

#include "lexer.hpp"
#include "parser.hpp"
#include "runtime/interpreter.hpp"
#include "util/file.hpp"

// Far from the most useful and thorough tests
class InterpreterTest : public testing::Test {
  public:
    InterpreterTest() : interpreter() {}

  protected:
    std::vector<Token> tokenize(Lexer *lexer) {
        lexer->retokenize();
        // create the list of tokens
        std::vector<Token> tokens;
        Token token = lexer->next();
        while (token.type != TokenType::END) {
            tokens.push_back(token);
            token = lexer->next();
        }
        tokens.push_back(token); // add the end token
        return tokens;
    }
    std::string run_and_capture(const std::string &filename) {
        testing::internal::CaptureStdout();
        Lexer lexer(load_file(filename));
        const auto &tokens = tokenize(&lexer);
        Parser parser(tokens);
        interpreter.eval(parser.parse());
        return testing::internal::GetCapturedStdout();
    }

    Interpreter interpreter;
};

// WARN: basically useless, but comment out whatever file(s) to test 1 by 1.
// This was the only way I could get GTest to not throw a fatal error about
// multiple stdout capturers.
TEST_F(InterpreterTest, CheckArrayStrings) {
    EXPECT_EQ(load_file("./tests/out/array_strings.output"),
              run_and_capture("./tests/cases/array_strings.choco"));
    EXPECT_EQ(load_file("./tests/out/expression.output"),
              run_and_capture("./tests/cases/expression.choco"));
    EXPECT_EQ(load_file("./tests/out/loop_func_struct.output"),
              run_and_capture("./tests/cases/loop_func_struct.choco"));
}
