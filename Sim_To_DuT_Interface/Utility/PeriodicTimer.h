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

#ifndef SIM_TO_DUT_INTERFACE_PERIODICTIMER_H
#define SIM_TO_DUT_INTERFACE_PERIODICTIMER_H

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include "../Events/SimEvent.h"

namespace sim_interface {
    class PeriodicTimer {
    public:
        explicit PeriodicTimer(const std::shared_ptr<boost::asio::io_service> &io, int periodMs, const SimEvent &event,
                               std::function<void(const SimEvent &)> callback);

        void start();
        void stop();

    private:
        void tick(const boost::system::error_code &e);

        boost::asio::steady_timer timer;
        std::function<void(const SimEvent &)> callback;
        const SimEvent &event;
        int periodMs;
    };
}


#endif //SIM_TO_DUT_INTERFACE_PERIODICTIMER_H
