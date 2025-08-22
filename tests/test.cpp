#include <fmt/os.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "lexer.hpp"
#include "parser.hpp"
#include "runtime/interpreter.hpp"

std::string source1 = "(-5 + -3) - 5 * -2";
std::string source2 = "8 * 3";
std::string source3 = "-2 + 3";

/*class InterpreterTest : public testing::Test {
  public:
    InterpreterTest() {
        lexer1 = std::make_unique<Lexer>(source1);
        lexer2 = std::make_unique<Lexer>(source2);
        lexer3 = std::make_unique<Lexer>(source3);
        lexer1->retokenize();
        lexer2->retokenize();
        lexer3->retokenize();

        parser1 = std::make_unique<Lexer>(tokenize(lexer1.get()));
        parser2 = std::make_unique<Lexer>(tokenize(lexer2.get()));
        parser3 = std::make_unique<Lexer>(tokenize(lexer3.get()));
    }

  private:
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

    std::unique_ptr<Lexer> lexer1, lexer2, lexer3;
    std::unique_ptr<Parser> parser1, parser2, parser3;
    Interpreter interpreter;
};*/

// TEST_F(InterpreterTest, CheckExpressionEvaluation) {
//     fmt::ostream out = fmt::output_file("tests/output/out.txt");
//     for (int ln = 0; ln < 4; ++ln) {
//         out.print("line {}\n", ln);
//     }
// }
