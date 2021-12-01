/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      CANConnector.h
 \brief     The CANConnector enables the communication over a CAN interface.
            It builds upon the socketCAN BCM socket and boost::asio.
 \author    Matthias Bank
 \version   1.0.0
 \date      12.11.2021
 ******************************************************************************/
#ifndef SIM_TO_DUT_INTERFACE_CANCONNECTOR_H
#define SIM_TO_DUT_INTERFACE_CANCONNECTOR_H


/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
// Project includes
#include "../DuTConnector.h"
#include "InterfaceIndexIO.h"
#include "CANConnectorConfig.h"

// System includes
#include <thread>
#include <iostream>
#include <linux/can.h>
#include <linux/can/bcm.h>
#include <boost/asio.hpp>
#include <boost/make_shared.hpp>
#include <boost/system/error_code.hpp>


/*******************************************************************************
 * DEFINES
 ******************************************************************************/

/**
 * Defines how many frames can be put in a bcmMsgMultipleFrames operation.
 * The socketCAN BCM can send up to 256 CAN frames in a sequence in the case
 * of a cyclic TX task configuration. See the socketCAN BCM documentation.
 */
#define MAXFRAMES 256


/*******************************************************************************
 * STRUCTS
 ******************************************************************************/

/**
 * Struct for a BCM message with a single CAN frame.
 */
struct bcmMsgSingleFrameCan{
    struct bcm_msg_head msg_head;
    struct can_frame canFrame[1];
};

/**
 * Struct for a BCM message with a single CANFD frame.
 */
struct bcmMsgSingleFrameCanFD{
    struct bcm_msg_head msg_head;
    struct canfd_frame canfdFrame[1];
};

/**
 * Struct for a BCM message with multiple CAN frames.
 */
struct bcmMsgMultipleFramesCan{
    struct bcm_msg_head msg_head;
    struct can_frame canFrames[MAXFRAMES];
};

/**
* Struct for a BCM message with multiple CANFD frames.
*/
struct bcmMsgMultipleFramesCanFD{
    struct bcm_msg_head msg_head;
    struct canfd_frame canfdFrames[MAXFRAMES];
};


/*******************************************************************************
 * CLASS DECLARATIONS
 ******************************************************************************/
namespace sim_interface::dut_connector::can{

    class CANConnector : public DuTConnector{

    public:
        // Functions members

        /**
        * CAN Connector constructor.
        * Configures the BCM socket based on the provided receive configuration.
        * Starts the io_context- and receive-loop of the CAN Connector.
        *
        * @param queueDuTToSim - Queue to write received simulation events to.
        * @param config        - Configuration for the connector.
        */
        CANConnector(std::shared_ptr<SharedQueue<SimEvent>> queueDuTToSim, const CANConnectorConfig &config);

        /**
        * CAN Connector destructor.
        * Stops the io_context loop of the CAN Connector.
        */
        ~CANConnector();

        /**
        * Gets information about the CAN Connector
        *
        * @return info - The connector information
        */
        ConnectorInfo getConnectorInfo() override;

        /**
        * Decides what to do with the event we received from the simulation.
        *
        * @param event - The event we received from the simulation.
        */
        void handleEventSingle(const SimEvent &event) override;

        // Data members

    private:
        // Function members

        /**
        * Creates the BCM socket that is used by the CAN Connector.
        *
        * @return The BCM socket.
        */
        boost::asio::generic::datagram_protocol::socket createBcmSocket(const CANConnectorConfig &config);

        /**
        * Starts the io context loop that is running in a dedicated thread.
        * Note: Because io_context.run() is a blocking call it needs the dedicated thread.
        */
        void startProcessing();

        /**
        * Stops the io context loop.
        */
        void stopProcessing();

        /**
        * Thread function for the io context loop.
        */
        void ioContextThreadFunction(const boost::shared_ptr<boost::asio::io_context>& context);

        /**
        * Receives on the BCM socket. The received data is stored in the rxBuffer.
        * After processing the receive operation the next receive operation is
        * created (function calls itself) to keep the io context loop running.
        */
        void receiveOnSocket();

        /**
        * Decides what to do with the data we received on the socket.
        *
        * @param head    - The received bcm msg head.
        * @param frames  - The received CAN or CANFD frames.
        * @param nframes - The number of the received frames.
        * @param isCANFD - Flag for CANFD frames.
        */
        void handleReceivedData(const bcm_msg_head* head, void* frames, uint32_t nframes, bool isCANFD);

        /**
        * Create a non cyclic transmission task for a single CAN/CANFD frame.
        *
        * @param frame   - The frame that should be send.
        * @param isCANFD - Flag for a CANFD frame.
        */
        void txSendSingleFrame(struct canfd_frame frame, bool isCANFD);

        /**
        * Create a non cyclic transmission task for multiple CAN/CANFD frames.
        *
        * @param frames  - The frames that should be send.
        * @param nframes - The number of frames that should be send.
        * @param isCANFD - Flag for CANFD frames.
        */
        void txSendMultipleFrames(struct canfd_frame frames[], int nframes, bool isCANFD);

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
        void txSetupSingleFrame(struct canfd_frame frame, uint32_t count, struct bcm_timeval ival1, struct bcm_timeval ival2, bool isCANFD);

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
        * @param isCANFD - Flag for CANFD frames.
        */
        void txSetupMultipleFrames(struct canfd_frame frames[], int nframes, uint32_t count[], struct bcm_timeval ival1[], struct bcm_timeval ival2[], bool isCANFD);

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
        void txSetupSequence(struct canfd_frame frames[], int nframes, uint32_t count, struct bcm_timeval ival1, struct bcm_timeval ival2, bool isCANFD);

        /**
        * Updates a cyclic transmission task for a CAN/CANFD frame.
        *
        * @param frames   - The updated CAN/CANFD frame data.
        * @param nframes  - The number of CAN/CANFD frames that should be updated.
        * @param isCANFD  - Flag for CANFD frames.
        * @param announce - Flag for immediately sending out the changes once will retaining the cycle.
        */
        void txSetupUpdateSingleFrame(struct canfd_frame frame, bool isCANFD, bool announce);

        /**
        * Updates a cyclic transmission task for one or multiple CAN/CANFD frames.
        *
        * @param frames   - The array of CAN/CANFD frames with the updated data.
        * @param nframes  - The number of CAN/CANFD frames that should be updated.
        * @param isCANFD  - Flag for CANFD frames.
        * @param announce - Flag for immediately sending out the changes once while retaining the cycle.
        */
        void txSetupUpdateMultipleFrames(struct canfd_frame frames[], int nframes, bool isCANFD, bool announce);

        /**
        * Removes a cyclic transmission task for the given CAN ID.
        *
        * Note: A cyclic transmission task for a sequence of frames can only
        * be deleted with the CAN ID that was set in the bcm_msg_head.
        *
        * @param canID - The CAN ID of the task that should be removed.
        */
        void txDelete(canid_t canID, bool isCANFD);

        /**
        * Creates a RX filter for the given CAN ID.
        * I. e. we get notified on all received frames with this CAN ID.
        *
        * @param canID    - The CAN ID that should be added to the RX filter.
        * @param isCANFD  - Flag for CANFD frames.
        */
        void rxSetupCanID(canid_t canID, bool isCANFD);

        /**
        * Creates a RX filter for the CAN ID and the relevant bits of the frame.
        * I. e. we only get notified on changes for the set bits in the mask.
        *
        * @param canID    - The CAN ID that should be added to the RX filter.
        * @param mask     - The mask for the relevant bits of the frame.
        * @param isCANFD  - Flag for CANFD frames.
        */
        void rxSetupMask(canid_t canID, struct canfd_frame mask, bool isCANFD);

        /**
        * Removes the RX filter for the given CAN ID.
        *
        * @param canID   - The CAN ID that should be removed from the RX filter.
        * @param isCANFD - Flag for CANFD frames.
        */
        void rxDelete(canid_t canID, bool isCANFD);

        // Data members
        boost::shared_ptr<boost::asio::io_context> ioContext;                           /**< The io_context used by the BCM socket.           */
        boost::asio::generic::datagram_protocol::socket bcmSocket;                      /**< The BCM socket that is used to send and receive. */
        std::array<std::uint8_t, sizeof(struct bcmMsgMultipleFramesCanFD)> rxBuffer{0}; /**< Buffer that stores the received data.            */
        std::thread ioContextThread;                                                    /**< Thread for the io_context loop.                  */
        CANConnectorConfig config;

    };

}


#endif //SIM_TO_DUT_INTERFACE_CANCONNECTOR_H
/*******************************************************************************
 * END OF FILE
 ******************************************************************************/
