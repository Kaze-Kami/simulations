#pragma once

/*
 * Created by Kami-Kaze on 4/14/2021.
 */

#include "event.h"
#include "core/input/key_codes.h"

namespace Engine {

    class KeyEvent : public Event {
    public:
        const KeyCode code;
        const int mods;

        inline KeyEvent(KeyCode code, int mods) : code(code), mods(mods) {}
        virtual ~KeyEvent() = default;
    };

    class KeyPressEvent : public KeyEvent {
    public:
        EVENT_CLASS_TYPE(KeyPress);

        inline KeyPressEvent(KeyCode code, int mods) : KeyEvent(code, mods) {}
    };

    class KeyReleaseEvent : public KeyEvent {
    public:
        EVENT_CLASS_TYPE(KeyRelease);

        inline KeyReleaseEvent(KeyCode code, int mods) : KeyEvent(code, mods) {}
    };

}