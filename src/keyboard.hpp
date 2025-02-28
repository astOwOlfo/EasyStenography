#include <map>
#include <string>
#include <optional>
#include <vector>
#include <tuple>
#include <set>
#include <fstream>
#include <iostream>
#include <cctype>
#include <functional>
#include <stdexcept>
#include <stdio.h>
#include <filesystem>
#include <chrono>

extern "C" {
    #include <X11/extensions/XInput2.h>
    #include <X11/Xlib.h>
    #include <X11/XKBlib.h>
    #include <X11/keysym.h>
    #include <X11/extensions/XTest.h>
    #include <xdo.h>
}

/* struct KeyboardEvent {
    std::string symbol;
    int keycode;
    unsigned int group;
    int level;
}; */

struct Key {
    std::string symbol;
    std::string symbol_without_modifiers;
    unsigned int keycode;
    bool shift;
    bool alt_gr;
};

class KeyboardWatcher {
public:
    KeyboardWatcher();
    ~KeyboardWatcher();
    Key wait_for_next_key_event();

private:
    Display* display;
    Window window;
};

class KeyboardWriter {
public:
    KeyboardWriter();
    ~KeyboardWriter();
    // void press_and_release_key(const std::string& symbol);
    // void press_and_release_key(const Key key);
    // void press_and_release_key(unsigned int keycode);
    
    void press_key(unsigned int keycode);
    void press_key(const std::string& symbol_without_modifiers);
    void release_key(unsigned int keycode);
    void release_key(const std::string& symbol_without_modifiers);
    std::vector<bool> are_pressed(const std::vector<unsigned int>& keycodes);

    unsigned int left_shift_keycode;
    unsigned int right_shift_keycode;
    unsigned int alt_gr_keycode;
    unsigned int backspace_keycode;

    void flush();

private:
    Display* display;
    Window window;
};

#pragma once

#include "keyboard.cpp"