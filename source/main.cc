#include <iostream>
#include <string>

#include "meta-command.hpp"
#include "statement.hpp"
#include "string-helper.hpp"
#include "table.hpp"

enum error {
    FILE_NAME_NOT_SPECIFIED = 1,
    TABLE_INIT_ERROR
};

int main(int argc, char** argv)
{

    if (argc < 2) {
        std::cerr << "Filename not specified\n";
        return FILE_NAME_NOT_SPECIFIED;
    }

    auto filepath = argv[1];
    auto table_init = funcdb::Table::init(filepath);

    if (std::holds_alternative<funcdb::Table::InitError>(table_init)) {
        auto const& error = std::get<funcdb::Table::InitError>(table_init);

        switch(error) {
            case funcdb::Table::InitError::FailedToCreateNewFile :
                std::cerr << "Failed to create file " << filepath << "\n";
                break;
            case funcdb::Table::InitError::NoReadPermission :
                std::cerr << "No permission to read from " << filepath << "\n";
                break;
            case funcdb::Table::InitError::NotRegularFile :
                std::cerr << filepath << " is not a regular file\n";
                break;
            case funcdb::Table::InitError::NoWritePermission :
                std::cerr << "No permission to write to " << filepath << "\n";
                break;
        }

        return TABLE_INIT_ERROR;
    }

    auto&& table = std::move(std::get<funcdb::Table>(table_init));

    auto const prompt = std::string("\n[db]: ");

    while (true)
    {

        auto input = funcdb::read_input(prompt);

        if (input.empty()) continue;

        if (input[0] == '.') {
            auto result = funcdb::do_meta_command(input);

            if (result == funcdb::MetaCommandResult::Success) /* do nothing */ ;
            else if (result == funcdb::MetaCommandResult::Exit) break;
            else if (result == funcdb::MetaCommandResult::UnrecognizedCommand) {
                std::cerr << "Unrecognized command [| " << input << " |]\n";
            }

            continue;
        }

        auto statement = funcdb::Statement::prepare(input);
        
        if (std::holds_alternative<funcdb::Statement>(statement)) {
            auto result = std::get<funcdb::Statement>(statement).execute(table);

            switch(result)
            {
                case funcdb::Statement::ExecuteResult::Success :
                    /* do nothing */
                    break;
                case funcdb::Statement::ExecuteResult::KeyAlreadyExists :
                    std::cerr << "Error: Key already exists.\n";
                    break;
                case funcdb::Statement::ExecuteResult::KeyDoesntExist :
                    std::cerr << "Error: Key doesn't exist.\n";
            }
        } else {
            auto error = std::get<funcdb::Statement::PrepareError>(statement);
            switch(error) {
                case funcdb::Statement::PrepareError::UnrecognizedStatement:
                    std::cerr << "Unrecognized token at the start of [| " << input << " |]\n";
                    break;
                case funcdb::Statement::PrepareError::StringTooLong:
                    std::cerr << "String too long.\n";
                    break;
                case funcdb::Statement::PrepareError::SyntaxError:
                    std::cerr << "Syntax error. Couldn't parse statement.\n";
                    break;
            }
        }
    }
}