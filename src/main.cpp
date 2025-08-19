#include <iostream>
#include <string>
#include <vector>

#include "lexer.hpp"
#include "parser.hpp"
#include "runtime/interpreter.hpp"
#include "token.hpp"
#include "util/file.hpp"

int main() {
    std::string source = load_file("./tests/main.choco");
    // std::string source = "if (1 >= 1) { print(\"in if\"); }";
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
    for (const auto &token : tokens) {
        std::cout << type_to_string(token.type) << std::endl;
    }

    std::cout << "PARSING\n\n";
    // generate the AST
    Parser parser{tokens};

    const std::vector<uptr<Statement>> &ast = parser.parse();

    Interpreter choco;
    std::cout << "Evaluating\n\n";
    std::cout << "choco >\n";
    choco.eval(ast);

    return 0;
}
