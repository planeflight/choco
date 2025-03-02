#include <iostream>
#include <string>

#include "src/lexer.hpp"

int main() {
    std::string source;
    source = "int main() {printf(\"Hello, World\");}";
    Lexer lexer(source);
    lexer.retokenize();
    Token token = lexer.next();
    while (token.type != TokenType::END) {
        std::cout << token.to_string() << std::endl;
        token = lexer.next();
    }
    return 0;
}
