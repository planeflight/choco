#include <iostream>
#include <string>
#include <vector>

#include "lexer.hpp"
#include "parser.hpp"
#include "runtime/interpreter.hpp"

int main() {
    std::string source;
    // source = "print(\"Hey\"); if 9 > 5 { let a = s * 5.5 + 10; }";
    // source =
    //     "let a = \"This is a string\";print(a);let b = true;print(b); let c =
    //     "
    //     "- 2.4; print(c);";
    source = "let a = (5+3)*2; let b = -5;print(a + b);";
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

    std::cout << "PARSING\n\n";
    // generate the AST
    Parser parser{tokens};

    std::cout << "Evaluating\n\n";

    Interpreter choco;

    const std::vector<uptr<Statement>> &ast = parser.parse();

    std::cout << "choco >\n";
    for (const auto &s : ast) {
        choco.evaluate(s.get());
    }
    return 0;
}
