#include <fmt/core.h>
#include <raylib.h>

#include <iostream>
#include <string>
#include <vector>

#include "lexer.hpp"
#include "parser.hpp"
#include "runtime/interpreter.hpp"
#include "token.hpp"
#include "util/error.hpp"
#include "util/file.hpp"

int main(int argc, char **argv) {
    if (argc != 2) {
        fmt::println("choco <file-name>");
        return 0;
    }

    // if there's an error anywhere we fall back because the interpreter cannot
    // continue at this point
    try {
        std::string source = load_file(argv[1]);
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

        // generate the AST
        Parser parser{tokens};
        const std::vector<uptr<Statement>> &ast = parser.parse();

        // run the interpreter
        Interpreter choco;
        choco.eval(ast);

    } catch (const Error &error) {
        return error.output_error();
    }
    return 0;
}
