#include <fmt/core.h>

#include <iostream>
#include <string>
#include <vector>

#include "fmt/base.h"
#include "lexer.hpp"
#include "parser.hpp"
#include "runtime/interpreter.hpp"
#include "token.hpp"
#include "util/file.hpp"
#include "util/status.hpp"

int main() {
    StatusOr<std::string> source = load_file("./tests/main.choco");
    if (!source.ok()) {
        source.output_error();
        return source.error();
    }
    // tokenize
    Lexer lexer{*source};
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

    return 0;
}
