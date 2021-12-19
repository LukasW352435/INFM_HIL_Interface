/**
 * Sim To DuT Interface
 *
 * Copyright (C) 2021 Lukas Wagenlehner
 *
 * This file is part of "Sim To DuT Interface".
 *
 * "Sim To DuT Interface" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Sim To DuT Interface" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Sim To DuT Interface".  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Lukas Wagenlehner
 * @version 1.0
 */

#ifndef SIM_TO_DUT_INTERFACE_SHAREDQUEUE_H
#define SIM_TO_DUT_INTERFACE_SHAREDQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

namespace sim_interface {
    /**
     * <summary>
     * A thread save shared queue to communicate between multiple threads.
     * The queue is a FIFO queue.
     * </summary>
     */
    template<class T>
    class SharedQueue {
    public:
        /**
         * Create a shared queue.
         */
        SharedQueue() = default;

        /**
         * Destroys the queue.
         */
        SharedQueue(const SharedQueue<T> &) = delete;

        /**
         * Push a element at the end of the queue.
         * @param elem Element to add to the queue.
         */
        void push(T elem) {
            std::unique_lock<std::mutex> lock(mutex);
            if (enqueue) {
                queue.push(elem);
                conditionVariable.notify_one();
            }
        }

        /**
         * Get the first element in the queue.
         * @param elem Reverence to the first element.
         * @return Returns TRUE if the element is valid and FALSE if not.
         */
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

        /**
         * Stops the queue and enqueues all its elements.
         */
        void Stop() {
            std::unique_lock<std::mutex> lock(mutex);
            requestEnd = true;
            conditionVariable.notify_one();
        }

        /**
         * Destroys and stops the queue.
         */
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
