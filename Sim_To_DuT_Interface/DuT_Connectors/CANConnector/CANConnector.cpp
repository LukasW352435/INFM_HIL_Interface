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
#include "CANConnector.h"


/*******************************************************************************
 * FUNCTION DEFINITIONS
 ******************************************************************************/
namespace sim_interface::dut_connector::can{

    /**
     * CAN Connector constructor.
     *
     * @param queueDuTToSim - Queue to write received simulation events to.
     * @param config        - Configuration for the connector.
     */
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

        // Start the receive loop on the socket
        receiveOnSocket();

        // Start the io context loop
        startProcessing();

        std::cout << "CAN Connector created" << std::endl;
    }

    /**
    * CAN Connector destructor.
    */
    CANConnector::~CANConnector(){

        // Stop the io context loop
        stopProcessing();

        std::cout << "CAN Connector destroyed" << std::endl;
    }

    /**
    * Creates the bcmSocket data member.
    *
    * @return The BCM socket.
    */
    boost::asio::generic::datagram_protocol::socket CANConnector::createBcmSocket(const CANConnectorConfig &config){

        // Error code return value
        boost::system::error_code errorCode;

        // Define Address family and protocol
        boost::asio::generic::datagram_protocol bcmProtocol(PF_CAN, CAN_BCM);

        // Create a BCM socket
        boost::asio::generic::datagram_protocol::socket socket(*ioContext, bcmProtocol);

        // Create an I/O command and resolve the interface name to an interface index
        InterfaceIndexIO interfaceIndexIO(config.interfaceName);
        socket.io_control(interfaceIndexIO, errorCode);

        // Check if we could resolve the interface correctly
        if(errorCode){
            std::cout << "An error occurred on the io control operation: " << errorCode.message() << std::endl;
        }

        // Connect the socket
        sockaddr_can addr = {0};
        addr.can_family   = AF_CAN;
        addr.can_ifindex  = interfaceIndexIO.index();

        boost::asio::generic::datagram_protocol::endpoint bcmEndpoint{&addr, sizeof(addr)};
        socket.connect(bcmEndpoint, errorCode);

        // Check if we could connect correctly
        if(errorCode){
            std::cout << "An error occurred on the connect operation: " << errorCode.message() << std::endl;
        }

        // Note: In contrast to a raw CAN socket there is no need to
        // explicitly enable CANFD for an BCM socket with setsockopt!

        return socket;
    }

    /**
    * Stats the io context loop.
    */
    void CANConnector::startProcessing(){

        // Run the io context in its own thread
        ioContextThread = std::thread(&CANConnector::ioContextThreadFunction, this, std::ref(ioContext));

        std::cout << "CAN Connector starting io context loop processing" << std::endl;
    }

    /**
    * Stops the io context loop.
    */
    void CANConnector::stopProcessing(){

        // Check if we need to stop the io context loop
        if(!ioContext->stopped()){
            ioContext->stop();
        }

        // Join the io context loop thread
        if(ioContextThread.joinable()){
            ioContextThread.join();
        }else{
            std::cout << "Error ioContextThread was not joinable" << std::endl;
        }

        std::cout << "CAN Connector stopped io context loop processing" << std::endl;
    }

    /**
    * Thread for the io context loop.
    */
    void CANConnector::ioContextThreadFunction(const boost::shared_ptr<boost::asio::io_context>& context){
        context->run();
    }

    /**
     * Gets information about the CAN Connector
     *
     * @return info - The connector information
     */
    ConnectorInfo CANConnector::getConnectorInfo(){
        ConnectorInfo info(
                "CAN Connector",
                0x0000001,
                "The CAN Connector enables the communication over a CAN/CANFD interface.");
        return info;
    }

    /**
    * Receives on the BCM socket. The received data is stored in the rxBuffer.
    * After processing the receive operation the next receive operation is
    * created to keep  the io context loop running.
    */
    void CANConnector::receiveOnSocket(){

        std::cout << "CAN Connector created new receive operation" << std::endl;

        // Create an async receive operation on the BCM socket
        bcmSocket.async_receive(boost::asio::buffer(rxBuffer),
                                [this](boost::system::error_code errorCode, std::size_t receivedBytes){

            // Lambda completion function for the async receive operation

            // Check the error code of the operation
            if(!errorCode){

                std::cout << "CAN Connector received: " << receivedBytes << " bytes" << std::endl;

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
                        std::cout << "The expected amount of bytes is not equal to the received bytes" << std::endl;
                    }

                }

            }else{
                std::cout << "An error occurred on the async receive operation: " << errorCode.message() << std::endl;
            }

            // Create the next receive operation
            receiveOnSocket();

        });

    }

    /**
    * Create a non cyclic transmission task for a single CAN/CANFD frame.
    *
    * @param frame   - The frame that should be send.
    * @param isCANFD - Flag for a CANFD frame.
    */
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
            std::cout << "Error could not make message structure" << std::endl;
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
                std::cout << "Transmission of TX_SEND completed successfully" << std::endl;
            }else{
                std::cerr << "Transmission of TX_SEND failed" << std::endl;
            }

        });

    }

    /**
    * Create a non cyclic transmission task for multiple CAN/CANFD frames.
    *
    * @param frames  - The frames that should be send.
    * @param nframes - The number of frames that should be send.
    * @param isCANFD - Flag for CANFD frames.
    */
    void CANConnector::txSendMultipleFrames(struct canfd_frame frames[], int nframes, bool isCANFD){

        // Note: The TX_SEND operation can only handle exactly one frame!
        // That's why we should use this wrapper for multiple frames.
        for(int index = 0; index < nframes; index++){
            txSendSingleFrame(frames[index], isCANFD);
        }

    }

    /**
    * Create a cyclic transmission task for a CAN/CANFD frame.
    *
    * @param frame   - The frame that should be send cyclic.
    * @param count   - Number of times the frame is send with the first interval.
    *                  If count is zero only the second interval is being used.
    * @param ival1   - First interval.
    * @param ival2   - Second interval.
    * @param isCANFD - Flag for a CANFD frames.
    */
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
            std::cout << "Error could not make message structure" << std::endl;
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
                std::cout << "Transmission of TX_SETUP completed successfully" << std::endl;
            }else{
                std::cerr << "Transmission of TX_SETUP failed" << std::endl;
            }

        });

    }

    /**
     * Create a cyclic transmission task for multiple CAN/CANFD frames.
    *
    * Note: The frames will not be send as a atomic sequence. We send for each TX_SETUP
    * a single CAN/CANFD frame with its CAN ID in the bcm_msg_head. This way we do not
    * create a cyclic transmission sequence which can only be removed with the CAN ID
    * that was set in the bcm_msg_head. Another benefit is that each CAN/CANFD frame
    * can have different count, ival1, and ival2 values.
    *
    * @param frames  - The frames that should be send cyclic.
    * @param nframes - The number of frames that should be send cyclic.
    * @param count   - Number of times the frame is send with the first interval.
    *                  If count is zero only the second interval is being used.
    * @param ival1   - First interval.
    * @param ival2   - Second interval.
    *  @param isCANFD - Flag for CANFD frames.
    */
    void CANConnector::txSetupMultipleFrames(struct canfd_frame frames[], int nframes, uint32_t count[], struct bcm_timeval ival1[],
                                             struct bcm_timeval ival2[], bool isCANFD){

        for(int index = 0; index < nframes; index++){
            txSetupSingleFrame(frames[index], count[index], ival1[index], ival2[index], isCANFD);
        }

    }

    /**
    * Create a cyclic transmission task for one or multiple CAN/CANFD frames.
    * If more than one frame should be send cyclic the provided sequence of
    * the frames is kept by the BCM.
    *
    * Note: The cyclic transmission task for the sequence can only be deleted
    * with the CAN ID that was set in the bcm_msg_head.
    *
    * @param frames   - The array of CAN/CANFD frames that should be send cyclic.
    * @param nframes  - The number of CAN/CANFD frames that should be send cyclic.
    * @param count    - Number of times the frame is send with the first interval.
    *                   If count is zero only the second interval is being used.
    * @param ival1    - First interval.
    * @param ival2    - Second interval.
    * @param isCANFD  - Flag for CANFD frames.
    */
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
            std::cout << "Error could not make message structure" << std::endl;
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
                std::cout << "Transmission of TX_SETUP sequence completed successfully" << std::endl;
            }else{
                std::cerr << "Transmission of TX_SETUP sequence failed" << std::endl;
            }

        });

    }

    /**
    * Updates a cyclic transmission task for a CAN/CANFD frame.
    *
    * @param frames   - The updated CAN/CANFD frame data.
    * @param nframes  - The number of CAN/CANFD frames that should be updated.
    * @param isCANFD  - Flag for CANFD frames.
    * @param announce - Flag for immediately sending out the changes once will retaining the cycle.
    */
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
            std::cout << "Error could not make message structure" << std::endl;
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
                std::cout << "Transmission of TX_SETUP update completed successfully" << std::endl;
            }else{
                std::cerr << "Transmission of TX_SETUP update failed" << std::endl;
            }

        });

    }

    /**
    * Updates a cyclic transmission task for one or multiple CAN/CANFD frames.
    *
    * @param frames   - The array of CAN/CANFD frames with the updated data.
    * @param nframes  - The number of CAN/CANFD frames that should be updated.
    * @param isCANFD  - Flag for CANFD frames.
    * @param announce - Flag for immediately sending out the changes once while retaining the cycle.
    */
    void CANConnector::txSetupUpdateMultipleFrames(struct canfd_frame frames[], int nframes, bool isCANFD, bool announce){

        for(int index = 0; index < nframes; index++){
            txSetupUpdateSingleFrame(frames[index], isCANFD, announce);
        }

    }

    /**
    * Removes a cyclic transmission task for the given CAN ID.
    *
    * Note: A cyclic transmission task for a sequence of frames can only
    * be deleted with the CAN ID that was set in the bcm_msg_head.
    *
    * @param canID - The CAN ID of the task that should be removed.
    */
    void CANConnector::txDelete(canid_t canID, bool isCANFD){

        // BCM message we are sending
        auto msg = std::make_shared<bcm_msg_head>();

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
                std::cout << "Transmission of TX_DELETE completed successfully" << std::endl;
            }else{
                std::cerr << "Transmission of TX_DELETE failed" << std::endl;
            }

        });

    }

    /**
     * Creates a RX filter for the given CAN ID.
    * I. e. we get notified on all received frames with this CAN ID.
    *
    * @param canID    - The CAN ID that should be added to the RX filter.
    * @param isCANFD  - Flag for CANFD frames.
    */
    void CANConnector::rxSetupCanID(canid_t canID, bool isCANFD){

        // BCM message we are sending
        auto msg = std::make_shared<bcm_msg_head>();

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
                std::cout << "Transmission of RX_SETUP based on a CAN ID completed successfully" << std::endl;
            }else{
                std::cerr << "Transmission of RX_SETUP based on a CAN ID failed" << std::endl;
            }

        });

    }

    /**
    * Creates a RX filter for the CAN ID and the relevant bits of the frame.
    * I. e. we only get notified on changes for the set bits in the mask.
    *
    * @param canID    - The CAN ID that should be added to the RX filter.
    * @param mask     - The mask for the relevant bits of the frame.
    * @param isCANFD  - Flag for CANFD frames.
    */
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
            std::cout << "Error could not make message structure" << std::endl;
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
                std::cout << "Transmission of RX_SETUP with mask completed successfully" << std::endl;
            }else{
                std::cerr << "Transmission of RX_SETUP with mask failed" << std::endl;
            }

        });
    }

    /**
    * Removes the RX filter for the given CAN ID.
    *
    * @param canID   - The CAN ID that should be removed from the RX filter.
    * @param isCANFD - Flag for CANFD frames.
    */
    void CANConnector::rxDelete(canid_t canID, bool isCANFD){

        // BCM message we are sending
        auto msg = std::make_shared<bcm_msg_head>();

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
                std::cout << "Transmission of RX_DELETE completed successfully" << std::endl;
            }else{
                std::cerr << "Transmission of RX_DELETE failed" << std::endl;
            }

        });

    }

    /**
    * Decides what to do with the data we received on the socket.
    *
    * @param head    - The received bcm msg head.
    * @param frames  - The received CAN or CANFD frames.
    * @param nframes - The number of the received frames.
    * @param isCANFD - Flag for CANFD frames.
    */
    void CANConnector::handleReceivedData(const bcm_msg_head *head, void *frames, uint32_t nframes, bool isCANFD){

        std::cout << "Handling the received data" << std::endl;

        switch(head->opcode){

            case RX_CHANGED:

                // Simple reception of a CAN/CANFD frame or a content change occurred.
                // TODO: Implement handling
                std::cout << "RX_CHANGED is not implemented" << std::endl;
                break;

            case RX_TIMEOUT:

                // Cyclic message is detected to be absent.
                std::cout << "RX_TIMEOUT is not implemented" << std::endl;
                break;

            case TX_EXPIRED:

                // Notification when counter finishes sending at ival1 interval.
                // Requires TX_COUNTEVT flag to be set at TX_SETUP.
                std::cerr << "TX_EXPIRED is not implemented" << std::endl;
                break;

            case RX_STATUS:

                // Reply to a RX_READ request that returns the RX content filter properties for a given CAN ID.
                // This should not occur on a regular receive.
                std::cerr << "Received unexpected RX_STATUS message" << std::endl;
                break;

            case TX_STATUS:

                // Reply to a TX_READ request that returns the TX transmission properties for a given CAN ID.
                // This should not occur on a regular receive.
                std::cerr << "Received unexpected TX_STATUS message" << std::endl;
                break;

            default:

                std::cerr << "Received unknown opcode!" << std::endl;
        }

    }

    /**
    * Decides what to do with the event we received from the simulation.
    *
    * @param event - The event we received from the simulation.
    */
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