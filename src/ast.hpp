#ifndef AST_HPP
#define AST_HPP

#include <vector>

#include "runtime/value.hpp"
#include "token.hpp"
#include "util/util.hpp"

enum class ASTNodeType {
    // statements
    PROGRAM,
    VARIABLE_DECLARATION,
    IF_STATEMENT,
    ELIF_STATEMENT,
    WHILE_STATEMENT,
    FUNCTION_CALL,
    // expressions
    LITERAL,
    BINARY,
    UNARY,
    SYMBOL
};

struct ASTNode {
    ASTNode() = default;
    ASTNodeType type;

    virtual ~ASTNode() = default;
};

using Statement = ASTNode;

struct Expr : public ASTNode {
    Expr() = default;
    virtual ~Expr() = default;
};

// numbers/strings/booleans
struct LiteralExpr : public Expr {
    LiteralExpr() : Expr() {
        type = ASTNodeType::LITERAL;
    }
    uptr<LiteralValue> value = nullptr;
};

struct UnaryExpr : public Expr {
    UnaryExpr() : Expr() {
        type = ASTNodeType::UNARY;
    }
    virtual ~UnaryExpr() {}
    TokenType op;
    // symbol or literal
    uptr<Expr> unary = nullptr;
};

struct SymbolExpr : public Expr {
    SymbolExpr(const std::string &symbol) : Expr(), symbol(symbol) {
        type = ASTNodeType::SYMBOL;
    }
    std::string symbol;
};

struct BinaryExpr : public Expr {
    BinaryExpr() : Expr() {
        type = ASTNodeType::BINARY;
    }
    uptr<Expr> left;
    uptr<Expr> right;
    TokenType op;
};
struct VariableDeclaration : public Expr {
    VariableDeclaration() : Expr() {
        type = ASTNodeType::VARIABLE_DECLARATION;
    }
    std::string name;
    uptr<Expr> value;
};

// children: statements
struct CallExpr : public Expr {
    CallExpr(const std::string &name) : Expr(), name(name) {
        type = ASTNodeType::FUNCTION_CALL;
    }
    std::vector<uptr<Expr>> params;
    std::string name;
};

struct ElifExpr : public Expr {
    ElifExpr() : Expr() {
        type = ASTNodeType::ELIF_STATEMENT;
    }
    uptr<Expr> condition;
    std::vector<uptr<Statement>> statements;
};

struct IfExpr : public Expr {
    IfExpr() : Expr() {
        type = ASTNodeType::IF_STATEMENT;
    }

    uptr<Expr> condition;
    std::vector<uptr<Statement>> statements;
    std::vector<uptr<ElifExpr>> elif_statements;
    std::vector<uptr<Statement>> else_statements;
};

#endif // AST_HPP
