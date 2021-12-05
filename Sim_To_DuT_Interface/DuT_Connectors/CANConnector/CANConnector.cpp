/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      CANConnector.cpp
 \brief     The Connector enables the communication over a CAN/CANFD interface.
            It builds upon the socketCAN BCM socket and boost::asio.
 \author    Matthias Bank
 \version   1.0.0
 \date      12.11.2021
 ******************************************************************************/


/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
// Project includes
#include "CANConnector.h"

// System includes


/*******************************************************************************
 * FUNCTION DEFINITIONS
 ******************************************************************************/
namespace sim_interface::dut_connector::can{

    CANConnector::CANConnector(
            std::shared_ptr<SharedQueue<SimEvent>> queueDuTToSim,
            const CANConnectorConfig &config):
            DuTConnector(std::move(queueDuTToSim), config),
            ioContext(boost::make_shared<boost::asio::io_context>()),
            bcmSocket(createBcmSocket(config)),
            config(config){

        // Create all receive operations
        for(auto const& [canID, receiveOperation] : config.frameToOperation){

            // Check if the receive operation has a mask
            if(receiveOperation.hasMask){

                // Set the CAN ID in the mask
                struct canfd_frame mask = receiveOperation.mask;
                mask.can_id = canID;

                // Create the receive operation
                rxSetupMask(canID, mask, receiveOperation.isCANFD);

            }else{

                // Create the receive operation
                rxSetupCanID(canID, receiveOperation.isCANFD);
            }

        }
        DuTLogger::logMessage("CAN Connector: Created initial RX setup", LOG_LEVEL::INFO);

        // Start the receive loop on the socket
        receiveOnSocket();

        // Start the io context loop
        startProcessing();

        DuTLogger::logMessage("CAN Connector: CAN Connector created", LOG_LEVEL::INFO);
    }

    CANConnector::~CANConnector(){

        // Stop the io context loop
        stopProcessing();

        DuTLogger::logMessage("CAN Connector: CAN Connector destroyed", LOG_LEVEL::INFO);
    }

    boost::asio::generic::datagram_protocol::socket CANConnector::createBcmSocket(const CANConnectorConfig &connectorConfig){

        // Error code return value
        boost::system::error_code errorCode;

        // Define Address family and protocol
        boost::asio::generic::datagram_protocol bcmProtocol(PF_CAN, CAN_BCM);

        // Create a BCM socket
        boost::asio::generic::datagram_protocol::socket socket(*ioContext, bcmProtocol);

        // Create an I/O command and resolve the interface name to an interface index
        InterfaceIndexIO interfaceIndexIO(connectorConfig.interfaceName);
        socket.io_control(interfaceIndexIO, errorCode);

        // Check if we could resolve the interface correctly
        if(errorCode){
            DuTLogger::logMessage("CAN Connector: An error occurred on the io control operation: " + errorCode.message(), LOG_LEVEL::ERROR);
            throw std::invalid_argument("CAN Connector: Could not resolve the interface name correctly");
        }

        // Connect the socket
        sockaddr_can addr = {0};
        addr.can_family   = AF_CAN;
        addr.can_ifindex  = interfaceIndexIO.index();

        boost::asio::generic::datagram_protocol::endpoint bcmEndpoint{&addr, sizeof(addr)};
        socket.connect(bcmEndpoint, errorCode);

        // Check if we could connect correctly
        if(errorCode){
            DuTLogger::logMessage("CAN Connector: An error occurred on the connect operation: " + errorCode.message(), LOG_LEVEL::ERROR);
            throw std::runtime_error("CAN Connector: Could not connect to the interface");
        }

        // Note: In contrast to a raw CAN socket there is no need to
        // explicitly enable CANFD for an BCM socket with setsockopt!

        return socket;
    }

    void CANConnector::startProcessing(){

        // Run the io context in its own thread
        ioContextThread = std::thread(&CANConnector::ioContextThreadFunction, this, std::ref(ioContext));

        DuTLogger::logMessage("CAN Connector: Starting the io context loop", LOG_LEVEL::INFO);
    }

    void CANConnector::stopProcessing(){

        // Check if we need to stop the io context loop
        if(!ioContext->stopped()){
            ioContext->stop();
        }

        // Join the io context loop thread
        if(ioContextThread.joinable()){
            ioContextThread.join();
        }else{
            DuTLogger::logMessage("CAN Connector: ioContextThread was not joinable", LOG_LEVEL::ERROR);
        }

        DuTLogger::logMessage("CAN Connector: Stopped the io context loop", LOG_LEVEL::INFO);
    }

    void CANConnector::ioContextThreadFunction(const boost::shared_ptr<boost::asio::io_context>& context){
        context->run();
    }

    ConnectorInfo CANConnector::getConnectorInfo(){

        ConnectorInfo info(
                "CAN Connector",
                0x0000001,
                "The CAN Connector enables the communication over a CAN/CANFD interface.");

        return info;
    }

    void CANConnector::receiveOnSocket(){

        DuTLogger::logMessage("CAN Connector: Creating a new receive operation on the socket", LOG_LEVEL::INFO);

        // Create an async receive operation on the BCM socket
        bcmSocket.async_receive(boost::asio::buffer(rxBuffer),
                                [this](boost::system::error_code errorCode, std::size_t receivedBytes){

            // Lambda completion function for the async receive operation

            // Check the error code of the operation
            if(!errorCode){

                DuTLogger::logMessage("CAN Connector: Received " + std::to_string(receivedBytes) + " bytes on the socket", LOG_LEVEL::INFO);

                // We need to receive at least a whole bcm_msg_head
                if(receivedBytes >= sizeof(bcm_msg_head)){

                    // Get the bcm_msg_head
                    const auto* head = reinterpret_cast<const bcm_msg_head*>(rxBuffer.data());

                    // Check if the message contains CAN or CANFD frames
                    bool isCANFD = false;

                    if(head->flags & CAN_FD_FRAME){
                        isCANFD = true;
                    }

                    // Calculate the expected size in bytes of the whole
                    // message based upon the bcm_msg_head information.
                    size_t expectedBytes = 0;

                    if(isCANFD){
                        expectedBytes = head->nframes * sizeof(canfd_frame) + sizeof(bcm_msg_head);
                    }else{
                        expectedBytes = head->nframes * sizeof(can_frame)   + sizeof(bcm_msg_head);
                    }

                    // Check if we received the whole message
                    if(receivedBytes == expectedBytes){

                        // Get the pointer to the frames and call the next function to process the data
                        auto frames = reinterpret_cast<void *>(rxBuffer.data() + sizeof(bcm_msg_head));
                        handleReceivedData(head, frames, head->nframes, isCANFD);

                    }else{
                        DuTLogger::logMessage("CAN Connector: The expected amount of bytes is not equal to the received bytes", LOG_LEVEL::ERROR);
                    }

                }

            }else{
                DuTLogger::logMessage("CAN Connector: An error occurred on the async receive operation: " + errorCode.message(), LOG_LEVEL::ERROR);
            }

            // Create the next receive operation
            receiveOnSocket();

        });

    }

    void CANConnector::txSendSingleFrame(struct canfd_frame frame, bool isCANFD){

        // BCM message we are sending with a single CAN or CANFD frame
        std::shared_ptr<void> msg = nullptr;
        size_t msgSize = 0;

        // Check if we are sending CAN or CANFD frames
        // and create the according struct
        if(isCANFD){
            msgSize = sizeof(struct bcmMsgSingleFrameCanFD);
            auto msgCANFD = std::make_shared<bcmMsgSingleFrameCanFD>();
            msg = std::reinterpret_pointer_cast<void>(msgCANFD);
        }else {
            msgSize = sizeof(struct bcmMsgSingleFrameCan);
            auto msgCAN = std::make_shared<bcmMsgSingleFrameCan>();
            msg = std::reinterpret_pointer_cast<void>(msgCAN);
        }

        // Error handling / Sanity check
        if(msg == nullptr){
            DuTLogger::logMessage("CAN Connector: Could not make the message structure", LOG_LEVEL::ERROR);
            return;
        }

        // Fill out the message
        if(isCANFD){
            std::shared_ptr<bcmMsgSingleFrameCanFD> msgCANFD = std::reinterpret_pointer_cast<bcmMsgSingleFrameCanFD>(msg);

            msgCANFD->msg_head.opcode  = TX_SEND;
            msgCANFD->msg_head.flags   = CAN_FD_FRAME;
            msgCANFD->msg_head.can_id  = frame.can_id;
            msgCANFD->msg_head.nframes = 1;
            msgCANFD->canfdFrame[0]    = frame;
        }else{
            std::shared_ptr<bcmMsgSingleFrameCan> msgCAN = std::reinterpret_pointer_cast<bcmMsgSingleFrameCan>(msg);
            auto canFrame = (struct can_frame *) &frame;

            msgCAN->msg_head.opcode    = TX_SEND;
            msgCAN->msg_head.can_id    = canFrame->can_id;
            msgCAN->msg_head.nframes   = 1;
            msgCAN->canFrame[0]        = *canFrame;
        }

        // Note: buffer doesn't accept smart pointers. Need to use a regular pointer.
        boost::asio::const_buffer buffer = boost::asio::buffer(msg.get(), msgSize);

        // Note: Must guarantee the validity of the argument until the handler is invoked.
        // We guarantee the validity through the lambda capture of the smart pointer.

        // Note: The TX_SEND operation can only handle exactly one frame!
        bcmSocket.async_send(buffer, [msg](boost::system::error_code errorCode, std::size_t size){

            // Lambda completion function for the async TX_SEND operation

            // Check boost asio error code
            if(!errorCode){
                DuTLogger::logMessage("CAN Connector: TX_SEND completed successfully", LOG_LEVEL::INFO);
            }else{
                DuTLogger::logMessage("CAN Connector: TX_SEND failed: " + errorCode.message(), LOG_LEVEL::ERROR);
            }

        });

    }

    void CANConnector::txSendMultipleFrames(struct canfd_frame frames[], int nframes, bool isCANFD){

        // Note: The TX_SEND operation can only handle exactly one frame!
        // That's why we should use this wrapper for multiple frames.
        for(int index = 0; index < nframes; index++){
            txSendSingleFrame(frames[index], isCANFD);
        }

    }

    void CANConnector::txSetupSingleFrame(struct canfd_frame frame, uint32_t count, struct bcm_timeval ival1,
                                          struct bcm_timeval ival2, bool isCANFD){

        // BCM message we are sending with a single CAN or CANFD frame
        std::shared_ptr<void> msg = nullptr;
        size_t msgSize = 0;

        // Check if we are sending CAN or CANFD frames
        // and create the according struct
        if(isCANFD){
            msgSize = sizeof(struct bcmMsgSingleFrameCanFD);
            auto msgCANFD = std::make_shared<bcmMsgSingleFrameCanFD>();
            msg = std::reinterpret_pointer_cast<void>(msgCANFD);
        }else{
            msgSize = sizeof(struct bcmMsgSingleFrameCan);
            auto msgCAN = std::make_shared<bcmMsgSingleFrameCan>();
            msg = std::reinterpret_pointer_cast<void>(msgCAN);
        }

        // Error handling / Sanity check
        if(msg == nullptr){
            DuTLogger::logMessage("CAN Connector: Could not make the message structure", LOG_LEVEL::ERROR);
            return;
        }

        // Note: By combining the flags SETTIMER and STARTTIMER
        // the BCM will start sending the messages immediately

        // Fill out the message
        if(isCANFD){
            std::shared_ptr<bcmMsgSingleFrameCanFD> msgCANFD = std::reinterpret_pointer_cast<bcmMsgSingleFrameCanFD>(msg);

            msgCANFD->msg_head.opcode  = TX_SETUP;
            msgCANFD->msg_head.flags   = CAN_FD_FRAME | SETTIMER | STARTTIMER;
            msgCANFD->msg_head.can_id  = frame.can_id;
            msgCANFD->msg_head.count   = count;
            msgCANFD->msg_head.ival1   = ival1;
            msgCANFD->msg_head.ival2   = ival2;
            msgCANFD->msg_head.nframes = 1;
            msgCANFD->canfdFrame[0]    = frame;
        }else{
            std::shared_ptr<bcmMsgSingleFrameCan> msgCAN = std::reinterpret_pointer_cast<bcmMsgSingleFrameCan>(msg);
            auto canFrame = (struct can_frame *) &frame;

            msgCAN->msg_head.opcode    = TX_SETUP;
            msgCAN->msg_head.flags     = SETTIMER | STARTTIMER;
            msgCAN->msg_head.can_id    = canFrame->can_id;
            msgCAN->msg_head.count     = count;
            msgCAN->msg_head.ival1     = ival1;
            msgCAN->msg_head.ival2     = ival2;
            msgCAN->msg_head.nframes   = 1;
            msgCAN->canFrame[0]        = *canFrame;
        }

        // Note: buffer doesn't accept smart pointers. Need to use a regular pointer.
        boost::asio::const_buffer buffer = boost::asio::buffer(msg.get(), msgSize);

        bcmSocket.async_send(buffer, [msg](boost::system::error_code errorCode, std::size_t size){

            // Lambda completion function for the async TX_SETUP operation

            // Check boost asio error code
            if(!errorCode){
                DuTLogger::logMessage("CAN Connector: TX_SETUP completed successfully", LOG_LEVEL::INFO);
            }else{
                DuTLogger::logMessage("CAN Connector: TX_SETUP failed: " + errorCode.message(), LOG_LEVEL::ERROR);
            }

        });

    }

    void CANConnector::txSetupMultipleFrames(struct canfd_frame frames[], int nframes, uint32_t count[], struct bcm_timeval ival1[],
                                             struct bcm_timeval ival2[], bool isCANFD){

        for(int index = 0; index < nframes; index++){
            txSetupSingleFrame(frames[index], count[index], ival1[index], ival2[index], isCANFD);
        }

    }

    void CANConnector::txSetupSequence(struct canfd_frame frames[], int nframes, uint32_t count,
                                       struct bcm_timeval ival1, struct bcm_timeval ival2, bool isCANFD){

        // BCM message we are sending with a single CAN or CANFD frame
        std::shared_ptr<void> msg = nullptr;
        size_t msgSize = 0;

        // Check if we are sending CAN or CANFD frames
        // and create the according struct
        if(isCANFD){
            msgSize = sizeof(struct bcmMsgMultipleFramesCanFD);
            auto msgCANFD = std::make_shared<bcmMsgMultipleFramesCanFD>();
            msg = std::reinterpret_pointer_cast<void>(msgCANFD);
        }else {
            msgSize = sizeof(struct bcmMsgMultipleFramesCan);
            auto msgCAN = std::make_shared<bcmMsgMultipleFramesCan>();
            msg = std::reinterpret_pointer_cast<void>(msgCAN);
        }

        // Error handling / Sanity check
        if(msg == nullptr){
            DuTLogger::logMessage("CAN Connector: Could not make the message structure", LOG_LEVEL::ERROR);
            return;
        }

        // Note: By combining the flags SETTIMER and STARTTIMER
        // the BCM will start sending the messages immediately

        // Fill out the message
        if(isCANFD){
            std::shared_ptr<bcmMsgMultipleFramesCanFD> msgCANFD = std::reinterpret_pointer_cast<bcmMsgMultipleFramesCanFD>(msg);

            msgCANFD->msg_head.opcode  = TX_SETUP;
            msgCANFD->msg_head.flags   = CAN_FD_FRAME | SETTIMER | STARTTIMER;
            msgCANFD->msg_head.can_id  = frames[0].can_id;
            msgCANFD->msg_head.count   = count;
            msgCANFD->msg_head.ival1   = ival1;
            msgCANFD->msg_head.ival2   = ival2;
            msgCANFD->msg_head.nframes = nframes;

            size_t arrSize = sizeof(struct canfd_frame) * nframes;
            std::memcpy(msgCANFD->canfdFrames, frames, arrSize);
        }else{
            std::shared_ptr<bcmMsgMultipleFramesCan> msgCAN = std::reinterpret_pointer_cast<bcmMsgMultipleFramesCan>(msg);
            auto firstCanFrame = (struct can_frame*) &frames[0];

            msgCAN->msg_head.opcode    = TX_SETUP;
            msgCAN->msg_head.flags     = SETTIMER | STARTTIMER;
            msgCAN->msg_head.can_id    = firstCanFrame->can_id;
            msgCAN->msg_head.count     = count;
            msgCAN->msg_head.ival1     = ival1;
            msgCAN->msg_head.ival2     = ival2;
            msgCAN->msg_head.nframes   = nframes;

            for(int index = 0; index < nframes; index++){
                auto canFrame = (struct can_frame*) &frames[index];
                msgCAN->canFrames[index] = *canFrame;
            }
        }

        // Note: buffer doesn't accept smart pointers. Need to use a regular pointer.
        boost::asio::const_buffer buffer = boost::asio::buffer(msg.get(), msgSize);

        bcmSocket.async_send(buffer, [msg](boost::system::error_code errorCode, std::size_t size){

            // Lambda completion function for the async TX_SETUP operation

            // Check boost asio error code
            if(!errorCode){
                DuTLogger::logMessage("CAN Connector: TX_SETUP (sequence) completed successfully", LOG_LEVEL::INFO);
            }else{
                DuTLogger::logMessage("CAN Connector: TX_SETUP (sequence) failed: " + errorCode.message(), LOG_LEVEL::ERROR);
            }

        });

    }

    void CANConnector::txSetupUpdateSingleFrame(struct canfd_frame frame, bool isCANFD, bool announce){

        // BCM message we are sending with a single CAN or CANFD frame
        std::shared_ptr<void> msg = nullptr;
        size_t msgSize = 0;

        // Check if we are sending CAN or CANFD frames
        // and create the according struct
        if(isCANFD){
            msgSize = sizeof(struct bcmMsgSingleFrameCanFD);
            auto msgCANFD = std::make_shared<bcmMsgSingleFrameCanFD>();
            msg = std::reinterpret_pointer_cast<void>(msgCANFD);
        }else{
            msgSize = sizeof(struct bcmMsgSingleFrameCan);
            auto msgCAN = std::make_shared<bcmMsgSingleFrameCan>();
            msg = std::reinterpret_pointer_cast<void>(msgCAN);
        }

        // Error handling / Sanity check
        if(msg == nullptr){
            DuTLogger::logMessage("CAN Connector: Could not make the message structure", LOG_LEVEL::ERROR);
            return;
        }

        // Note: By combining the flags SETTIMER and STARTTIMER
        // the BCM will start sending the messages immediately

        // Fill out the message
        if(isCANFD){
            std::shared_ptr<bcmMsgSingleFrameCanFD> msgCANFD = std::reinterpret_pointer_cast<bcmMsgSingleFrameCanFD>(msg);

            msgCANFD->msg_head.opcode  = TX_SETUP;
            msgCANFD->msg_head.flags   = CAN_FD_FRAME;
            msgCANFD->msg_head.can_id  = frame.can_id;
            msgCANFD->msg_head.nframes = 1;
            msgCANFD->canfdFrame[0]    = frame;

            if(announce){
                msgCANFD->msg_head.flags = msgCANFD->msg_head.flags | TX_ANNOUNCE;
            }

        }else{
            std::shared_ptr<bcmMsgSingleFrameCan> msgCAN = std::reinterpret_pointer_cast<bcmMsgSingleFrameCan>(msg);
            auto canFrame = (struct can_frame *) &frame;

            msgCAN->msg_head.opcode    = TX_SETUP;
            msgCAN->msg_head.can_id    = canFrame->can_id;
            msgCAN->msg_head.nframes   = 1;
            msgCAN->canFrame[0]        = *canFrame;

            if(announce){
                msgCAN->msg_head.flags = TX_ANNOUNCE;
            }

        }

        // Note: buffer doesn't accept smart pointers. Need to use a regular pointer.
        boost::asio::const_buffer buffer = boost::asio::buffer(msg.get(), msgSize);

        bcmSocket.async_send(buffer, [msg](boost::system::error_code errorCode, std::size_t size){

            // Lambda completion function for the async TX_SETUP operation

            // Check boost asio error code
            if(!errorCode){
                DuTLogger::logMessage("CAN Connector: TX_SETUP (update) completed successfully", LOG_LEVEL::INFO);
            }else{
                DuTLogger::logMessage("CAN Connector: TX_SETUP (update) failed: " + errorCode.message(), LOG_LEVEL::ERROR);
            }

        });

    }

    void CANConnector::txSetupUpdateMultipleFrames(struct canfd_frame frames[], int nframes, bool isCANFD, bool announce){

        for(int index = 0; index < nframes; index++){
            txSetupUpdateSingleFrame(frames[index], isCANFD, announce);
        }

    }

    void CANConnector::txDelete(canid_t canID, bool isCANFD){

        // BCM message we are sending
        auto msg = std::make_shared<bcm_msg_head>();

        // Error handling / Sanity check
        if(msg == nullptr){
            DuTLogger::logMessage("CAN Connector: Could not make the message structure", LOG_LEVEL::ERROR);
            return;
        }

        // Fill out the message
        msg->opcode = TX_DELETE;
        msg->can_id = canID;

        if(isCANFD){
            msg->flags = CAN_FD_FRAME;
        }

        // Note: buffer doesn't accept smart pointers. Need to use a regular pointer.
        boost::asio::const_buffer buffer = boost::asio::buffer(msg.get(), sizeof(bcm_msg_head));

        bcmSocket.async_send(buffer, [msg](boost::system::error_code errorCode, std::size_t size){

            // Lambda completion function for the async TX_DELETE operation

            // Check boost asio error code
            if(!errorCode){
                DuTLogger::logMessage("CAN Connector: TX_DELETE completed successfully", LOG_LEVEL::INFO);
            }else{
                DuTLogger::logMessage("CAN Connector: TX_DELETE failed: " + errorCode.message(), LOG_LEVEL::ERROR);
            }

        });

    }

    void CANConnector::rxSetupCanID(canid_t canID, bool isCANFD){

        // BCM message we are sending
        auto msg = std::make_shared<bcm_msg_head>();

        // Error handling / Sanity check
        if(msg == nullptr){
            DuTLogger::logMessage("CAN Connector: Could not make the message structure", LOG_LEVEL::ERROR);
            return;
        }

        // Fill out the message
        msg->opcode = RX_SETUP;
        msg->flags  = RX_FILTER_ID;
        msg->can_id = canID;

        if(isCANFD){
            msg->flags = msg->flags | CAN_FD_FRAME;
        }

        // Note: buffer doesn't accept smart pointers. Need to use a regular pointer.
        boost::asio::const_buffer buffer = boost::asio::buffer(msg.get(), sizeof(bcm_msg_head));

        bcmSocket.async_send(buffer, [msg](boost::system::error_code errorCode, std::size_t size){

            // Lambda completion function for the async RX_SETUP operation

            // Check boost asio error code
            if(!errorCode){
                DuTLogger::logMessage("CAN Connector: RX_SETUP (CAN ID) completed successfully", LOG_LEVEL::INFO);
            }else{
                DuTLogger::logMessage("CAN Connector: RX_SETUP (CAN ID) failed: " + errorCode.message(), LOG_LEVEL::ERROR);
            }

        });

    }

    void CANConnector::rxSetupMask(canid_t canID, struct canfd_frame mask, bool isCANFD){

        // BCM message we are sending with a single CAN or CANFD frame
        std::shared_ptr<void> msg = nullptr;
        size_t msgSize = 0;

        // Check if we are sending CAN or CANFD frames
        // and create the according struct
        if(isCANFD){
            msgSize = sizeof(struct bcmMsgSingleFrameCanFD);
            auto msgCANFD = std::make_shared<bcmMsgSingleFrameCanFD>();
            msg = std::reinterpret_pointer_cast<void>(msgCANFD);
        }else {
            msgSize = sizeof(struct bcmMsgSingleFrameCan);
            auto msgCAN = std::make_shared<bcmMsgSingleFrameCan>();
            msg = std::reinterpret_pointer_cast<void>(msgCAN);
        }

        // Error handling / Sanity check
        if(msg == nullptr){
            DuTLogger::logMessage("CAN Connector: Could not make the message structure", LOG_LEVEL::ERROR);
            return;
        }

        // Fill out the message
        if(isCANFD){
            std::shared_ptr<bcmMsgSingleFrameCanFD> msgCANFD = std::reinterpret_pointer_cast<bcmMsgSingleFrameCanFD>(msg);

            msgCANFD->msg_head.opcode  = RX_SETUP;
            msgCANFD->msg_head.flags   = CAN_FD_FRAME;
            msgCANFD->msg_head.can_id  = canID;
            msgCANFD->msg_head.nframes = 1;

            msgCANFD->canfdFrame[0]    = mask;
        }else{
            std::shared_ptr<bcmMsgSingleFrameCan> msgCAN = std::reinterpret_pointer_cast<bcmMsgSingleFrameCan>(msg);
            auto maskCAN = (struct can_frame*) &mask;

            msgCAN->msg_head.opcode    = RX_SETUP;
            msgCAN->msg_head.can_id    = canID;
            msgCAN->msg_head.nframes   = 1;

            msgCAN->canFrame[0]        = *maskCAN;
        }

        // Note: buffer doesn't accept smart pointers. Need to use a regular pointer.
        boost::asio::const_buffer buffer = boost::asio::buffer(msg.get(), msgSize);

        bcmSocket.async_send(buffer, [msg](boost::system::error_code errorCode, std::size_t size){

            // Lambda completion function for the async RX_SETUP operation

            // Check boost asio error code
            if(!errorCode){
                DuTLogger::logMessage("CAN Connector: RX_SETUP (mask) completed successfully", LOG_LEVEL::INFO);
            }else{
                DuTLogger::logMessage("CAN Connector: RX_SETUP (mask) failed: " + errorCode.message(), LOG_LEVEL::INFO);
            }

        });
    }

    void CANConnector::rxDelete(canid_t canID, bool isCANFD){

        // BCM message we are sending
        auto msg = std::make_shared<bcm_msg_head>();

        // Error handling / Sanity check
        if(msg == nullptr){
            DuTLogger::logMessage("CAN Connector: Could not make the message structure", LOG_LEVEL::ERROR);
            return;
        }

        // Fill out the message
        msg->opcode = RX_DELETE;
        msg->can_id = canID;

        if(isCANFD){
            msg->flags = CAN_FD_FRAME;
        }

        // Note: buffer doesn't accept smart pointers. Need to use a regular pointer.
        boost::asio::const_buffer buffer = boost::asio::buffer(msg.get(), sizeof(bcm_msg_head));

        bcmSocket.async_send(buffer, [msg](boost::system::error_code errorCode, std::size_t size){

            // Lambda completion function for the async TX_DELETE operation

            // Check boost asio error code
            if(!errorCode){
                DuTLogger::logMessage("CAN Connector: RX_DELETE completed successfully", LOG_LEVEL::INFO);
            }else{
                DuTLogger::logMessage("CAN Connector: RX_DELETE failed: " + errorCode.message(), LOG_LEVEL::ERROR);
            }

        });

    }

    void CANConnector::handleReceivedData(const bcm_msg_head *head, void *frames, uint32_t nframes, bool isCANFD){

        DuTLogger::logMessage("CAN Connector: Handling the received data", LOG_LEVEL::INFO);

        switch(head->opcode){

            case RX_CHANGED:

                // Simple reception of a CAN/CANFD frame or a content change occurred.
                // TODO: Implement handling
                DuTLogger::logMessage("CAN Connector: RX_CHANGED handling is not implemented", LOG_LEVEL::WARNING);
                break;

            case RX_TIMEOUT:

                // Cyclic message is detected to be absent by the timeout monitoring.
                // There is no function implemented yet that should cause/use this.
                DuTLogger::logMessage("CAN Connector: RX_TIMEOUT handling is not implemented", LOG_LEVEL::WARNING);
                break;

            case TX_EXPIRED:

                // Notification when counter finishes sending at ival1 interval.
                // Requires TX_COUNTEVT flag to be set at TX_SETUP.
                // There is no function implemented yet that should cause/use this.
                DuTLogger::logMessage("CAN Connector: TX_EXPIRED handling is not implemented", LOG_LEVEL::WARNING);
                break;

            case RX_STATUS:

                // Reply to a RX_READ request that returns the RX content filter properties for a given CAN ID.
                // This should not occur on our regular receive loop.
                DuTLogger::logMessage("CAN Connector: Received unexpected RX_STATUS message", LOG_LEVEL::WARNING);
                break;

            case TX_STATUS:

                // Reply to a TX_READ request that returns the TX transmission properties for a given CAN ID.
                // This should not occur on our regular receive loop.
                DuTLogger::logMessage("CAN Connector: Received unexpected TX_STATUS message", LOG_LEVEL::WARNING);
                break;

            default:

                DuTLogger::logMessage("CAN Connector: Received unknown opcode", LOG_LEVEL::WARNING);
        }

    }

    void CANConnector::handleEventSingle(const SimEvent &event){

        // Test CAN Frame
        struct can_frame canFrame1 = {0};
        canFrame1.can_id  = 0x123;
        canFrame1.can_dlc = 4;
        canFrame1.data[0] = 0xDE;
        canFrame1.data[1] = 0xAD;
        canFrame1.data[2] = 0xBE;
        canFrame1.data[3] = 0xEF;

        struct can_frame canFrame2 = {0};
        canFrame2.can_id  = 0x345;
        canFrame2.can_dlc = 3;
        canFrame2.data[0] = 0xC0;
        canFrame2.data[1] = 0xFF;
        canFrame2.data[2] = 0xEE;

        // CANFD frame array containing CAN frames
        struct canfd_frame frameArrCAN[2];
        frameArrCAN[0] = *((struct canfd_frame*) &canFrame1);
        frameArrCAN[1] = *((struct canfd_frame*) &canFrame2);

        struct can_frame canFrame1Modified = {0};
        canFrame1Modified.can_id  = 0x123;
        canFrame1Modified.can_dlc = 4;
        canFrame1Modified.data[0] = 0xBE;
        canFrame1Modified.data[1] = 0xEF;
        canFrame1Modified.data[2] = 0xDE;
        canFrame1Modified.data[3] = 0xAD;

        struct can_frame canFrame2Modified = {0};
        canFrame2Modified.can_id  = 0x345;
        canFrame2Modified.can_dlc = 5;
        canFrame2Modified.data[0] = 0xC0;
        canFrame2Modified.data[1] = 0xFF;
        canFrame2Modified.data[2] = 0xEE;
        canFrame2Modified.data[3] = 0xFF;
        canFrame2Modified.data[4] = 0xEE;

        // CANFD frame array containing CAN frames with modified data
        struct canfd_frame frameArrCANModified[2];
        frameArrCANModified[0] = *((struct canfd_frame*) &canFrame1Modified);
        frameArrCANModified[1] = *((struct canfd_frame*) &canFrame2Modified);

        // Test CANFD Frame
        struct canfd_frame canfdFrame1 = {0};
        canfdFrame1.can_id   = 0x567;
        canfdFrame1.len      = 16;
        canfdFrame1.data[0]  = 0xDE;
        canfdFrame1.data[1]  = 0xAD;
        canfdFrame1.data[2]  = 0xBE;
        canfdFrame1.data[3]  = 0xEF;
        canfdFrame1.data[4]  = 0xDE;
        canfdFrame1.data[5]  = 0xAD;
        canfdFrame1.data[6]  = 0xBE;
        canfdFrame1.data[7]  = 0xEF;
        canfdFrame1.data[8]  = 0xDE;
        canfdFrame1.data[9]  = 0xAD;
        canfdFrame1.data[10] = 0xBE;
        canfdFrame1.data[11] = 0xEF;
        canfdFrame1.data[12] = 0xDE;
        canfdFrame1.data[13] = 0xAD;
        canfdFrame1.data[14] = 0xBE;
        canfdFrame1.data[15] = 0xEF;

        struct canfd_frame canfdFrame2 = {0};
        canfdFrame2.can_id   = 0x789;
        canfdFrame2.len      = 12;
        canfdFrame2.data[0]  = 0xC0;
        canfdFrame2.data[1]  = 0xFF;
        canfdFrame2.data[2]  = 0xEE;
        canfdFrame2.data[3]  = 0xC0;
        canfdFrame2.data[4]  = 0xFF;
        canfdFrame2.data[5]  = 0xEE;
        canfdFrame2.data[6]  = 0xC0;
        canfdFrame2.data[7]  = 0xFF;
        canfdFrame2.data[8]  = 0xEE;
        canfdFrame2.data[9]  = 0xC0;
        canfdFrame2.data[10] = 0xFF;
        canfdFrame2.data[11] = 0xEE;

        // CANFD frame array
        struct canfd_frame frameArrCANFD[2];
        frameArrCANFD[0] = canfdFrame1;
        frameArrCANFD[1] = canfdFrame2;

        // Test intervals
        struct bcm_timeval ival1 = {0};
        ival1.tv_sec  = 0;
        ival1.tv_usec = 500;

        struct bcm_timeval ival2 = {0};
        ival2.tv_sec  = 1;
        ival2.tv_usec = 0;

        struct bcm_timeval ivalZero = {0};

        struct bcm_timeval ival1Arr[2];
        ival1Arr[0] = ival1;
        ival1Arr[1] = ival2;

        struct bcm_timeval ivalArr1Zero[2];
        ivalArr1Zero[0] = ivalZero;
        ivalArr1Zero[1] = ivalZero;

        struct bcm_timeval ival2Arr[2];
        ival2Arr[0] = ival2;
        ival2Arr[1] = ival2;

        // Test Counts
        uint32_t countArr[2];
        countArr[0] = 10;
        countArr[1] = 5;

        uint32_t countArrZero[2] = {0};

        // Test Mask
        struct canfd_frame mask = {0};
        mask.len     = 1;
        mask.data[0] = 0xFF;

        // Test txSendSingleFrame with a single CAN frame
        for(auto & i : frameArrCAN){
            txSendSingleFrame(i, false);
        }

        // Test txSendSingleFrame with a single CANFD frames
        //for(auto & i : frameArrCANFD){
        //    txSendSingleFrame(i, true);
        //}

        // Test txSendMultipleFrames with multiple CAN frames
        //txSendMultipleFrames(frameArrCAN, 2, false);

        // Test txSendMultipleFrames with multiple CANFD frames
        //txSendMultipleFrames(frameArrCANFD, 2, true);

        // Test txSetupSingleFrame with a single CAN frame
        //for(auto & i : frameArrCAN){
        //    txSetupSingleFrame(i, 2, ival1, ival2, false);
        //}

        // Test txSetupSingleFrame with a single CANFD frame
        //for(auto & i : frameArrCANFD){
        //   txSetupSingleFrame(i, 2, ival1, ival2, true);
        //}

        // Test txSetupMultipleFrames with CAN frames
        //txSetupMultipleFrames(frameArrCAN, 2, countArr, ival1Arr, ival2Arr, false);

        // Test txSetupMultipleFrames with CANFD frames
        //txSetupMultipleFrames(frameArrCANFD, 2, countArr, ival1Arr, ival2Arr, true);

        // Test txSetupSequence with multiple CAN frames
        //txSetupSequence(frameArrCAN, 2, 3, ival1, ival2, false);

        // Test txSetupSequence with multiple CANFD frames
        //txSetupSequence(frameArrCANFD, 2, 3, ival1, ival2, true);

        // Test txSetupUpdate without announce
        //txSetupMultipleFrames(frameArrCAN, 2, countArrZero, ivalArr1Zero, ival2Arr, false);
        //sleep(10);
        //txSetupUpdateMultipleFrames(frameArrCANModified, 2, false, false);
        //sleep(10);

        // Test txSetupUpdate with announce
        //txSetupMultipleFrames(frameArrCAN, 2, countArrZero, ivalArr1Zero, ival2Arr,false);
        //sleep(10);
        //txSetupUpdateMultipleFrames(frameArrCANModified, 2, false, true);
        //sleep(10);

        // Test txDelete with a canfd transmission task
        //txSetupMultipleFrames(frameArrCANFD, 2, countArr, ival1Arr, ival2Arr, true);
        //std::this_thread::sleep_for(std::chrono::seconds(3));
        //txDelete(frameArrCANFD[0].can_id, true);
        //std::this_thread::sleep_for(std::chrono::seconds(3));
        //txDelete(frameArrCANFD[1].can_id, true);

        // Test txDelete with a canfd sequence transmission task
        //txSetupSequence(&canfdFrame1, 1, 3, ival1, ival2, true);
        //std::this_thread::sleep_for(std::chrono::seconds(5));
        //txDelete(canfdFrame1.can_id, true);

        // Test rxSetupCanID with a CAN frame
        //rxSetupCanID(0x222, false);

        // Test rxSetupCanID with a CANFD frame
        //rxSetupCanID(0x333, true);

        // Test rxSetupMask with a CAN frame
        //rxSetupMask(0x222, mask, false);

        // Test rxSetupMask with a CANFD frame
        //rxSetupMask(0x333, mask, true);

        // Test rxDelete with a CAN frame
        //rxSetupCanID(0x222, false);
        //std::this_thread::sleep_for(std::chrono::seconds(3));
        //rxDelete(0x222, false);

        // Test rxDelete with a CANFD frame
        //rxSetupCanID(0x333, true);
        //std::this_thread::sleep_for(std::chrono::seconds(3));
        //rxDelete(0x333, true);
    }

}


/*******************************************************************************
 * END OF FILE
 ******************************************************************************/