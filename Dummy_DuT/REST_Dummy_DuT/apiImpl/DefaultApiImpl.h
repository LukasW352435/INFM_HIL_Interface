/**
 * HIL - REST Dummy DuT
 * This the implementation of the API defined in REST_Dummy_DuT.yaml
 *
 * Copyright (C) 2021 Michael Schmitz
 *
 * This file is part of "HIL - REST Dummy DuT".
 *
 * "HIL - REST Dummy DuT" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "HIL - REST Dummy DuT" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "HIL - REST Dummy DuT".  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Michael Schmitz
 * @version 1.0
 */

#ifndef DUMMY_REST_DUT_DEFAULTAPIIMPL_H
#define DUMMY_REST_DUT_DEFAULTAPIIMPL_H


#include <Message.h>
#include <string>
#include <curl/curl.h>
#include <thread>
#include "DefaultApi.h"
#include "../messages/MessageWithTimestamp.h"
#include "../messages/InterfaceMessages.h"
#include "RegisteredInterface.h"

namespace dummy_dut::rest::impl {
    class DefaultApiImpl : dummy_dut::rest::api::DefaultApi {
    public:
        explicit DefaultApiImpl(int port, std::set<std::string> operations);

        ~DefaultApiImpl() override;

    private:
        static std::pair<int, std::string> dashboardHandler(DefaultApiImpl *instance);

        static std::pair<int, std::string>
        dashboardPostHandler(DefaultApiImpl *instance, const std::shared_ptr<model::Message> &message);

        void dashboardMessageStreamHandler(dummy_dut::rest::messages::MessageWithTimestamp message);

        static std::pair<int, std::string> addCallbackHandler(DefaultApiImpl *instance, const std::string &callback);

        static std::pair<int, std::string> removeCallbackHandler(DefaultApiImpl *instance, const std::string &callback);

        static std::pair<int, std::string>
        sendMessageHandler(DefaultApiImpl *instance, const std::shared_ptr<dummy_dut::rest::model::Message> &message);

        void callUrl(const std::string &url, const std::string &message);

        std::string getMessages();

        std::string getOperationOptions();

        std::string getDashboardPage();

        std::set<std::string> operations;
        dummy_dut::rest::messages::InterfaceMessages interfaceMessages = {};
        dummy_dut::rest::impl::RegisteredInterface registeredInterfaces = {};
        struct curl_slist *curlJsonHeader = curl_slist_append(curlJsonHeader, "Content-Type: application/json");
        std::vector<std::thread> curlThreads = std::vector<std::thread>();
    };
}


#endif //DUMMY_REST_DUT_DEFAULTAPIIMPL_H
