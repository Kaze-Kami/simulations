#pragma once

/*
 * Created by Kami-Kaze on 3/27/2021.
 *
 */

#include <string>
#include <sstream>

namespace Engine {

    enum class EventType {
        None = 0,
        WindowClose, WindowResize, WindowMove, ViewportChange,
        MouseMove, MouseButtonPress, MouseButtonRelease, MouseWheelScroll,
        KeyPress, KeyRelease,
    };

#define EVENT_CLASS_TYPE(type) inline static EventType getStaticType() { return EventType::type; } \
                               inline virtual EventType getType() const override { return getStaticType(); }      \
                               inline const char* getName() const override { return #type; }
    class Event {
    public:
        virtual ~Event() = default;

        bool handled = false;

        virtual EventType getType() const = 0;
        virtual const char* getName() const = 0;
        inline virtual std::string toString() const { return getName(); }
    };

    class EventDispatcher
    {
    public:
        explicit EventDispatcher(Event& event): event(event) {}

        template<typename T, typename F>
        inline void dispatch(const F& func)
        {
            if (event.getType() == T::getStaticType())
            {
                event.handled |= func(static_cast<T&>(event));
            }
        }
    private:
        Event& event;
    };


    inline std::ostream& operator<<(std::ostream& os, const Event& e)
    {
        return os << e.toString();
    }
}
