#include "parser.hpp"

#include <memory>
#include <stdexcept>
#include <string>

#include "ast.hpp"
#include "runtime/value.hpp"
#include "token.hpp"
#include "util/util.hpp"

Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens) {}

Parser::~Parser() {}

Token *Parser::expect(TokenType type) {
    if (!match(type))
        throw std::runtime_error("Error: expected " + type_to_string(type) +
                                 " but got " + type_to_string(current()->type) +
                                 "\n");
    Token *curr = current();
    advance();
    return curr;
}

bool Parser::match(TokenType type) {
    return current()->type == type;
}

bool Parser::match_peek(TokenType type) {
    return peek()->type == type;
}

const std::vector<uptr<Statement>> &Parser::parse() {
    while (current()->type != TokenType::END) {
        uptr<Statement> s = declaration();
        statements.push_back(std::move(s));
    }
    return statements;
}

Token *Parser::peek() {
    return &tokens[idx + 1];
}

Token *Parser::current() {
    return &tokens[idx];
}

Token *Parser::previous() {
    return &tokens[idx - 1];
}

void Parser::advance() {
    idx++;
}

std::vector<uptr<Statement>> Parser::parse_body() {
    std::vector<uptr<Statement>> statements;
    expect(TokenType::OPEN_CURLY);
    while (!match(TokenType::CLOSE_CURLY)) {
        statements.push_back(declaration());
    }
    if (statements.size() == 0)
        throw std::runtime_error("Missing closing '}' brace");
    advance();
    return statements;
}

uptr<Expr> Parser::declaration() {
    // variable assignment
    if (match(TokenType::LET)) {
        advance();
        return var_declaration();
    }
    // variable reassignment
    if (match(TokenType::SYMBOL) && match_peek(TokenType::ASSIGNMENT)) {
        advance();
        Token *symbol = previous();

        uptr<VariableDeclaration> declaration =
            std::make_unique<VariableDeclaration>();
        declaration->name = symbol->content();
        declaration->type = ASTNodeType::VARIABLE_REASSIGN;

        expect(TokenType::ASSIGNMENT);
        declaration->value = expression();
        expect(TokenType::SEMICOLON);

        return declaration;
    }
    // pure function calls where primary=function_call so guarantee semicolon
    // after simplest form: func(p1, p2, ...);
    if (match(TokenType::SYMBOL) && match_peek(TokenType::OPEN_PAREN)) {
        auto func_call = expression();
        expect(TokenType::SEMICOLON);
        return func_call;
    }
    // defining a function
    if (match(TokenType::FUNCTION)) {
        advance();
        return function_def();
    }
    // return value
    if (match(TokenType::RETURN)) {
        advance();
        auto return_expr = std::make_unique<ReturnExpr>();
        return_expr->content = expression();
        expect(TokenType::SEMICOLON);
        return return_expr;
    }
    // classes
    if (match(TokenType::CLASS)) {
        advance();
        return class_def();
    }
    return statement();
}

uptr<Expr> Parser::statement() {
    if (match(TokenType::IF)) {
        advance();
        expect(TokenType::OPEN_PAREN);

        auto if_stmnt = std::make_unique<IfExpr>();
        if_stmnt->condition = expression();

        expect(TokenType::CLOSE_PAREN);
        if_stmnt->statements = parse_body();
        // elif parsing
        while (match(TokenType::ELIF)) {
            advance();
            if_stmnt->elif_statements.push_back(std::make_unique<ElifExpr>());
            expect(TokenType::OPEN_PAREN);
            if_stmnt->elif_statements.back()->condition = expression();
            expect(TokenType::CLOSE_PAREN);
            if_stmnt->elif_statements.back()->statements = parse_body();
        }

        // else parsing
        if (match(TokenType::ELSE)) {
            advance();
            if_stmnt->else_statements = parse_body();
        }
        return if_stmnt;
    }
    if (match(TokenType::WHILE)) {
        advance();
        expect(TokenType::OPEN_PAREN);
        auto while_stmnt = std::make_unique<WhileExpr>();
        while_stmnt->condition = expression();

        expect(TokenType::CLOSE_PAREN);
        while_stmnt->statements = parse_body();
        return while_stmnt;
    }
    return expression();
}

uptr<Expr> Parser::expression() {
    return or_expr();
}

uptr<Expr> Parser::or_expr() {
    auto left = and_expr();

    while (match(TokenType::OR)) {
        advance();
        auto right = and_expr();
        auto binary_expr = std::make_unique<BinaryExpr>();
        binary_expr->left = std::move(left);
        binary_expr->op = TokenType::OR;
        binary_expr->right = std::move(right);
        left = std::move(binary_expr);
    }
    return left;
}

uptr<Expr> Parser::and_expr() {
    auto left = equality();

    while (match(TokenType::AND)) {
        advance();
        auto right = equality();
        auto binary_expr = std::make_unique<BinaryExpr>();
        binary_expr->left = std::move(left);
        binary_expr->op = TokenType::AND;
        binary_expr->right = std::move(right);
        left = std::move(binary_expr);
    }
    return left;
}

uptr<Expr> Parser::equality() {
    auto comp =
        comparison(); // the recursive descent part if it's not an equality

    while (match(TokenType::NOT_EQUAL) || match(TokenType::EQUALS)) {
        advance();
        Token *op = previous();
        auto right = comparison();
        auto binary_expr = std::make_unique<BinaryExpr>();
        binary_expr->left = std::move(comp);
        binary_expr->op = op->type;
        binary_expr->right = std::move(right);
        comp = std::move(binary_expr);
    }
    return comp;
}

uptr<Expr> Parser::comparison() {
    auto left = term();
    while (match(TokenType::LT) || match(TokenType::GT) ||
           match(TokenType::LOT) || match(TokenType::GOT)) {
        advance();
        Token *op = previous();
        auto right = term();
        auto binary_expr = std::make_unique<BinaryExpr>();
        binary_expr->left = std::move(left);
        binary_expr->op = op->type;
        binary_expr->right = std::move(right);
        left = std::move(binary_expr);
    }
    return left;
}

uptr<Expr> Parser::term() {
    auto left = factor();
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        advance();
        Token *op = previous();
        auto right = factor();
        auto binary_expr = std::make_unique<BinaryExpr>();
        binary_expr->left = std::move(left);
        binary_expr->op = op->type;
        binary_expr->right = std::move(right);
        left = std::move(binary_expr);
    }
    return left;
}

uptr<Expr> Parser::factor() {
    auto left = unary();
    while (match(TokenType::MUL) || match(TokenType::DIV)) {
        advance();
        Token *op = previous();
        auto right = unary();
        auto binary_expr = std::make_unique<BinaryExpr>();
        binary_expr->left = std::move(left);
        binary_expr->op = op->type;
        binary_expr->right = std::move(right);
        left = std::move(binary_expr);
    }
    return left;
}

uptr<Expr> Parser::unary() {
    // (! | - ) unary | primary
    while (match(TokenType::NOT) || match(TokenType::MINUS)) {
        advance();
        Token *op = previous();
        auto unary_expr = std::make_unique<UnaryExpr>();
        unary_expr->op = op->type;
        unary_expr->unary = unary(); // recursive unary
        return unary_expr;
    }
    // must be call or primary
    return primary();
}

uptr<Expr> Parser::primary() {
    // lowest precendence: "last step in parsing"
    // i.e. literals are already evaluated, parentheses are first to be
    // evaluated
    const std::string &curr_content = current()->content();

    if (match(TokenType::STRING)) {
        uptr<LiteralExpr> expr = std::make_unique<LiteralExpr>();
        // trim the quotes at the beginning and end
        expr->value = std::make_unique<StringValue>(
            curr_content.substr(1, curr_content.size() - 2));
        advance();
        return expr;
    }
    if (match(TokenType::NUMBER)) {
        uptr<LiteralExpr> expr = std::make_unique<LiteralExpr>();
        expr->value = std::make_unique<NumValue>(std::stod(curr_content));
        advance();
        return expr;
    }
    if (match(TokenType::BOOLEAN)) {
        uptr<LiteralExpr> expr = std::make_unique<LiteralExpr>();
        expr->value = std::make_unique<BoolValue>(curr_content == "true");
        advance();
        return expr;
    }
    if (match(TokenType::SYMBOL)) {
        auto symbol = std::make_unique<SymbolExpr>(curr_content);
        advance();
        // function call
        if (match(TokenType::OPEN_PAREN)) {
            advance();
            return finish_call(std::move(symbol));
        }
        return symbol;
    }
    // array
    if (match(TokenType::OPEN_BRACKET)) {
        std::cout << "yeah\n";
        advance();
        auto expr = std::make_unique<ListExpr>();
        auto advance_wrapper = [&]() -> bool {
            advance();
            return true;
        };

        // if there are args
        if (!match(TokenType::CLOSE_BRACKET)) {
            do {
                auto list_elem = expression();
                std::cout << (int)list_elem->type << std::endl;
                expr->elements.push_back(std::move(list_elem));
            } while (match(TokenType::COMMA) && advance_wrapper());
        }
        expect(TokenType::CLOSE_BRACKET);
        return expr;
    }
    // parentheses
    if (match(TokenType::OPEN_PAREN)) {
        advance();
        auto expr = expression();
        expect(TokenType::CLOSE_PAREN);
        return expr;
    }
    std::cout << curr_content << std::endl;
    UNIMPLEMENTED();
}

uptr<VariableDeclaration> Parser::var_declaration() {
    Token *symbol = expect(TokenType::SYMBOL);
    uptr<VariableDeclaration> declaration =
        std::make_unique<VariableDeclaration>();
    declaration->name = symbol->content();
    expect(TokenType::ASSIGNMENT);

    declaration->value = expression();
    expect(TokenType::SEMICOLON);
    return declaration;
}

uptr<Expr> Parser::finish_call(uptr<SymbolExpr> expr) {
    std::vector<uptr<Expr>> args;
    auto advance_wrapper = [&]() -> bool {
        advance();
        return true;
    };

    // if there are args
    if (!match(TokenType::CLOSE_PAREN)) {
        do {
            args.push_back(expression());
        } while (match(TokenType::COMMA) && advance_wrapper());
    }
    expect(TokenType::CLOSE_PAREN);
    auto call = std::make_unique<CallExpr>();
    call->callee = std::move(expr);
    call->params = std::move(args);
    return call;
}

uptr<FunctionDefExpr> Parser::function_def() {
    auto func_def = std::make_unique<FunctionDefExpr>();
    expect(TokenType::SYMBOL);
    func_def->name = previous()->content();
    expect(TokenType::OPEN_PAREN);

    auto advance_wrapper = [&]() -> bool {
        advance();
        return true;
    };

    // if there are args
    if (!match(TokenType::CLOSE_PAREN)) {
        do {
            expect(TokenType::SYMBOL);
            auto param = previous();
            func_def->params.push_back(param->content());
        } while (match(TokenType::COMMA) && advance_wrapper());
    }
    expect(TokenType::CLOSE_PAREN);

    func_def->statements = parse_body();

    return func_def;
}

uptr<Expr> Parser::class_def() {
    auto class_expr = std::make_unique<ClassDefinitionExpr>();
    expect(TokenType::SYMBOL);
    class_expr->name = previous()->content();

    expect(TokenType::OPEN_CURLY);

    while (!match(TokenType::CLOSE_CURLY)) {
        // either member variables
        if (match(TokenType::LET)) {
            advance();
            auto var_decl = var_declaration();
            class_expr->attributes.push_back(std::move(var_decl));
        }
        // or defining a method
        if (match(TokenType::FUNCTION)) {
            advance();
            auto function = function_def();
            class_expr->functions.push_back(std::move(function));
        }
    }
    advance();

    return class_expr;
}
