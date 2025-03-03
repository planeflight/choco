#include <iostream>
#include <string>
#include <vector>

#include "src/lexer.hpp"
#include "src/parser.hpp"

int main() {
    std::string source;
    source = "print(\"Hey\"); if 9 > 5 { let a = s * 5.5 + 10; }";
    // tokenize
    Lexer lexer{source};
    lexer.retokenize();
    // create the list of tokens
    std::vector<Token> tokens;
    Token token = lexer.next();
    while (token.type != TokenType::END) {
        tokens.push_back(token);
        std::cout << token.to_string() << "\n";
        token = lexer.next();
    }
    tokens.push_back(token); // add the end token

    // generate the AST
    Parser parser{tokens};
    auto ast = parser.parse();
    return 0;
}
