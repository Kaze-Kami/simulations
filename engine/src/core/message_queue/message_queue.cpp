/*
 * Created by Kami-Kaze on 3/27/2021.
 */

#include "message_queue.h"

namespace Engine {

    Engine::MessageQueue::MessageQueue(const EventConsumer& consumer) : callback(consumer) {}

    void Engine::MessageQueue::dispatch(Event* e) {
        mutex.lock();
        buffer.push_back(e);
        mutex.unlock();
    }

    void Engine::MessageQueue::dispatchImmediately(Event* event) {
        mutex.lock();
        buffer.push_front(event);
        mutex.unlock();
    }

    void Engine::MessageQueue::pump() {
        mutex.lock();
        while (!buffer.empty()) {
            Event* e = buffer.front();
            buffer.pop_front();
            callback(e);

            // event is handled
            delete e;
        }
        mutex.unlock();
    }

    Event* MessageQueue::peakFront() {
        mutex.lock();
        auto r = buffer.front();
        mutex.unlock();
        return r;
    }

    Event* MessageQueue::peakBack() {
        mutex.lock();
        auto r = buffer.back();
        mutex.unlock();
        return r;
    }

}