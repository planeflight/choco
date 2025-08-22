#ifndef VALUE_HPP
#define VALUE_HPP

#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "util/util.hpp"

enum class ValueType { NONE = 0, BOOL, NUMBER, STRING, CLASS, LIST, OBJECT };

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

struct ObjectValue : public LiteralValue {
    ObjectValue() {
        type = ValueType::OBJECT;
    }
    std::unordered_map<std::string, LiteralValue *> values;
};

struct ListValue : public LiteralValue {
    ListValue() {
        type = ValueType::LIST;
    }
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
        case ValueType::LIST: {
            auto &v = static_cast<ListValue &>(value);
            std::stringstream ss;
            ss << "[";
            for (int i = 0; i < v.value.size() - 1; ++i) {
                ss << literal_to_string(*v.value[i]) << ", ";
            }
            ss << literal_to_string(*v.value.back()) << "]";
            return ss.str();
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
