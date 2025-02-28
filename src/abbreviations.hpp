#include <string>
#include <vector>
#include <tuple>
#include <optional>
#include <map>

// Position in a file.
struct Position {
    const std::string& file;
    const size_t line;
};

bool operator==(const Position& left, const Position& right);

struct Conflict {
    const std::string abbreviation;
    const std::optional<std::string> unparsed_first_definition;
    const std::optional<Position> first_definition_position;
    const std::optional<std::string> unparsed_redefinition;
    const std::optional<Position> redefinition_position;
};

struct PossibleSuffixes {
    bool s;
    bool apostrophe_s;
    bool ed;
    bool ing;
    bool er;
    bool ly;
    bool y;
    bool full;
};

class Abbreviations {
public:
    std::optional<std::string> expand(const std::string& abbreviation) const;
    std::vector<std::string> possible_abbreviations(const std::string& expansion) const;

    bool has_abbreviation(const std::string& abbreviation) const;

    std::optional<Conflict> add_abbreviation_if_no_conflict(
        const std::string& abbreviation,
        const std::string& expansion,
        const std::optional<const Position>& definition_position,
        const std::optional<const std::string>& unparsed_definition
    );

    std::vector<Conflict> add_abbreviation_variants_if_no_conflict(
        const std::string& abbreviation,
        const std::string& expansion,
        const PossibleSuffixes& possible_suffixes,
        const std::optional<const Position>& definition_position,
        const std::optional<const std::string>& unparsed_definition
    );

private:
    std::map<const std::string, const std::string> abbreviation_to_expansion;
    std::map<const std::string, std::vector<std::string>> expansion_to_abbreviations;
    std::map<const std::string, const std::optional<const Position>>
        abbreviation_to_definition_position;
    std::map<const std::string, const std::optional<const std::string>> abbreviation_to_unparsed_definition;
};


#pragma once

#include "abbreviations.cpp"