/**
 * HIL - V2X Connector
 * This the connector to interface with the V2X DuT
 *
 * Copyright (C) 2021 Franziska Ihrler
 *
 * This file is part of "HIL - V2X Connector".
 *
 * "HIL - V2X Connector"" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "HIL - V2X Connector"" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "HIL - V2X Connector".  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Franziska Ihrler
 * @author Michael Schmitz
 * @version 1.0
 */

#include <utility>
#include <boost/asio.hpp>
#include "V2XConnector.h"
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include "EthernetPacket.h"
#include "../../DuTLogger/DuTLogger.h"

namespace sim_interface::dut_connector::v2x {
    V2XConnector::V2XConnector(std::shared_ptr<SharedQueue<SimEvent>> queueDuTToSim,
                               const V2XConnectorConfig &config)
            : DuTConnector(std::move(queueDuTToSim), config),
              _socket(ioService, boost::asio::generic::raw_protocol(AF_PACKET, SOCK_RAW)),
              receiveBuffer(2048, 0x00), ethernetFrameType(config.ethernetFrameType) {
        try {
            int index = getIfnameIndex(config.ifname);

            // Configure socket to receive all ethernet frames
            struct sockaddr_ll socket_address = {0};
            socket_address.sll_family = AF_PACKET;
            socket_address.sll_protocol = htons(ETH_P_ALL);
            socket_address.sll_ifindex = index;

            _socket.bind(boost::asio::generic::raw_protocol::endpoint(&socket_address, sizeof(sockaddr_ll)));
            receiveEndpoint = _socket.local_endpoint();
        } catch (std::exception &e) {
            DuTLogger::logMessage(fmt::format("V2XConnector: Exception on opening socket: {}", e.what()),
                                  LOG_LEVEL::ERROR);
        }
        startReceive();
        sockRunner = std::thread([&]() {
            this->ioService.run();
        });

    }

    int V2XConnector::getIfnameIndex(const std::string &ifname) {
        struct ifreq ifr{};
        memset(&ifr, 0, sizeof(ifr));
        strncpy(ifr.ifr_name, ifname.c_str(), sizeof(ifr.ifr_name));
        int s = socket(AF_INET, SOCK_STREAM, 0);
        ioctl(s, SIOCGIFINDEX, &ifr);
        close(s);
        DuTLogger::logMessage(
                fmt::format("V2XConnector: index of interface {}: {}", ifname, ifr.ifr_ifindex),
                INFO);
        return ifr.ifr_ifindex;
    }

    V2XConnector::~V2XConnector() {
        ioService.stop();
        sockRunner.join();
    }

    void V2XConnector::receiveCallback(const std::vector<unsigned char> &msg) {
        sendEventToSim(SimEvent("V2X", EthernetPacket(msg).getPacketAsArchive(), "V2X"));
    }

    void V2XConnector::startReceive() {
        _socket.async_receive_from(
                boost::asio::buffer(receiveBuffer),
                receiveEndpoint,
                [this](auto &&PH1, auto &&PH2) {
                    onReceive(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
                });
    }

    void V2XConnector::onReceive(const boost::system::error_code &ec, std::size_t read_bytes) {
        if (!ec) {
            if (read_bytes > 0) {
                DuTLogger::logMessage(fmt::format("V2XConnector: received {} bytes", read_bytes), LOG_LEVEL::DEBUG);
                std::vector<unsigned char> msg = std::vector(receiveBuffer.begin(), receiveBuffer.begin() + read_bytes);

                receiveCallback(msg);
            }
            startReceive();
        } else {
            DuTLogger::logMessage(fmt::format("V2XConnector: Got boost::system::error_code {}, stopping receive", ec),
                                  LOG_LEVEL::ERROR);
        }
    }

    void V2XConnector::handleEventSingle(const SimEvent &e) {
        EthernetPacket packet = EthernetPacket(boost::apply_visitor(EventVisitor(), e.value));
        auto bytes = packet.toBytes(ethernetFrameType);
        boost::asio::const_buffer buffer = boost::asio::buffer(bytes, bytes.size() * sizeof(unsigned char));
        std::size_t ret = _socket.send(buffer);
        if (ret == 0) {
            DuTLogger::logMessage("V2XConnector: Error sending over socket, no bytes send", LOG_LEVEL::ERROR);
        }
    }

    ConnectorInfo V2XConnector::getConnectorInfo() {
        return {
                "V2X Connector",
                0x0000001,
                " "};
    }
}
