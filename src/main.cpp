#include <fmt/core.h>

#include <iostream>
#include <string>
#include <vector>

#include "fmt/base.h"
#include "lexer.hpp"
#include "parser.hpp"
#include "runtime/interpreter.hpp"
#include "token.hpp"
#include "util/error.hpp"
#include "util/file.hpp"

int main() {
    try {
        std::string source = load_file("./tests/main.choco");
        // tokenize
        Lexer lexer{source};
        lexer.retokenize();
        // create the list of tokens
        std::vector<Token> tokens;
        Token token = lexer.next();
        while (token.type != TokenType::END) {
            tokens.push_back(token);
            token = lexer.next();
        }
        tokens.push_back(token); // add the end token

        fmt::println("PARSING");
        // generate the AST
        Parser parser{tokens};

        const std::vector<uptr<Statement>> &ast = parser.parse();

        Interpreter choco;
        fmt::println("EVALUATING\n>");
        choco.eval(ast);

    } catch (Error error) {
        return error.output_error();
    }

    return 0;
}
