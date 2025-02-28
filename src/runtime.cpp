#include <optional>
#include <chrono>

#include "abbreviations.hpp"
#include "keyboard.hpp"
#include "notification.hpp"

void show_abbreviation_suggestion_notification(
    const std::string& word,
    const std::vector<std::string>& possible_abbreviations
) {
    std::string message = std::string("\"") + word + "\" could be abbreviated as ";
    for (int i = 0; i < possible_abbreviations.size(); i++) {
        message += std::string("\"") + possible_abbreviations[i] + "\"";
        if (i == possible_abbreviations.size() - 1) {
            continue;
        }
        if (i == possible_abbreviations.size() - 2) {
            if (possible_abbreviations.size() == 2) {
                message += std::string(" or ");
            }
            else {
                message += std::string(", or ");
            }
            continue;
        }
        message += std::string(", ");
    }
    show_notification(message);
}

bool is_ascii_letter_or_apsotrophe(char c) {
    if (c == '\'') {
        return true;
    }
    if ('a' <= c && c <= 'z') {
        return true;
    }
    if ('A' <= c && c <= 'Z') {
        return true;
    }
    return false;
}

void erase_abbreviation_and_type_expansion(
    KeyboardWatcher& keyboard_watcher,
    KeyboardWriter& keyboard_writer,
    const std::string& abbreviation,
    const std::string& expansion,
    const Key& key_pressed_after_abbreviation_was_typed
) {
    int n_key_presses = 0;
    
    for (int i = 0; i < abbreviation.length() + 1; i++) {
        keyboard_writer.press_key(keyboard_writer.backspace_keycode);
        keyboard_writer.release_key(keyboard_writer.backspace_keycode);
        n_key_presses++;
    }

    auto x = keyboard_writer.are_pressed({
        keyboard_writer.left_shift_keycode,
        keyboard_writer.right_shift_keycode,
        keyboard_writer.alt_gr_keycode,
    });
    bool left_shift_pressed_initially = x[0];
    bool right_shift_pressed_initially = x[1];
    bool alt_gr_pressed_initially = x[2];

    std::cout << "left shift pressed initially: " << left_shift_pressed_initially << " right shift pressed initially: " << right_shift_pressed_initially << std::endl;

    std::cout << "alt gr key code: " << keyboard_writer.alt_gr_keycode << std::endl;
    std::cout << "alt gr pressed: " << alt_gr_pressed_initially << std::endl;

    // if (left_shift_pressed_initially) {
        std::cout << "releasing left shift" << std::endl;
        keyboard_writer.release_key(keyboard_writer.left_shift_keycode);
    // }
    // if (right_shift_pressed_initially) {
        std::cout << "releasing right shift" << std::endl;
        keyboard_writer.release_key(keyboard_writer.right_shift_keycode);
    // }
    // if (alt_gr_pressed_initially) {
        std::cout << "realeasing alt-gr" << std::endl;
        keyboard_writer.release_key(keyboard_writer.alt_gr_keycode);
    // }

    bool left_shift_pressed = false;

    for (char letter : expansion) {
        std::cout << " letter: " << letter << std::endl;
        if (std::islower(letter) && left_shift_pressed) {
            keyboard_writer.release_key(keyboard_writer.left_shift_keycode);
            left_shift_pressed = false;
        } else if (std::isupper(letter) && !left_shift_pressed) {
            keyboard_writer.press_key(keyboard_writer.left_shift_keycode);
            left_shift_pressed = true;
            n_key_presses++;
        }

        bool abbreviation_contains_letter =
               abbreviation.find(letter)               != std::string::npos
            || abbreviation.find(std::toupper(letter)) != std::string::npos;

        std::string key_symbol = letter == ' ' ? " " : std::string(1, std::tolower(letter));
        std::cout << "key symbol: " << key_symbol << std::endl;
        if (abbreviation_contains_letter) {
            keyboard_writer.release_key(key_symbol);
        }
        keyboard_writer.press_key(key_symbol);
        keyboard_writer.release_key(key_symbol);
        n_key_presses++;
    }

    if (key_pressed_after_abbreviation_was_typed.shift && !left_shift_pressed) {
        keyboard_writer.press_key(keyboard_writer.left_shift_keycode);
        n_key_presses++;
    }
    if (!key_pressed_after_abbreviation_was_typed.shift && left_shift_pressed) {
        keyboard_writer.release_key(keyboard_writer.left_shift_keycode);
    }
    bool alt_gr_pressed = key_pressed_after_abbreviation_was_typed.alt_gr;
    if (alt_gr_pressed) {
        keyboard_writer.press_key(keyboard_writer.alt_gr_keycode);
        n_key_presses++;
    }

    // to do: condition on the first key release here
    keyboard_writer.release_key(key_pressed_after_abbreviation_was_typed.symbol_without_modifiers);
    keyboard_writer.press_key(key_pressed_after_abbreviation_was_typed.symbol_without_modifiers);
    keyboard_writer.release_key(key_pressed_after_abbreviation_was_typed.symbol_without_modifiers);
    n_key_presses++;

    if (left_shift_pressed_initially && !left_shift_pressed) {
        keyboard_writer.press_key(keyboard_writer.left_shift_keycode);
        n_key_presses++;
    }
    if (!left_shift_pressed_initially && left_shift_pressed) {
        keyboard_writer.release_key(keyboard_writer.left_shift_keycode);
    }

    if (right_shift_pressed_initially) {
        keyboard_writer.press_key(keyboard_writer.right_shift_keycode);
        n_key_presses++;
    }

    if (alt_gr_pressed_initially && !alt_gr_pressed) {
        keyboard_writer.press_key(keyboard_writer.alt_gr_keycode);
        n_key_presses++;
    }
    if (!alt_gr_pressed_initially && alt_gr_pressed) {
        keyboard_writer.release_key(keyboard_writer.alt_gr_keycode);
    }

    keyboard_writer.flush();

    // ignore the key presses done by this function
    for (int i = 0; i < n_key_presses; i++) {
        Key key = keyboard_watcher.wait_for_next_key_event();
        std::cout << "ignoring key: " << key.symbol_without_modifiers << std::endl;
    }
}

std::vector<std::string> NON_LETTER_SYMBOL_KEYS {
    "space",
    "underscore",
    "exclam",
    "at",
    "numbersign",
    "percent",
    "asciicircum",
    "ampersand",
    "asterisk",
    "parenleft",
    "parenright",
    "braceleft",
    "braceright",
    "bracketleft",
    "bracketright",
    "minus",
    "equal",
    "plus",
    "colon",
    "semicolon",
    "quotedbl",
    "less",
    "greater",
    "comma",
    "period",
    "question",
    "slash",
    "backslash",
    "bar",
    "Return"
};

void watch_and_expand_abbreviations(const Abbreviations& abbreviations) {
    KeyboardWatcher keyboard_watcher {};
    KeyboardWriter keyboard_writer {};

    bool shift = false;
    bool combination = false;

    std::string current_word;

    while (true) {
        Key key = keyboard_watcher.wait_for_next_key_event();
        std::string key_symbol = key.symbol == std::string("apostrophe") ? std::string("'") : key.symbol;

        std::cout << "pressed: " << key_symbol << " without modifiers: " << key.symbol_without_modifiers << " keycode: " << key.keycode << " current word: " << current_word << std::endl;
        std::cout << "left shift keycode: " << keyboard_writer.left_shift_keycode << std::endl;

        if (key.symbol_without_modifiers == "BackSpace") {
            if (!current_word.empty()) {
                current_word = current_word.substr(0, current_word.length() - 1);
            }
            continue;
        }

        bool letter_or_apostrophe = key_symbol.length() == 1
            && is_ascii_letter_or_apsotrophe(key_symbol[0]);
        if (letter_or_apostrophe) {
            current_word += key_symbol;
            continue;
        }

        if (current_word.empty()) {
            continue;
        }

        if (
               key.symbol_without_modifiers == "Shift_L"
            || key.symbol_without_modifiers == "Shift_R"
            || key.symbol_without_modifiers == "ISO_Level3_Shift"
        ) {
            continue;
        }

        bool non_letter_symbol = std::find(
            NON_LETTER_SYMBOL_KEYS.begin(),
            NON_LETTER_SYMBOL_KEYS.end(),
            key_symbol
        ) != NON_LETTER_SYMBOL_KEYS.end();
        if (!non_letter_symbol) {
            current_word = "";
            continue;
        }

        std::optional<std::string> expansion = abbreviations.expand(current_word);
        bool expand_abbreviation = expansion.has_value() && current_word != expansion.value();
        if (expand_abbreviation) {
            erase_abbreviation_and_type_expansion(
                keyboard_watcher,
                keyboard_writer,
                current_word,
                expansion.value(),
                key
            );
        }
        else {
            std::vector<std::string> possible_abbreviations =
                abbreviations.possible_abbreviations(current_word);
            for (std::string abbreviation : possible_abbreviations) {
                if (abbreviation.length() < current_word.length()) {
                    show_abbreviation_suggestion_notification(current_word, possible_abbreviations);
                    continue;
                }
            }
        }

        current_word = "";
    }
}
