#pragma once

/*
 * Created by Kami-Kaze on 3/27/2021.
 */

#include "core/events/event.h"

#include <functional>
#include <deque>
#include <mutex>

namespace Engine {

    using EventConsumer = std::function<void(Event*)>;

    class MessageQueue {
    public:
        explicit MessageQueue(const EventConsumer& consumer);
        ~MessageQueue() = default;

        void dispatch(Event *e);
        void dispatchImmediately(Event* event);

        void pump();

        Event* peakFront();
        Event* peakBack();

    private:
        EventConsumer callback;
        std::deque<Event*> buffer;
        std::mutex mutex;
    };

}