#pragma once

#include <cstdlib>
#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace rdb::parser {

using Value = std::variant<int32_t, float, std::string_view>;

using Operand = std::variant<std::string_view, Value>;

enum class Operation {
    Lt,
    Rt,
    Eq,
    Lte,
    Rte,
    Neq,
};

struct Expression {
    Operand left_;
    Operation operation_;
    Operand right_;
};

class Statement {
   public:
    virtual ~Statement() = 0;
    virtual std::string to_string() const = 0;
};

using StatementPtr = std::unique_ptr<const Statement>;

class SelectStatement : public Statement {
   public:
    explicit SelectStatement(
        const std::vector<std::string_view>& column_names,
        const std::string_view table_name,
        const std::optional<Expression> expression = std::nullopt)
        : column_names_(column_names),
          table_name_(table_name),
          expression_(expression) {}

    const std::vector<std::string_view>& column_names() const {
        return column_names_;
    }

    std::string_view table_name() const { return table_name_; }

    std::optional<Expression> expression() const { return expression_; }

    std::string to_string() const override;

   private:
    std::vector<std::string_view> column_names_;
    std::string_view table_name_;
    std::optional<Expression> expression_;
};

using SelectStatementPtr = std::unique_ptr<const SelectStatement>;

class InsertStatement : public Statement {
   public:
    explicit InsertStatement(
        const std::string_view table_name,
        const std::vector<std::string_view>& column_names,
        const std::vector<Value>& values)
        : table_name_(table_name),
          column_names_(column_names),
          values_(values) {}
    std::string_view table_name() const { return table_name_; }
    const std::vector<std::string_view>& column_names() const {
        return column_names_;
    }
    const std::vector<Value>& values() const { return values_; }

    std::string to_string() const override;

   private:
    std::string_view table_name_;
    std::vector<std::string_view> column_names_;
    std::vector<Value> values_;
};

using InsertStatementPtr = std::unique_ptr<const InsertStatement>;

class DeleteStatement : public Statement {
   public:
    explicit DeleteStatement(
        const std::string_view table_name,
        const std::optional<Expression> expression = std::nullopt)
        : table_name_(table_name), expression_(expression) {}

    std::string_view table_name() const { return table_name_; }

    std::optional<Expression> expression() const { return expression_; }

    std::string to_string() const override;

   private:
    std::string_view table_name_;
    std::optional<Expression> expression_;
};

using DeleteStatementPtr = std::unique_ptr<const DeleteStatement>;

class DropTableStatement : public Statement {
   public:
    explicit DropTableStatement(std::string_view table_name)
        : table_name_(table_name) {}
    std::string_view table_name() const { return table_name_; }

    std::string to_string() const override;

   private:
    std::string_view table_name_;
};

using DropTableStatementPtr = std::unique_ptr<const DropTableStatement>;

}  // namespace rdb::parser
