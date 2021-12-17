#pragma once

#include <cstdlib>
#include <exception>
#include <memory>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace rdb::parser {

using Value = std::variant<int32_t, float, std::string_view>;

class Statement {
   public:
    virtual ~Statement() = 0;
    virtual std::string to_string() const = 0;
};

using StatementPtr = std::unique_ptr<const Statement>;

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

class InsertStatement : public Statement {
   public:
    InsertStatement(
        const std::string_view table_name,
        const std::vector<std::string_view>& column_names,
        const std::vector<Value>& values)
        : table_name_(table_name),
          column_names_(column_names),
          values_(values) {}
    std::string_view table_name() const { return table_name_; }
    std::vector<std::string_view>& column_names() { return column_names_; }
    std::vector<Value>& values() { return values_; }

    std::string to_string() const override;

   private:
    std::string_view table_name_;
    std::vector<std::string_view> column_names_;
    std::vector<Value> values_;
};

using InsertStatementPtr = std::unique_ptr<const InsertStatement>;

}  // namespace rdb::parser
