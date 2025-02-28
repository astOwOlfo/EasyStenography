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
#include <stdio.h>
#include <stdlib.h>
#include <tuple>

extern "C" {
    #include <X11/extensions/XInput2.h>
    #include <X11/Xlib.h>
    #include <X11/XKBlib.h>
    #include <X11/keysym.h>
    #include <X11/extensions/XTest.h>
    #include <xdo.h>
    #include <X11/Xutil.h>
    #include <xdo.h>
}

void select_keyboard_events(Display* display, Window window) {
    XIEventMask mask[2];
    XIEventMask *m;

    m = &mask[0];
    m->deviceid = XIAllDevices;
    m->mask_len = XIMaskLen(XI_LASTEVENT);
    m->mask = (unsigned char*)calloc(m->mask_len, sizeof(char));
    XISetMask(m->mask, XI_KeyPress);
    // XISetMask(m->mask, XI_KeyRelease);

    m = &mask[1];
    m->deviceid = XIAllMasterDevices;
    m->mask_len = XIMaskLen(XI_LASTEVENT);
    m->mask = (unsigned char*)calloc(m->mask_len, sizeof(char));

    XISelectEvents(display, window, &mask[0], 2);
    XSync(display, False);

    free(mask[0].mask);
    free(mask[1].mask);
}

std::string keycode_to_string(Display* display, int keycode) {
    KeySym key_sym = XkbKeycodeToKeysym(display, keycode, 0, 0);
    return std::string(XKeysymToString(key_sym));
}

KeyboardWatcher::KeyboardWatcher() {
    display = XOpenDisplay(NULL);
    window = DefaultRootWindow(display);
    select_keyboard_events(display, window);
}

KeyboardWatcher::~KeyboardWatcher() {
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

std::string get_key_string(Display* display, XEvent event, int group, int level) {
    int keycode = ((XIDeviceEvent*)event.xcookie.data)->detail;
    KeySym keysym = XkbKeycodeToKeysym(display, keycode, group, level);
    char* symbol = XKeysymToString(keysym);
    if (!symbol) {
        return "unknown";
    }
    return std::string(symbol);
}

Key KeyboardWatcher::wait_for_next_key_event() {
    while (true)
    {
        XEvent event;
        XNextEvent(display, &event);


        bool can_get_event_data = XGetEventData(display, &event.xcookie);
        bool is_generic_event = event.xcookie.type == GenericEvent;
        int event_type = event.xcookie.evtype;
        bool key_press = event_type == XI_KeyPress;

        bool return_event = can_get_event_data && is_generic_event && key_press;
        if (return_event) {
            unsigned int keycode = ((XIDeviceEvent*)event.xcookie.data)->detail;
            
            XkbStateRec state;
            XkbGetState(display, XkbUseCoreKbd, (XkbStatePtr)&state);

            bool shift  = state.mods & ShiftMask;
            bool alt_gr = state.mods & Mod5Mask;
            int level = 2 * alt_gr + shift;

            unsigned int group = state.group;

            std::string symbol = get_key_string(display, event, group, level);
            std::string symbol_without_modifiers = get_key_string(display, event, group, 0);
            
            XFreeEventData(display, &event.xcookie);
            
            return Key {
                .symbol = symbol,
                .symbol_without_modifiers = symbol_without_modifiers,
                .keycode = keycode,
                .shift = shift,
                .alt_gr = alt_gr
            };
        } else {
            XFreeEventData(display, &event.xcookie);
        }
    }
}

std::vector<bool> KeyboardWriter::are_pressed(const std::vector<unsigned int>& keycodes) {
    char keys[32];
    XQueryKeymap(display, keys);

    std::vector<bool> pressed {};
    for (bool keycode : keycodes) {
        bool is_pressed = keys[keycode/8] & (1 << (keycode % 8));
        pressed.push_back(is_pressed);
    }
    return pressed;
}

unsigned int string_to_keycode(Display* display, const std::string& symbol_without_modifiers) {
    KeySym key_symbol = XStringToKeysym(symbol_without_modifiers.c_str());
    unsigned int keycode = XKeysymToKeycode(display, key_symbol);
    return keycode;
}

KeyboardWriter::KeyboardWriter() {
    display = XOpenDisplay(NULL);
    window = DefaultRootWindow(display);

    left_shift_keycode = XKeysymToKeycode(display, XK_Shift_L);
    right_shift_keycode = XKeysymToKeycode(display, XK_Shift_R);
    alt_gr_keycode = 108; // for some reason XKeysymToKeycode returns the wrong keycode
    backspace_keycode = XKeysymToKeycode(display, XK_BackSpace);
}

KeyboardWriter::~KeyboardWriter() {
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

void KeyboardWriter::press_key(const std::string& symbol_without_modifiers) {
    press_key(string_to_keycode(display, symbol_without_modifiers));
}

void KeyboardWriter::release_key(const std::string& symbol_without_modifiers) {
    release_key(string_to_keycode(display, symbol_without_modifiers));
}

void KeyboardWriter::press_key(unsigned int keycode) {
    XTestFakeKeyEvent(display, keycode, true, 0);
}

void KeyboardWriter::release_key(unsigned int keycode) {
    XTestFakeKeyEvent(display, keycode, false, 0);
}

/* void KeyboardWriter::press_and_release_key(unsigned int keycode) {
    unsigned int left_shift_keycode = XKeysymToKeycode(display, XK_Shift_L);
    unsigned int right_shift_keycode = XKeysymToKeycode(display, XK_Shift_R);
    unsigned int alt_gr_keycode = 108; // for some reason XKeysymToKeycode returns the wrong keycode
    // std::cout << "left shift pressed: " << is_pressed(display, left_shift_keycode) << " right shift is pressed: " << is_pressed(display, right_shift_keycode) << " alt gr is pressed: " << is_pressed(display, alt_gr_keycode) << std::endl;

    XTestFakeKeyEvent(display, left_shift_keycode, false, 0);
    XTestFakeKeyEvent(display, right_shift_keycode, false, 0);
    XTestFakeKeyEvent(display, alt_gr_keycode, false, 0);
    
    XTestFakeKeyEvent(display, keycode, false, 0);  // release
    XTestFakeKeyEvent(display, keycode, true, 0);   // press
    XTestFakeKeyEvent(display, keycode, false, 0);  // release
}; */

void KeyboardWriter::flush() {
    // XSync(display, False);
    XFlush(display);
}
