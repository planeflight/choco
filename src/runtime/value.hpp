#ifndef VALUE_HPP
#define VALUE_HPP

#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "util/util.hpp"

enum class ValueType { NONE = 0, BOOL, NUMBER, STRING, CLASS, LIST, OBJECT };

struct LiteralValue {
    LiteralValue() = default;
    virtual ~LiteralValue() = default;
    ValueType type;
    virtual void operator=(LiteralValue &other) = 0;
};

struct NoneValue : public LiteralValue {
    NoneValue() {
        type = ValueType::NONE;
    }
    void operator=(LiteralValue &other) override {}
};

struct BoolValue : public LiteralValue {
    BoolValue(bool value) : value(value) {
        type = ValueType::BOOL;
    }

    void operator=(LiteralValue &other) override {
        if (other.type != ValueType::BOOL) {
            throw std::runtime_error("Cannot assign different types");
        }
        value = static_cast<BoolValue &>(other).value;
    }
    bool value;
};

struct NumValue : public LiteralValue {
    NumValue(double value) : value(value) {
        type = ValueType::NUMBER;
    }
    void operator=(LiteralValue &other) override {
        if (other.type != ValueType::NUMBER) {
            throw std::runtime_error("Cannot assign different types");
        }
        value = static_cast<NumValue &>(other).value;
    }

    double value;
};

struct StringValue : public LiteralValue {
    StringValue(const std::string &value) : value(value) {
        type = ValueType::STRING;
    }

    void operator=(LiteralValue &other) override {
        if (other.type != ValueType::STRING) {
            throw std::runtime_error("Cannot assign different types");
        }
        value = static_cast<StringValue &>(other).value;
    }

    std::string value;
};

struct ObjectValue : public LiteralValue {
    ObjectValue() {
        type = ValueType::OBJECT;
    }
    void operator=(LiteralValue &other) override {
        if (other.type != ValueType::OBJECT) {
            throw std::runtime_error("Cannot assign different types");
        }
        // reset values and copy everything according to copy or reference rules
        values.clear();
        auto obj = static_cast<ObjectValue &>(other);
        for (const auto &[key, val] : obj.values) {
            values[key] = val;
        }
    }

    std::unordered_map<std::string, LiteralValue *> values;
};

struct ListValue : public LiteralValue {
    ListValue() {
        type = ValueType::LIST;
    }
    void operator=(LiteralValue &other) override {
        if (other.type != ValueType::LIST) {
            throw std::runtime_error("Cannot assign different types");
        }
        auto list = static_cast<ListValue &>(other);

        value.clear();
        value.reserve(list.value.size());
        for (const auto &elem : list.value) {
            value.push_back(elem); // pass by reference
        }
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
