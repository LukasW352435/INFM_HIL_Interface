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

        // Create the codec
        this->codec = CANConnectorCodecFactory::createCodec(this->config.codecName);
        InterfaceLogger::logMessage("CAN Connector: Created <" + this->config.codecName + "> codec", LOG_LEVEL::INFO);

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

        InterfaceLogger::logMessage("CAN Connector: Created initial RX setup", LOG_LEVEL::INFO);

        // Create the isSetup map based on the send operations in the config.
        // This map keeps track if we already created a cyclic send operation,
        // so we know if we need to create a new one or only perform an update.
        for(auto const& [operation, sendOperation] : config.operationToFrame){

            // Add it to the map if it is a cyclic operation
            if(sendOperation.isCyclic){
                this->isSetup[operation] = false;
            }

        }

        InterfaceLogger::logMessage("CAN Connector: Created initial isSetup map", LOG_LEVEL::INFO);

        // Start the receive loop on the socket
        receiveOnSocket();

        // Start the io context loop
        startProcessing();

        InterfaceLogger::logMessage("CAN Connector: CAN Connector created", LOG_LEVEL::INFO);
    }

    CANConnector::~CANConnector(){

        // Stop the io context loop
        stopProcessing();

        InterfaceLogger::logMessage("CAN Connector: CAN Connector destroyed", LOG_LEVEL::INFO);
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
            InterfaceLogger::logMessage("CAN Connector: An error occurred on the io control operation: " + errorCode.message(), LOG_LEVEL::ERROR);
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
            InterfaceLogger::logMessage("CAN Connector: An error occurred on the connect operation: " + errorCode.message(), LOG_LEVEL::ERROR);
            throw std::runtime_error("CAN Connector: Could not connect to the interface");
        }

        // Note: In contrast to a raw CAN socket there is no need to
        // explicitly enable CANFD for an BCM socket with setsockopt!

        return socket;
    }

    void CANConnector::startProcessing(){

        // Run the io context in its own thread
        ioContextThread = std::thread(&CANConnector::ioContextThreadFunction, this, std::ref(ioContext));

        InterfaceLogger::logMessage("CAN Connector: Starting the io context loop", LOG_LEVEL::INFO);
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
            InterfaceLogger::logMessage("CAN Connector: ioContextThread was not joinable", LOG_LEVEL::ERROR);
        }

        InterfaceLogger::logMessage("CAN Connector: Stopped the io context loop", LOG_LEVEL::INFO);
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

        InterfaceLogger::logMessage("CAN Connector: Creating a new receive operation on the socket", LOG_LEVEL::INFO);

        // Create an async receive operation on the BCM socket
        bcmSocket.async_receive(boost::asio::buffer(rxBuffer),
                                [this](boost::system::error_code errorCode, std::size_t receivedBytes){

            // Lambda completion function for the async receive operation

            // Check the error code of the operation
            if(!errorCode){

                InterfaceLogger::logMessage("CAN Connector: Received " + std::to_string(receivedBytes) + " bytes on the socket", LOG_LEVEL::INFO);

                // We need to receive at least a whole bcm_msg_head
                if(receivedBytes >= sizeof(bcm_msg_head)){

                    // Get the bcm_msg_head
                    const auto* head = reinterpret_cast<const bcm_msg_head*>(rxBuffer.data());

                    // Check if the message contains CAN or CANFD frames
                    bool isCANFD = false;

                    if(head->flags & CAN_FD_FRAME){
                        isCANFD = true;
                    }

                    // Calculate the expected size in bytes of the whole message based upon the bcm_msg_head information.
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
                        InterfaceLogger::logMessage("CAN Connector: The expected amount of bytes is not equal to the received bytes", LOG_LEVEL::ERROR);
                    }

                }

            }else{
                InterfaceLogger::logMessage("CAN Connector: An error occurred on the async receive operation: " + errorCode.message(), LOG_LEVEL::ERROR);
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
            InterfaceLogger::logMessage("CAN Connector: Could not make the message structure", LOG_LEVEL::ERROR);
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
                InterfaceLogger::logMessage("CAN Connector: TX_SEND completed successfully", LOG_LEVEL::INFO);
            }else{
                InterfaceLogger::logMessage("CAN Connector: TX_SEND failed: " + errorCode.message(), LOG_LEVEL::ERROR);
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
            InterfaceLogger::logMessage("CAN Connector: Could not make the message structure", LOG_LEVEL::ERROR);
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

        InterfaceLogger::logMessage("CAN Connector: TX_SETUP created for the CAN ID: " + convertCanIdToHex(frame.can_id), LOG_LEVEL::INFO);

        // Note: buffer doesn't accept smart pointers. Need to use a regular pointer.
        boost::asio::const_buffer buffer = boost::asio::buffer(msg.get(), msgSize);

        bcmSocket.async_send(buffer, [msg](boost::system::error_code errorCode, std::size_t size){

            // Lambda completion function for the async TX_SETUP operation

            // Check boost asio error code
            if(!errorCode){
                InterfaceLogger::logMessage("CAN Connector: TX_SETUP completed successfully", LOG_LEVEL::INFO);
            }else{
                InterfaceLogger::logMessage("CAN Connector: TX_SETUP failed: " + errorCode.message(), LOG_LEVEL::ERROR);
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
            InterfaceLogger::logMessage("CAN Connector: Could not make the message structure", LOG_LEVEL::ERROR);
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
                InterfaceLogger::logMessage("CAN Connector: TX_SETUP (sequence) completed successfully", LOG_LEVEL::INFO);
            }else{
                InterfaceLogger::logMessage("CAN Connector: TX_SETUP (sequence) failed: " + errorCode.message(), LOG_LEVEL::ERROR);
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
            InterfaceLogger::logMessage("CAN Connector: Could not make the message structure", LOG_LEVEL::ERROR);
            return;
        }

        // Fill out the message
        // Note: By combining the flags SETTIMER and STARTTIMER the BCM will start sending the messages immediately
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

        InterfaceLogger::logMessage("CAN Connector: TX_SETUP (update) created for the CAN ID: " + convertCanIdToHex(frame.can_id), LOG_LEVEL::INFO);

        // Note: buffer doesn't accept smart pointers. Need to use a regular pointer.
        boost::asio::const_buffer buffer = boost::asio::buffer(msg.get(), msgSize);

        bcmSocket.async_send(buffer, [msg](boost::system::error_code errorCode, std::size_t size){

            // Lambda completion function for the async TX_SETUP operation

            // Check boost asio error code
            if(!errorCode){
                InterfaceLogger::logMessage("CAN Connector: TX_SETUP (update) completed successfully", LOG_LEVEL::INFO);
            }else{
                InterfaceLogger::logMessage("CAN Connector: TX_SETUP (update) failed: " + errorCode.message(), LOG_LEVEL::ERROR);
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
            InterfaceLogger::logMessage("CAN Connector: Could not make the message structure", LOG_LEVEL::ERROR);
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
                InterfaceLogger::logMessage("CAN Connector: TX_DELETE completed successfully", LOG_LEVEL::INFO);
            }else{
                InterfaceLogger::logMessage("CAN Connector: TX_DELETE failed: " + errorCode.message(), LOG_LEVEL::ERROR);
            }

        });

    }

    void CANConnector::rxSetupCanID(canid_t canID, bool isCANFD){

        // BCM message we are sending
        auto msg = std::make_shared<bcm_msg_head>();

        // Error handling / Sanity check
        if(msg == nullptr){
            InterfaceLogger::logMessage("CAN Connector: Could not make the message structure", LOG_LEVEL::ERROR);
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
                InterfaceLogger::logMessage("CAN Connector: RX_SETUP (CAN ID) completed successfully", LOG_LEVEL::INFO);
            }else{
                InterfaceLogger::logMessage("CAN Connector: RX_SETUP (CAN ID) failed: " + errorCode.message(), LOG_LEVEL::ERROR);
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
            InterfaceLogger::logMessage("CAN Connector: Could not make the message structure", LOG_LEVEL::ERROR);
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
                InterfaceLogger::logMessage("CAN Connector: RX_SETUP (mask) completed successfully", LOG_LEVEL::INFO);
            }else{
                InterfaceLogger::logMessage("CAN Connector: RX_SETUP (mask) failed: " + errorCode.message(), LOG_LEVEL::INFO);
            }

        });
    }

    void CANConnector::rxDelete(canid_t canID, bool isCANFD){

        // BCM message we are sending
        auto msg = std::make_shared<bcm_msg_head>();

        // Error handling / Sanity check
        if(msg == nullptr){
            InterfaceLogger::logMessage("CAN Connector: Could not make the message structure", LOG_LEVEL::ERROR);
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
                InterfaceLogger::logMessage("CAN Connector: RX_DELETE completed successfully", LOG_LEVEL::INFO);
            }else{
                InterfaceLogger::logMessage("CAN Connector: RX_DELETE failed: " + errorCode.message(), LOG_LEVEL::ERROR);
            }

        });

    }

    void CANConnector::handleReceivedData(const bcm_msg_head *head, void *frames, uint32_t nframes, bool isCANFD){

        InterfaceLogger::logMessage("CAN Connector: Handling the received data", LOG_LEVEL::INFO);

        switch(head->opcode){

            case RX_CHANGED:

                // Simple reception of a CAN/CANFD frame or a content change occurred.
                InterfaceLogger::logMessage("CAN Connector: RX_CHANGED for CAN ID: <" + convertCanIdToHex(head->can_id) + ">" , LOG_LEVEL::INFO);

                // We should only receive one frame in each RX_CHANGED BCM message
                if(nframes != 1){
                    InterfaceLogger::logMessage("CAN Connector: Received an RX_CHANGED message with multiple frames", LOG_LEVEL::ERROR);
                    return;
                }

                CANConnector::handleRxChanged(head, frames, isCANFD);
                break;

            case RX_TIMEOUT:

                // Cyclic message is detected to be absent by the timeout monitoring.
                // There is no function implemented yet that should cause/use this.
                InterfaceLogger::logMessage("CAN Connector: RX_TIMEOUT handling is not implemented", LOG_LEVEL::WARNING);
                break;

            case TX_EXPIRED:

                // Notification when counter finishes sending at ival1 interval.
                // Requires TX_COUNTEVT flag to be set at TX_SETUP.
                // There is no function implemented yet that should cause/use this.
                InterfaceLogger::logMessage("CAN Connector: TX_EXPIRED handling is not implemented", LOG_LEVEL::WARNING);
                break;

            case RX_STATUS:

                // Reply to a RX_READ request that returns the RX content filter properties for a given CAN ID.
                // This should not occur on our regular receive loop.
                InterfaceLogger::logMessage("CAN Connector: Received unexpected RX_STATUS message", LOG_LEVEL::WARNING);
                break;

            case TX_STATUS:

                // Reply to a TX_READ request that returns the TX transmission properties for a given CAN ID.
                // This should not occur on our regular receive loop.
                InterfaceLogger::logMessage("CAN Connector: Received unexpected TX_STATUS message", LOG_LEVEL::WARNING);
                break;

            default:
                InterfaceLogger::logMessage("CAN Connector: Received unknown opcode", LOG_LEVEL::WARNING);
        }

    }

    void CANConnector::handleRxChanged(const bcm_msg_head* head, void* frame, bool isCANFD){

        // The simulation events we received
        std::vector<SimEvent> events;

        // Note: We could also pass a reference instead of passing a copy of the received
        // frame data in a struct can_fd. This way it may be a bit easier to end user that
        // only wants to implement codec?

        // Check if it is a CAN or CANFD frame we need to fill out.
        if(isCANFD){
            // Initialize the struct with zero
            struct canfd_frame canfdFrame = {0};

            // Copy the frame data
            auto canfdFramePtr = (struct canfd_frame*) frame;
            canfdFrame = *canfdFramePtr;

            events = codec->convertFrameToSimEvent(canfdFrame, isCANFD);
        }else{
            // Initialize the struct with zero
            struct canfd_frame canfdFrame = {0};

            // Copy the frame data
            auto canFramePtr = (struct can_frame*) frame;
            struct can_frame canFrame = *canFramePtr;
            canfdFrame = *((struct canfd_frame*) &canFrame);

            events = codec->convertFrameToSimEvent(canfdFrame, isCANFD);
        }

        // Sanity check
        if(events.empty()){
            InterfaceLogger::logMessage("CAN Connector: Codec returned no simulation events for the received frame", LOG_LEVEL::WARNING);
        }

        // Send the received simulation events to the simulation
        for(const auto& event: events){
            InterfaceLogger::logMessage("CAN Connector: Send SimEvent: <" + event.operation + ">", LOG_LEVEL::INFO);
            sendEventToSim(event);
        }

    }

    void CANConnector::handleEventSingle(const SimEvent &event){

        // Convert the simulation event to a CAN/CANFD frame payload and the sendOperation name
        auto data = codec->convertSimEventToFrame(event);
        auto frameData = data.first;
        CANConnectorSendOperation sendOperation = this->config.operationToFrame.at(data.second);

        // Sanity checks to identify errors made by the user written codec
        if(frameData.empty()){
            InterfaceLogger::logMessage("CAN Connector: Codec returned an empty frame payload for a simulation event", LOG_LEVEL::WARNING);
            return;
        }else{
            if(sendOperation.isCANFD){
                if(CANFD_MAX_DLEN < frameData.size()){
                    InterfaceLogger::logMessage(
                            "CAN Connector: Codec returned a frame payload that is bigger than the CANFD frame",
                            LOG_LEVEL::ERROR);
                    return;
                }
            }else{
                if(CAN_MAX_DLEN < frameData.size()){
                    InterfaceLogger::logMessage(
                            "CAN Connector: Codec returned a frame payload that is bigger than the CAN frame",
                            LOG_LEVEL::ERROR);
                    return;
                }
            }
        }

        // Fill out the frame
        struct canfd_frame canfdFrame = {0};

        if(sendOperation.isCANFD){
            canfdFrame.can_id = sendOperation.canID;
            canfdFrame.len    = frameData.size();
            memcpy((void*) &canfdFrame.data[0], frameData.data(), frameData.size());
        }else{
            struct can_frame canFrame = {0};
            canFrame.can_id  = sendOperation.canID;
            canFrame.can_dlc = frameData.size();
            memcpy((void*) &canFrame.data[0], frameData.data(), frameData.size());
            canfdFrame = *((struct canfd_frame*) &canFrame);
        }

        // Check if we should send it once or cyclic
        if(sendOperation.isCyclic){

            // Check if a cyclic send operation was set up already
            if(this->isSetup.at(data.second)){
                // Update the cyclic send operation with the new frame payload
                txSetupUpdateSingleFrame(canfdFrame, sendOperation.isCANFD, sendOperation.announce);
            }else{
                // Create a new cyclic send operation and remember that we already set it up
                this->isSetup[data.second] = true;
                txSetupSingleFrame(canfdFrame, sendOperation.count, sendOperation.ival1, sendOperation.ival2, sendOperation.isCANFD);
            }

        }else{
            // Send out the frame once
            txSendSingleFrame(canfdFrame, sendOperation.isCANFD);
        }

    }

    std::string CANConnector::convertCanIdToHex(canid_t canID){
        std::stringstream canIdHex;
        canIdHex << "0x" << std::hex << canID;
        return canIdHex.str();
    }

}


/*******************************************************************************
 * END OF FILE
 ******************************************************************************/