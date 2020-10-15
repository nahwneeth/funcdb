#include "statement.hpp"

#include "string-helper.hpp"
#include "table.hpp"
#include "b-plus-tree/leaf-node.hpp"

namespace funcdb
{

    std::string const Statement::kCommandInsert = "insert";
    std::string const Statement::kCommandSelect = "select";
    std::string const Statement::kCommandReplace = "replace";
    std::string const Statement::kCommandCommit = "commit";
    std::string const Statement::kCommandRollback = "rollback";

    Statement::Statement(Type type, std::variant<uint32_t, 
                        std::pair<uint32_t, std::string>, 
                        std::nullptr_t> data)
    : m_type{type}, m_data{data}
    {
    }

    std::variant<Statement::PrepareError, Statement>
    Statement::prepare(std::string const& input)
    {
        auto words = split(input);

        if (words.size() <= 0) return PrepareError::UnrecognizedStatement;

        bool is_insert = false;
        if ((is_insert = words[0] == kCommandInsert) || 
            words[0] == kCommandReplace) {
            if (words.size() == 1 + 2) {
                auto key = 0LL;

                try {
                    key = std::stoul(words[1]);
                } catch(const std::exception& e) {
                    return PrepareError::SyntaxError;
                }

                if (words[2].size() > LeafNode::kValueSize) {
                        return PrepareError::StringTooLong;
                }

                auto row = std::pair<uint32_t, std::string>();
                row.first = key;
                row.second = words[2];

                return Statement((is_insert ? Type::Insert : Type::Replace), row);
            } else return PrepareError::SyntaxError;
        } else if (words[0] == kCommandSelect) {
            if (words.size() == 1) {
                return Statement(Type::SelectAll);
            } else if (words.size() == 1 + 1) {
                auto key = 0LL;

                try {
                    key = std::stoul(words[1]);
                } catch(const std::exception& e) {
                    return PrepareError::SyntaxError;
                }

                return Statement(Type::Select, key);

            } else return PrepareError::SyntaxError;
        } else if (words[0] == kCommandCommit) {
            return Statement(Type::Commit);
        } else if (words[0] == kCommandRollback) {
            return Statement(Type::Rollback);
        } else {
            return PrepareError::UnrecognizedStatement;
        }
    }

    Statement::ExecuteResult
    Statement::execute(Table& table) const
    {
        if (m_type == Type::Insert) {
            auto const& data = std::get<std::pair<uint32_t, std::string>>(m_data);
            if(!table.insert(data.first, data.second)) return ExecuteResult::KeyAlreadyExists;
        } else if (m_type == Type::Replace) {
            auto const& data = std::get<std::pair<uint32_t, std::string>>(m_data);
            if(!table.replace(data.first, data.second)) return ExecuteResult::KeyDoesntExist;
        } else if (m_type == Type::Select) {
            auto const& key = std::get<uint32_t>(m_data);
            if (!table.select(key)) return ExecuteResult::KeyDoesntExist;
        } else if (m_type == Type::Commit) {
            table.commit();
        } else if (m_type == Type::Rollback) {
            table.rollback();
        } else {
            table.select_all();
        }

        return ExecuteResult::Success;
    }

} // namespace funcdb
