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
        /**
         * Construct all ApiResources and publish them on the restbed service.
         * Register all handlers with their ApiResources.
         * Starts the REST server.
         * @param port Port to start on
         * @param operations List of valid operations
         */


        explicit DefaultApiImpl(int port, std::set<std::string> operations);

        /**
         * Wait for all curl threads to finish, free all global curl resources
         */
        ~DefaultApiImpl() override;

    private:
        /**
         * Handler to construct the dashboard web page
         * @param instance Instance to call the handler on
         * @return Status code and HTML code (empty, if stub should set based on message defined in spec)
         */
        static std::pair<int, std::string> dashboardHandler(DefaultApiImpl *instance);

        /**
         * Handler to receive message from dashboard (DuT) and send it to all registered callbacks (in new threads)
         * @param instance Instance to call the handler on
         * @param message Message received
         * @return Status code and HTML code (empty, if stub should set based on message defined in spec)
         */
        static std::pair<int, std::string>
        dashboardPostHandler(DefaultApiImpl *instance, const std::shared_ptr<model::Message> &message);

        /**
         * Handler to send Server-Sent Events (SSE) to all open sessions
         * @param instance Instance to call the handler on
         * @param message Message to send as SSE
         * @return Status code and HTML code (empty, if stub should set based on message defined in spec)
         */
        void dashboardMessageStreamHandler(dummy_dut::rest::messages::MessageWithTimestamp message);

        /**
        * Handler to add an URL to registered callbacks
        * @param instance Instance to call the handler on
        * @param callback Callback to register
        * @return Status code and HTML code (empty, if stub should set based on message defined in spec)
        */
        static std::pair<int, std::string> addCallbackHandler(DefaultApiImpl *instance, const std::string &callback);

        /**
         * Handler to remove an URL from registered callbacks
         * @param instance Instance to call the handler on
         * @param callback Callback to deregister
         * @return Status code and HTML code (empty, if stub should set based on message defined in spec)
         */
        static std::pair<int, std::string> removeCallbackHandler(DefaultApiImpl *instance, const std::string &callback);

        /**
         * Handler to receive messages from the interface
         * Sends the message over SSE stream and saves it to list
         * @param instance Instance to call the handler on
         * @param message Message that was received
         * @return Status code and HTML code (empty, if stub should set based on message defined in spec)
         */
        static std::pair<int, std::string>
        sendMessageHandler(DefaultApiImpl *instance, const std::shared_ptr<dummy_dut::rest::model::Message> &message);

        /**
         * Send given message to given url with an timeout of 5sec
         * @param url URL to send message to
         * @param message JSON encoded message to send
         */
        void callUrl(const std::string &url, const std::string &message);

        /**
         * Construct a list of received messages in HTML
         * @return HTML snippet for messages
         */
        std::string getMessages();

        /**
         * Construct all options for the select dropdown to send messages
         * @return HTML snippet containg valid select options
         */
        std::string getOperationOptions();

        /**
         * Construct the dashboard with all its functionality and already received messages
         * @return HTML web page
         */
        std::string getDashboardPage();

        std::set<std::string> operations;
        dummy_dut::rest::messages::InterfaceMessages interfaceMessages = {};
        dummy_dut::rest::impl::RegisteredInterface registeredInterfaces = {};
        struct curl_slist *curlJsonHeader = curl_slist_append(curlJsonHeader, "Content-Type: application/json");
        std::vector<std::thread> curlThreads = std::vector<std::thread>();
    };
}


#endif //DUMMY_REST_DUT_DEFAULTAPIIMPL_H
