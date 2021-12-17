#include <librdb/parser/Statements.hpp>

#include <sstream>
#include <string>
#include <string_view>

namespace rdb::parser {

Statement::~Statement() = default;

static std::string value_to_str(const Value& value) {
    if (const int32_t* pval = std::get_if<int32_t>(&value)) {
        return std::to_string(*pval);
    }
    if (const float* pval = std::get_if<float>(&value)) {
        return std::to_string(*pval);
    }
    return std::string(*std::get_if<std::string_view>(&value));
}

static std::string operand_to_str(const Operand& operand) {
    if (const std::string_view* pval =
            std::get_if<std::string_view>(&operand)) {
        return std::string(*pval);
    }
    return value_to_str(*std::get_if<Value>(&operand));
}

static std::string operation_to_str(const Operation& operation) {
    switch (operation) {
        case Operation::Lte:
            return "<=";
        case Operation::Rte:
            return ">=";
        case Operation::Neq:
            return "!=";
        case Operation::Lt:
            return "<";
        case Operation::Rt:
            return ">";
        case Operation::Eq:
            return "=";
    }
    return "Unexpected";
}

static std::string expression_to_str(const Expression& expression) {
    return operand_to_str(expression.left_) + " " +
           operation_to_str(expression.operation_) + " " +
           operand_to_str(expression.right_);
}

std::string SelectStatement::to_string() const {
    std::stringstream out;
    out << "SELECT ";

    for (auto column_name : column_names()) {
        out << column_name << " ";
    }

    out << "FROM " << table_name();

    if (expression()) {
        out << " WHERE " << expression_to_str(*expression());
    }

    out << ";";

    return out.str();
}

std::string InsertStatement::to_string() const {
    std::stringstream out;
    auto column_name = column_names().begin();
    out << "INSERT INTO " << table_name() << " (" << *column_name;
    for (column_name++; column_name != column_names().end(); column_name++) {
        out << ", " << *column_name;
    }
    auto value = values().begin();
    out << ") VALUES (" << value_to_str(*value);
    for (value++; value != values().end(); value++) {
        out << ", " << value_to_str(*value);
    }
    out << ");";

    return out.str();
}

std::string DeleteStatement::to_string() const {
    std::stringstream out;
    out << "DELETE FROM " << table_name();
    if (expression()) {
        out << " WHERE " << expression_to_str(*expression());
    }
    out << ";";

    return out.str();
}

std::string DropTableStatement::to_string() const {
    std::stringstream out;
    out << "DROP TABLE " << table_name() << ";";
    return out.str();
}

}  // namespace rdb::parser
