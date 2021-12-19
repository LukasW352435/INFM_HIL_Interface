/**
 * HIL - Periodic Timer
 * This an utility to create periodic timers for SimEvents
 *
 * Copyright (C) 2021 Michael Schmitz
 *
 * This file is part of "HIL - Periodic Timer".
 *
 * "HIL - Periodic Timer" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "HIL - Periodic Timer" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "HIL - Periodic Timer".  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Michael Schmitz
 * @version 1.0
 */

#include <chrono>
#include <utility>
#include "PeriodicTimer.h"
#include "../Interface_Logger/InterfaceLogger.h"

namespace sim_interface {
    PeriodicTimer::PeriodicTimer(const std::shared_ptr<boost::asio::io_service> &io, int periodMs,
                                 SimEvent event, std::function<void(const SimEvent &)> callback) :
            periodMs(periodMs), event(std::move(event)), callback(std::move(callback)),
            timer(*io) {
        timer.expires_from_now(boost::asio::chrono::milliseconds(periodMs));
    }

    /**
     * Start the async waiting on the constructed timer with tick() as callback
     */
    void PeriodicTimer::start() {
        timer.async_wait([&](boost::system::error_code e) { this->tick(e); });
    }

    /**
     * Cancel the constructed timer
     */
    void PeriodicTimer::stop() {
        timer.cancel();
    }

    /**
     * Called everytime the timer elapsed or an error is thrown
     * Calls the callback given when this PeriodicTimer was constructed with given SimEvent
     *
     * @param e boost error code (aborted timer)
     */
    void PeriodicTimer::tick(const boost::system::error_code &e) {
        if (e == boost::asio::error::operation_aborted) return;
        InterfaceLogger::logMessage(fmt::format("PeriodicTimer: Period elapsed for {}", event.operation),
                                    LOG_LEVEL::DEBUG);
        callback(event);
        timer.expires_from_now(boost::asio::chrono::milliseconds(periodMs));
        timer.async_wait([&](boost::system::error_code e) { this->tick(e); });
    }
}
