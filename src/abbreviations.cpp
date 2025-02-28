#include <string>
#include <vector>
#include <tuple>
#include <optional>
#include <map>

#include "parser.hpp"

bool ends_with(const std::string& word, const std::string& suffix) {
    if (suffix.length() > word.length()) {
        return false;
    }
    for (size_t i = 0; i < suffix.length(); i++) {
        if (word[word.length() - i - 1] != suffix[suffix.length() - i - 1]) {
            return false;
        }
    }
    return true;
}

bool is_consonant(char c) {
    switch (std::tolower(c)) {
        case 'b':
        case 'c':
        case 'd':
        case 'f':
        case 'g':
        case 'h':
        case 'j':
        case 'l':
        case 'm':
        case 'n':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'v':
        case 'w':
        case 'x':
        case 'z':
            return true;
        default:
            return false;
    }
}

bool is_vowel(char c) {
    switch (std::tolower(c)) {
        case 'a':
        case 'e':
        case 'i':
        case 'o':
        case 'u':
        case 'y':
            return true;
        default:
            return false;
    }
}

std::string plural(const std::string& word) {
    if (  
           ends_with(word, "i") || ends_with(word, "u")
        && !(word.length() >= 2 && is_vowel(word[word.length() - 2]))
    ) {
        return word + "es";
    }
    if (ends_with(word, "y")) {
        return word.substr(0, word.length() - 1) + "ies";
    }
    return word + "s";
}

std::string append_e(const std::string& word) {
    if (ends_with(word, "e")) {
        return word;
    }
    if (ends_with(word, "y")) {
        return word.substr(0, word.length() - 1) + "ie";
    }
    if (word.length() >= 2 && is_consonant(word.back()) && is_vowel(word[word.length() - 2])) {
        return word + word.back() + "e";
    }
    return word + "e";
}

std::string preterit(const std::string& word) {
    return append_e(word) + "d";
}

std::string agentive(const std::string& word) {
    return append_e(word) + "r";
}

std::string present_participle(const std::string& word) {
    if (word.back() == 'e') {
        return word.substr(0, word.size() - 1) + "ing";
    }
    if (word.length() >= 2 && is_consonant(word.back()) && is_vowel(word[word.length() - 2])) {
        return word + word.back() + "ing";
    }
    return word + "ing";
}

std::string adverb_form(const std::string& word) {
    if (ends_with(word, "y")) {
        if (word.length() >= 2 && is_vowel(word[word.length() - 2])) {
            return word + "ly";
        }
        return word.substr(0, word.length() - 1) + "ily";
    }
    if (ends_with(word, "le")) {
        return word.substr(0, word.length() - 1) + "y";
    }
    if (ends_with(word, "ic")) {
        return word + "ally";
    }
    if (ends_with(word, "ll")) {
        return word + "y";
    }
    if (ends_with(word, "e")) {
        if (word.length() >= 2 && is_vowel(word[word.length() - 2])) {
            return word + "ly";
        }
        return word.substr(0, word.length() - 1) + "ly";
    }
    return word + "ly";
}

std::string adjective_form(const std::string& word) {
    if (ends_with(word, "e")) {
        return word.substr(0, word.length() - 1) + "y";
    }
    if (word.length() >= 2 && is_consonant(word.back()) && is_vowel(word[word.length() - 2])) {
        return word + word.back() + "y";
    }
    return word + "y";
}

std::string add_full_suffix(const std::string& word) {
    if (ends_with(word, "e")) {
        return word.substr(0, word.length() - 1) + "full";
    }
    if (ends_with(word, "y")) {
        return word.substr(0, word.length() - 1) + "ifull";
    }
    return word + "full";
}

std::vector<std::tuple<std::string, std::string>> variants_with_suffixes(
    const std::string& abbreviation,
    const std::string& expansion,
    const PossibleSuffixes& possible_suffixes
) {
    std::vector<std::tuple<std::string, std::string>> variants {};
    
    variants.push_back({abbreviation, expansion});

    if (possible_suffixes.s) {
        variants.push_back({abbreviation + "s", plural(expansion)});
        variants.push_back({abbreviation + "s'", plural(expansion) + "'"});
    }

    if (possible_suffixes.apostrophe_s) {
        variants.push_back({abbreviation + "'", expansion + "'s"});
    }

    if (possible_suffixes.ed) {
        variants.push_back({abbreviation + "d", preterit(expansion)});

    }

    if (possible_suffixes.ing) {
        variants.push_back({abbreviation + "g", present_participle(expansion)});
    }

    if (possible_suffixes.er) {
        variants.push_back({abbreviation + "r", agentive(expansion)});
        variants.push_back({abbreviation + "rs", agentive(expansion) + "s"});
        variants.push_back({abbreviation + "r'", agentive(expansion) + "'s"});
        variants.push_back({abbreviation + "rs'", agentive(expansion) + "s'"});
    }

    if (possible_suffixes.ly) {
        variants.push_back({abbreviation + "l", adverb_form(expansion)});
    }

    if (possible_suffixes.y) {
        variants.push_back({abbreviation + "y", adjective_form(expansion)});
        variants.push_back({abbreviation + "yl", adverb_form(adjective_form(expansion))});
        variants.push_back({abbreviation + "yf", add_full_suffix(adjective_form(expansion))});
    }

    if (possible_suffixes.full) {
        variants.push_back({abbreviation + "f", add_full_suffix(expansion)});
    }

    for (auto [abbreviation, _]: variants) {
        if (abbreviation.length() == 0) {
            continue;
        }
        variants.push_back({std::string("'") + abbreviation, abbreviation});
    }

    return variants;
}

std::vector<std::tuple<std::string, std::string>> capitalization_variants(
    const std::string& abbreviation,
    const std::string& expansion
) {
    for (char c : abbreviation) {
        if (std::tolower(c) != c) {
            return {{abbreviation, expansion}};
        }
    }

    int abbreviation_i_first_letter = 0;
    for (size_t i = 0; i < abbreviation.length(); i++) {
        if (std::toupper(abbreviation[i]) != abbreviation[i]) {
            abbreviation_i_first_letter = i;
            break;
        }
    }

    const std::string abbreviation_with_first_letter_capitalized =
          abbreviation.substr(0, abbreviation_i_first_letter)
        + ((char)std::toupper(abbreviation[abbreviation_i_first_letter]))
        + abbreviation.substr(abbreviation_i_first_letter + 1, abbreviation.length() - 1);

    int expansion_i_first_letter = 0;
    for (size_t i = 0; i < expansion.length(); i++) {
        if (std::toupper(expansion[i]) != expansion[i]) {
            expansion_i_first_letter = i;
            break;
        }
    }

    const std::string expansion_with_first_letter_capitalized =
          expansion.substr(0, expansion_i_first_letter)
        + ((char)std::toupper(expansion[expansion_i_first_letter]))
        + expansion.substr(expansion_i_first_letter + 1, expansion.length() - 1);

    std::string capitalized_abbreviation {};
    for (char c : abbreviation) {
        capitalized_abbreviation += std::toupper(c);
    }

    std::string capitalized_expansion {};
    for (char c : expansion) {
        capitalized_expansion += std::toupper(c);
    }

    if (abbreviation_with_first_letter_capitalized == capitalized_abbreviation) {
        return {
            {abbreviation, expansion},
            {abbreviation_with_first_letter_capitalized, expansion_with_first_letter_capitalized}
        };
    }

    return {
        {abbreviation, expansion},
        {abbreviation_with_first_letter_capitalized, expansion_with_first_letter_capitalized},
        {capitalized_abbreviation, capitalized_expansion}
    };
}

std::vector<std::tuple<std::string, std::string>> variants(
    const std::string& abbreviation,
    const std::string& expansion,
    const PossibleSuffixes possible_suffixes
) {
    std::vector<std::tuple<std::string, std::string>> variants_ {};

    for (
        auto [abbreviation_variant, expansion_variant] :
            variants_with_suffixes(abbreviation, expansion, possible_suffixes)
    ) {
        for (
            auto [abbreviation_variant_variant, expansion_variant_vaniant] :
                capitalization_variants(abbreviation_variant, expansion_variant)
        ) {
            variants_.push_back({abbreviation_variant_variant, expansion_variant_vaniant});
        }
    }

    return variants_;
}

std::optional<std::string> Abbreviations::expand(const std::string& abbreviation) const {
    auto it = abbreviation_to_expansion.find(abbreviation);
    if (it == abbreviation_to_expansion.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::vector<std::string> Abbreviations::possible_abbreviations(const std::string& expansion) const {
    auto it = expansion_to_abbreviations.find(expansion);
    if (it == expansion_to_abbreviations.end()) {
        return {};
    }
    return it->second;
}

bool Abbreviations::has_abbreviation(const std::string& abbreviation) const {
    return abbreviation_to_expansion.find(abbreviation) != abbreviation_to_expansion.end();
}

std::optional<Conflict> Abbreviations::add_abbreviation_if_no_conflict(
    const std::string& abbreviation,
    const std::string& expansion,
    const std::optional<const Position>& definition_position,
    const std::optional<const std::string>& unparsed_definition
) {
    if (has_abbreviation(abbreviation)) {
        return Conflict {
            .abbreviation = abbreviation,
            .unparsed_first_definition = abbreviation_to_unparsed_definition[abbreviation],
            .first_definition_position = abbreviation_to_definition_position[abbreviation],
            .unparsed_redefinition = unparsed_definition,
            .redefinition_position = definition_position
        };
    }

    abbreviation_to_expansion.emplace(abbreviation, expansion);
    abbreviation_to_definition_position.emplace(abbreviation, definition_position);
    abbreviation_to_unparsed_definition.emplace(abbreviation, unparsed_definition);

    if (expansion_to_abbreviations.find(expansion) == expansion_to_abbreviations.end()) {
        expansion_to_abbreviations.emplace(expansion, std::vector<std::string> {abbreviation});
    }
    else {
        expansion_to_abbreviations[expansion].push_back(abbreviation);
    }

    return std::nullopt;
}

std::vector<Conflict> Abbreviations::add_abbreviation_variants_if_no_conflict(
    const std::string& abbreviation,
    const std::string& expansion,
    const PossibleSuffixes& possible_suffixes,
    const std::optional<const Position>& definition_position,
    const std::optional<const std::string>& unparsed_definition
) {
    std::vector<Conflict> conflicts {};

    for (
        auto [abbreviation_variant, expansion_variant]
            : variants(abbreviation, expansion, possible_suffixes)
    ) {
        std::optional<Conflict> conflict = add_abbreviation_if_no_conflict(
            abbreviation_variant,
            expansion_variant,
            definition_position,
            unparsed_definition
        );
            
        if (conflict.has_value()) {
            conflicts.push_back(conflict.value());
        }
    }

    return conflicts;
}

bool operator==(const Position& left, const Position& right) {
    return left.file == right.file && left.line == right.line;
}