#ifndef PARSER_HPP
#define PARSER_HPP

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <vector>

#include "token.hpp"
#include "util.hpp"

struct Node {
    virtual std::string to_string() const {
        // UNIMPLEMENTED();
    }
};

struct NumberNode : public Node {
    NumberNode(Token *t) : token(t) {}
    std::string to_string() const override {
        return token->to_string();
    }

    Token *token;
};

struct BinaryOpNode : public Node {
    BinaryOpNode(Node l, Token *o, Node r) : left(l), op(o), right(r) {}
    std::string to_string() const override {
        return left.to_string() + " " + op->to_string() + right.to_string();
    }
    Node left;
    Token *op;
    Node right;
};

struct Parser {
    Parser(const std::vector<Token> &tokens) : i(0), tokens(tokens) {
        current_token = &this->tokens[i];
        peek_token = &this->tokens[i + 1];
    }

    auto parse() {
        program();
        return 1;
    }

    bool check_current(TokenType type) {
        return current_token->type == type;
    }

    bool check_peek(TokenType type) {
        return peek_token->type == type;
    }

    void match(TokenType type) {
        if (!check_current(type)) {
            std::cerr << "Expected " << type_to_string(type) << ", got "
                      << type_to_string(current_token->type) << std::endl;
            exit(EXIT_FAILURE);
        }
        next_token();
    }

    void next_token() {
        i++;
        current_token = &tokens[i];
        peek_token = nullptr;
        if (i < tokens.size() - 1) {
            peek_token = &tokens[i + 1];
        }
    }

    void program() {
        std::cout << "PROGRAM\n";

        // go through program
        while (!check_current(TokenType::END)) {
            statement();
        }
        std::cout << "Parsing Completed\n";
    }

    void expr() {
        std::cout << "EXPRESSION\n";

        term();
        // can have 0+ +/- and expressions
        while (check_current(TokenType::PLUS) ||
               check_current(TokenType::MINUS)) {
            next_token();
            term();
        }
    }

    // term: unary {( / | * ) unary}
    void term() {
        std::cout << "TERM\n";
        unary();
        while (check_current(TokenType::MUL) || check_current(TokenType::DIV)) {
            next_token();
            unary();
        }
    }

    void unary() {
        std::cout << "UNARY\n";

        // optional +/-
        if (check_current(TokenType::PLUS) || check_current(TokenType::MINUS)) {
            next_token();
        }
        primary();
    }

    void primary() {
        std::cout << "PRIMARY (" << current_token->text << ")\n";
        if (check_current(TokenType::NUMBER)) {
            next_token();
        } else if (check_current(TokenType::SYMBOL)) {
            next_token();
        } else {
            std::cerr << "Unexpected token at " << current_token->text;
            exit(EXIT_FAILURE);
        }
    }

    void comparison() {
        const auto is_comp_op = [&]() -> bool {
            return check_current(TokenType::LT) ||
                   check_current(TokenType::GT) ||
                   check_current(TokenType::EQUALS) ||
                   check_current(TokenType::NOT_EQUAL) ||
                   check_current(TokenType::GOT) ||
                   check_current(TokenType::LOT);
        };
        // find the first expression
        expr();
        if (is_comp_op()) {
            next_token();
            expr();
        } else {
            std::cerr << "Expected comparison operator at : "
                      << current_token->text << std::endl;
            std::exit(EXIT_FAILURE);
        }
        // find the second expression
        while (is_comp_op()) {
            next_token();
            expr();
        }
    }

    void statement() {
        // print (expression | string)
        if (check_current(TokenType::PRINT)) {
            std::cout << "STATEMENT: PRINT\n";
            next_token();
            if (check_current(TokenType::OPEN_PAREN)) {
                next_token();
                if (check_current(TokenType::STRING)) {
                    next_token();
                } else {
                    // expression
                    expr();
                }
                match(TokenType::CLOSE_PAREN);
            }
        } // IF
        else if (check_current(TokenType::IF)) {
            std::cout << "STATEMENT: IF\n";
            next_token();
            comparison();

            match(TokenType::OPEN_CURLY);
            while (!check_current(TokenType::CLOSE_CURLY)) {
                statement();
            }

            match(TokenType::CLOSE_CURLY);
        } // WHILE
        else if (check_current(TokenType::WHILE)) {
            std::cout << "STATEMENT: WHILE\n";
            next_token();
            comparison();

            match(TokenType::OPEN_CURLY);
            while (!check_current(TokenType::CLOSE_CURLY)) {
                statement();
            }
            match(TokenType::CLOSE_CURLY);
        }
        // LET
        else if (check_current(TokenType::LET)) {
            std::cout << "STATEMENT: LET\n";
            next_token();
            match(TokenType::SYMBOL);
            match(TokenType::ASSIGNMENT);
            expr();
        } else {
            std::cerr << "Invalid statement at ";
            UNIMPLEMENTED();
        }

        // check end line
        match(TokenType::SEMICOLON);
        std::cout << "END OF STATEMENT\n";
    }

    size_t i = 0;
    std::vector<Token> tokens;
    size_t tok_idx = 1;
    Token *current_token = nullptr, *peek_token = nullptr;
};

#endif // PARSER_HPP
