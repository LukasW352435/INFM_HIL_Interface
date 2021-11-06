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

#include "DefaultApiImpl.h"

#include <utility>
#include <iostream>
#include <curl/curl.h>

using namespace thi::dummy_dut;


namespace thi::dummy_dut::api_impl {
    /**
     * Construct all ApiResources and publish them on the restbed service.
     * Register all handlers with their ApiResources.
     * Starts the REST server.
     * @param port Port to start on
     * @param operations List of valid operations
     */
    DefaultApiImpl::DefaultApiImpl(int port, std::set<std::string> operations) : DefaultApi() {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        this->operations = std::move(operations);
        auto dashboardRes = std::make_shared<api::DefaultApiDashboardResource>();
        dashboardRes->set_handler_GET([this]() { return DefaultApiImpl::dashboardHandler(this); });
        dashboardRes->set_handler_POST([this](const std::shared_ptr<model::Message> &message) {
            return DefaultApiImpl::dashboardPostHandler(this, message);
        });
        this->m_spDefaultApiDashboardResource = dashboardRes;
        auto dashboardResMessages = std::make_shared<api::DefaultApiDashboardMessagesResource>();
        this->m_spDefaultApiDashboardMessagesResource = dashboardResMessages;

        auto readRes = std::make_shared<api::DefaultApiReadResource>();
        readRes->set_handler_POST(
                [this](const std::string &callback) { return DefaultApiImpl::addCallbackHandler(this, callback); });
        readRes->set_handler_DELETE(
                [this](const std::string &callback) { return DefaultApiImpl::removeCallbackHandler(this, callback); });
        this->m_spDefaultApiReadClientUrlResource = readRes;

        auto sendRes = std::make_shared<api::DefaultApiSendResource>();
        sendRes->set_handler_PUT([this](const std::shared_ptr<model::Message> &message) {
            return DefaultApiImpl::sendMessageHandler(this, message);
        });
        this->m_spDefaultApiSendResource = sendRes;


        startService(port);
    }

    /**
     * Wait for all curl threads to finish, free all global curl resources
     */
    DefaultApiImpl::~DefaultApiImpl() {
        for (auto &thread: curlThreads) {
            thread.join();
        }
        curl_slist_free_all(curlJsonHeader);
        curl_global_cleanup();
    }

    /**
     * Handler to construct the dashboard web page
     * @param instance Instance to call the handler on
     * @return Status code and HTML code (empty, if stub should set based on message defined in spec)
     */
    std::pair<int, std::string> DefaultApiImpl::dashboardHandler(DefaultApiImpl *instance) {
        return {200, instance->getDashboardPage()};
    }

    /**
     * Handler to receive message from dashboard (DuT) and send it to all registered callbacks (in new threads)
     * @param instance Instance to call the handler on
     * @param message Message received
     * @return Status code and HTML code (empty, if stub should set based on message defined in spec)
     */
    std::pair<int, std::string>
    DefaultApiImpl::dashboardPostHandler(DefaultApiImpl *instance, const std::shared_ptr<model::Message> &message) {
        if (message->getKey().empty() || message->getStatus().empty()) {
            return {400, ""};
        }
        std::cout << "Received message from dummy DuT: " << message->getKey() << ": " << message->getStatus()
                  << std::endl;
        auto messageJson = message->toJsonString(true);
        for (const auto &callback: instance->registeredInterfaces.getCallbacks()) {
            instance->curlThreads.emplace_back(
                    [instance, callback, messageJson] { instance->callUrl(callback, messageJson); });
        }
        return {201, ""};
    }

    /**
     * Handler to send Server-Sent Events (SSE) to all open sessions
     * @param instance Instance to call the handler on
     * @param message Message to send as SSE
     * @return Status code and HTML code (empty, if stub should set based on message defined in spec)
     */
    void DefaultApiImpl::dashboardMessageStreamHandler(messages::MessageWithTimestamp message) {
        const auto event = "data: " + message.toTableEntryWithoutNewline() + "\n\n";
#define SESSIONS this->m_spDefaultApiDashboardMessagesResource->sessions
        SESSIONS.erase(std::remove_if(SESSIONS.begin(), SESSIONS.end(),
                                      [](const std::shared_ptr<restbed::Session> &session) { return session->is_closed(); }),
                       SESSIONS.end());

        for (const auto &session: SESSIONS) {
            session->yield(event);
        }
    }

    /**
     * Handler to add an URL to registered callbacks
     * @param instance Instance to call the handler on
     * @param callback Callback to register
     * @return Status code and HTML code (empty, if stub should set based on message defined in spec)
     */
    std::pair<int, std::string>
    DefaultApiImpl::addCallbackHandler(DefaultApiImpl *instance, const std::string &callback) {
        if (callback.empty() || !(callback.starts_with("http://") || callback.starts_with("https://"))) {
            return {400, ""};
        }
        if (instance->registeredInterfaces.addInterfaceCallback(callback)) {
            return {200, ""};
        } else {
            return {208, ""};
        }
    }

    /**
     * Handler to remove an URL from registered callbacks
     * @param instance Instance to call the handler on
     * @param callback Callback to deregister
     * @return Status code and HTML code (empty, if stub should set based on message defined in spec)
     */
    std::pair<int, std::string>
    DefaultApiImpl::removeCallbackHandler(DefaultApiImpl *instance, const std::string &callback) {
        if (callback.empty() || !(callback.starts_with("http://") || callback.starts_with("https://"))) {
            return {400, ""};
        }
        if (instance->registeredInterfaces.removeInterfaceCallback(callback)) {
            return {200, ""};
        } else {
            return {404, ""};
        }
    }

    /**
     * Handler to receive messages from the interface
     * Sends the message over SSE stream and saves it to list
     * @param instance Instance to call the handler on
     * @param message Message that was received
     * @return Status code and HTML code (empty, if stub should set based on message defined in spec)
     */
    std::pair<int, std::string>
    DefaultApiImpl::sendMessageHandler(DefaultApiImpl *instance, const std::shared_ptr<model::Message> &message) {
        if (message->getKey().empty() || message->getStatus().empty()) {
            return {400, ""};
        }
        std::cout << "Received message from interface: " << message->getKey() << ": " << message->getStatus()
                  << std::endl;
        if (instance->operations.find(message->getKey())->empty()) {
            return {404, "Key not found"};
        }

        auto messageWithTimestamp = messages::MessageWithTimestamp(message.get());
        instance->dashboardMessageStreamHandler(messageWithTimestamp);
        instance->interfaceMessages.addMessage(messageWithTimestamp);
        return {200, ""};
    }

    /**
     * Send given message to given url with an timeout of 5sec
     * @param url URL to send message to
     * @param message JSON encoded message to send
     */
    void DefaultApiImpl::callUrl(const std::string &url, const std::string &message) {
        auto handle = curl_easy_init();

        long responseCode = 500;
        if (handle) {
            curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
            curl_easy_setopt(handle, CURLOPT_USERAGENT, "REST_Dummy_DuT");
            curl_easy_setopt(handle, CURLOPT_POSTFIELDS, message.c_str());
            curl_easy_setopt(handle, CURLOPT_HTTPHEADER, curlJsonHeader);
            curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "PUT");
            curl_easy_setopt(handle, CURLOPT_TIMEOUT, 5);

            curl_easy_perform(handle);
            curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &responseCode);
            curl_easy_cleanup(handle);
        }
        if (responseCode != 200) {
            std::cout << "Error on calling interface handler, HTTP-Code: " << responseCode << std::endl;
        }
    }

    /**
     * Construct a list of received messages in HTML
     * @return HTML snippet for messages
     */
    std::string DefaultApiImpl::getMessages() {
        auto messages = this->interfaceMessages.getMessages();

        std::string out;
        for (auto message: messages) {
            out += message.toTableEntry();
        }
        return out;
    }

    /**
     * Construct all options for the select dropdown to send messages
     * @return HTML snippet containg valid select options
     */
    std::string DefaultApiImpl::getOperationOptions() {
        std::string out;
        for (const auto &operation: operations) {
            out += R"V0G0N(
                    <option value=")V0G0N";
            out += operation;
            out += "\">";
            out += operation;
            out += "</option>";
        }
        return out;
    }

    /**
     * Construct the dashboard with all its functionality and already received messages
     * @return HTML web page
     */
    std::string DefaultApiImpl::getDashboardPage() {
        std::string page = R"V0G0N(<!DOCTYPE html>
<html>
<head>
    <title>REST Dummy DuT</title>
    <meta charset="UTF-8">
    <!-- Bootstrap 5 -->
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js" integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM" crossorigin="anonymous"></script>
    <script>
        function formDataToJson(form) {
            const json = {};
            for (const pair of new FormData(form)) {
                json[pair[0]] = pair[1];
            }
            return json;
        }

        async function submitForm(e, form) {
            e.preventDefault();

            const json = formDataToJson(form);

            const response = await fetch("/dashboard", { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(json) });
            if (response.status == 201) {
                addSubmitMessage("success", "Message sent");
            } else {
                addSubmitMessage("danger", "Error sending message");
            }
            setTimeout(removeSubmitMessage, 5000);
        }

        function addSubmitMessage(state, message) {
            const stateSubmit = document.getElementById("stateSubmit");
            stateSubmit.insertAdjacentHTML("beforeend", '<div class="alert alert-' + state + '" role="alert">' + message + '</div>');
        }

        function removeSubmitMessage() {
            const stateSubmit = document.getElementById("stateSubmit");
            stateSubmit.innerHTML = "";
        }

        function addMessage(data)
        {
            const tableBody = document.getElementById("messages");
            tableBody.insertAdjacentHTML("beforeend", data);
        }
    </script>
</head>
<body>
    <nav class="navbar navbar-fixed-top navbar-dark bg-dark mb-4">
        <span class="navbar-nav navbar-brand h1 text-light container-fluid">REST Dummy DutT</span>
    </nav>
    <form class="container-fluid" action="" id="sendForm">
        <label>Send a message</label>
        <div class="row">
            <div class="col">
                <select required name="key" class="form-select" aria-label="Operation select">
                    <option selected value="">Select operation</option>)V0G0N";
        page += getOperationOptions();
        page += R"V0G0N(
                </select>
            </div>
            <div class="col">
                <input required name="status" type="text" class="form-control" placeholder="Value" aria-label="Value">
            </div>
        </div>
        <div class="row p-2">
            <div class="col text-end">
                <button type="submit" class="btn btn-primary">Submit</button>
            </div>
        </div>
        <div class="row p-2">
            <div id="stateSubmit" class="col"></div>
        </div>
    </form>

    <div class="container-fluid">
        <table class="table">
            <thead>
                <tr>
                    <th scope="col">Operation</th>
                    <th scope="col">Value</th>
                    <th scope="col">Timestamp (UTC)</th>
                </tr>
            </thead>
            <tbody id="messages">)V0G0N";
        page += getMessages();
        page += R"V0G0N(
            </tbody>
        </table>
    </div>
    <script>
        const sendForm = document.querySelector("#sendForm");
        if (sendForm) {
            sendForm.addEventListener("submit", function(e) {
                submitForm(e, this);
            });
        }

        if ( !!window.EventSource )
	    {
	        var source = new EventSource( "dashboard/messages" );

            source.addEventListener( "message", function(evt) { addMessage( evt.data ); }, false );

            source.addEventListener( "open", function(evt) { console.info("Message stream opened"); }, false );

            source.addEventListener( 'error', function(evt) {
                if ( evt.readyState == EventSource.CLOSED ) {
                    console.error("Message stream closed");
                } }, false );
	    }
	    else
	    {
	        alert( "WebSockets NOT supported by your Browser!" );
	    }
    </script>
</body>
</html>)V0G0N";
        return page;
    }
}