//
// Created by Lukas on 02.11.2021.
//

#ifndef SIM_TO_DUT_INTERFACE_SHAREDQUEUE_H
#define SIM_TO_DUT_INTERFACE_SHAREDQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

namespace sim_interface {
    template<class T>
    class SharedQueue {
    public:
        SharedQueue() = default;

        SharedQueue(const SharedQueue<T> &) = delete;

        void push(T elem) {
            std::unique_lock<std::mutex> lock(mutex);
            if (enqueue) {
                queue.push(elem);
                conditionVariable.notify_one();
            }
        }

        bool pop(T &elem) {
            std::unique_lock<std::mutex> lock(mutex);
            while (queue.empty() && !requestEnd) {
                conditionVariable.wait(lock);
            }
            if (requestEnd) {
                End();
                return false;
            }
            elem = queue.front();
            queue.pop();
            return true;
        }

        void Stop() {
            std::unique_lock<std::mutex> lock(mutex);
            requestEnd = true;
            conditionVariable.notify_one();
        }

        virtual ~SharedQueue() {
            Stop();
        }

    private:
        void End() {
            enqueue = false;
            while (!queue.empty()) {
                queue.pop();
            }
        }

        std::queue<T> queue;
        std::mutex mutex;
        std::condition_variable conditionVariable;
        bool requestEnd = false;
        bool enqueue = true;
    };
}

#endif //SIM_TO_DUT_INTERFACE_SHAREDQUEUE_H
