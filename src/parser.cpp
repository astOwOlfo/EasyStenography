#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <optional>

#include "result.hpp"

std::vector<std::string> read_lines(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream file(filename);
    std::string line;
    
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    
    return lines;
}

std::vector<std::string> split_on_spaces(const std::string& s) {
    std::vector<std::string> parts;

    for (char c : s) {
        if (std::isspace(c)) {
            if (parts.empty()) {
                continue;
            }
            if (!parts.back().empty()) {
                parts.push_back("");
            }
            continue;
        }
        if (parts.empty()) {
            parts.push_back("");
        }
        parts.back() += c;
    }

    if (!parts.empty() && parts.back().empty()) {
        parts.pop_back();
    }

    return parts;
}

Result<PossibleSuffixes, std::string> parse_possible_suffixes(const std::string& s) {
    PossibleSuffixes suffixes {};

    for (char c : s) {
        switch (c) {
        case 's':
            suffixes.s = true;
            break;

        case '\'':
            suffixes.apostrophe_s = true;
            break;

        case 'd':
            suffixes.ed = true;
            break;

        case 'g':
            suffixes.ing = true;
            break;

        case 'r':
            suffixes.er = true;
            break;

        case 'l':
            suffixes.ly = true;
            break;

        case 'y':
            suffixes.y = true;
            break;

        case 'f':
            suffixes.full = true;
            break;

        case 'n':
            suffixes.s = true;
            suffixes.apostrophe_s = true;
            break;

        case 'v':
            suffixes.s = true;
            suffixes.ing = true;
            suffixes.ed = true;
            suffixes.er = true;
            break;

        case 'a':
            suffixes.ly = true;
            suffixes.full = true;
            break;
        
        default:
            std::string message = std::string("Unsupported option \"") + c + "\" in possible suffixes "
                + "string \"" + s + "\". The supported options are \"s\" for plural (-s, -es), ' for "
                + "possessive (-\"s), \"d\" for preterit (-d, -ed), \"g\" for present participle (-ing), "
                + "\"r\" for agentive (-er, -r), \"l\" for adverb form (-ly), \"y\" for adjective form "
                + "(-y), \"f\" for the -full suffix, \"n\" for nouns, which is shorthand to \"s'\", "
                + "\"v\" for verbs, which is equivalent to \"sdgr, and \"a\" for adjectives, which is "
                + "a shorthand for \"lf\".";
            return Result<PossibleSuffixes, std::string>::Error(message);
        }
    }

    return Result<PossibleSuffixes, std::string>::Ok(suffixes);
}

void print_position(const Position& position) {
    std::cerr << position.file << ":" << position.line;
}

void print_error_message(
    const std::string& line,
    const std::string& error_message,
    const std::optional<Position>& position
) {
    if (position.has_value()) {
        print_position(position.value());
    }
    std::cerr << ": Error: " << error_message << std::endl;
    std::cerr << "Line Containing Error: " << line << std::endl;
    std::cerr << std::endl << std::endl;
}

void print_invalid_number_of_fields_error(
    const std::string& line,
    size_t n_fields,
    const std::optional<Position>& position
) {
    print_error_message(
        line,
        "The line has " + std::to_string(n_fields) + " fields. Lines must have either 2 or 3 "
            "fields, separated by spaces or tabs.",
        position
    );
}

void print_conflict(const Conflict& conflict) {
    if (conflict.redefinition_position.has_value()) {
        print_position(conflict.redefinition_position.value());
    }
    std::cerr << ": Error: Abbreviation \"" + conflict.abbreviation << "\" defined here. But this ";
    std::cerr << " abbreviation has already been defined ";
    if (conflict.first_definition_position.has_value()) {
        std::cerr << " at ";
        print_position(conflict.first_definition_position.value());
        std::cerr << " .";
    }
    else {
        std::cerr << "earlier.";
    }
    std::cerr << std::endl;
    if (conflict.unparsed_first_definition.has_value()) {
        std::cerr << "First definition: " << conflict.unparsed_first_definition.value()
            << std::endl;
    }
    if (conflict.unparsed_redefinition.has_value()) {
        std::cerr << "Redefinition:     " << conflict.unparsed_redefinition.value() << std::endl;
    }
    std::cerr << std::endl;
}

void print_file_not_found_error(const std::string& filename) {
    std::cerr << "Error: File \"" << filename << "\" not found." << std::endl << std::endl;
}

void parse_abbreviations_file(Abbreviations& abbreviations, const std::string& filename) {
    if (!std::filesystem::exists(filename)) {
        print_file_not_found_error(filename);
        return;
    }

    std::vector<std::string> lines = read_lines(filename);
    
    for (size_t i_line = 0; i_line < lines.size(); i_line++) {
        Position position { .file = filename, .line = i_line + 1 };
        
        std::string line = lines[i_line];
        
        std::vector<std::string> fields = split_on_spaces(line);

        if (fields.empty()) {
            continue;
        }

        bool is_comment = fields[0][0] == '#';
        if (is_comment) {
            continue;
        }

        if (fields.size() != 2 && fields.size() != 3) {
            print_invalid_number_of_fields_error(line, fields.size(), {position});
            continue;
        }

        if (fields.size() == 2) {
            fields.push_back("");
        }

        std::string expansion = fields[0];
        std::string abbreviation = fields[1];
        Result<PossibleSuffixes, std::string> possible_suffixes_or_error =
            parse_possible_suffixes(fields[2]);

        if (possible_suffixes_or_error.is_error()) {
            print_error_message(line, possible_suffixes_or_error.unwrap_error(), {position});
            continue;
        }

        PossibleSuffixes possible_suffixes = possible_suffixes_or_error.unwrap();

        std::vector<Conflict> conflicts = abbreviations.add_abbreviation_variants_if_no_conflict(
            abbreviation,
            expansion,
            possible_suffixes,
            {position},
            line
        );

        for (int i = 0; i < conflicts.size(); i++) {
            Conflict conflict = conflicts[i];

            bool similar_conflict_already_printed = false;
            for (int j = 0; j < i; j++) {
                if (
                       conflicts[j].first_definition_position == conflict.first_definition_position
                    && conflicts[j].redefinition_position == conflict.redefinition_position
                ) {
                    similar_conflict_already_printed = true;
                }
            }
            if (similar_conflict_already_printed) {
                continue;
            }

            print_conflict(conflict);
        }
    }
}
