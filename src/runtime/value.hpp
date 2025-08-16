#ifndef VALUE_HPP
#define VALUE_HPP

#include <string>
#include <vector>

#include "util/util.hpp"

enum class ValueType { NONE = 0, BOOL, NUMBER, STRING, LIST };

struct LiteralValue {
    LiteralValue() = default;
    virtual ~LiteralValue() = default;
    ValueType type;
};

struct NoneValue : public LiteralValue {
    NoneValue() {
        type = ValueType::NONE;
    }
};

struct BoolValue : public LiteralValue {
    BoolValue(bool value) : value(value) {
        type = ValueType::BOOL;
    }

    bool value;
};

struct NumValue : public LiteralValue {
    NumValue(double value) : value(value) {
        type = ValueType::NUMBER;
    }

    double value;
};

struct StringValue : public LiteralValue {
    StringValue(const std::string &value) : value(value) {
        type = ValueType::STRING;
    }

    std::string value;
};

struct ListValue : public LiteralValue {
    ListValue(const std::vector<LiteralValue *> &literals) : value(literals) {}
    std::vector<LiteralValue *> value;
};

inline std::string literal_to_string(LiteralValue &value) {
    switch (value.type) {
        case ValueType::NONE:
            return "None";
        case ValueType::BOOL: {
            auto &v = static_cast<BoolValue &>(value);
            return v.value ? "true" : "false";
        }
        case ValueType::NUMBER: {
            auto &v = static_cast<NumValue &>(value);
            return std::to_string(v.value);
        }
        case ValueType::STRING: {
            auto &v = static_cast<StringValue &>(value);
            return v.value;
        }
    }
    UNIMPLEMENTED();
    return "";
}

//
// struct ListValue : public RuntimeValue {
//     std::vector<RuntimeValue &> value;
// };

#endif // VALUE_HPP
