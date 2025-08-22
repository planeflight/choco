#include "ast.hpp"

#include <sstream>

std::string DotExpr::full_expr() const {
    std::stringstream ss;
    auto symbol = static_cast<SymbolExpr *>(head.get());
    ss << symbol->symbol;
    for (auto &symbol : after) {
        ss << "." << static_cast<SymbolExpr *>(symbol.get())->symbol;
    }
    return ss.str();
}
