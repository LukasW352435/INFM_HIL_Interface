/*******************************************************************************
 \project   INFM_HIL_Interface
 \file      Dummy_CANFD_Config.h
 \brief     Defines the adjustable communication parameters.
 \author    Matthias Bank
 \version   1.0.0
 \date      24.10.2021
 ******************************************************************************/
#ifndef DUMMY_CANFD_CONFIG_H
#define DUMMY_CANFD_CONFIG_H


/*******************************************************************************
 * DEFINES
 ******************************************************************************/
#define FRAMEID      0x222      // The changed ID of the frames we are sending back

#define CANFD        0          // Flag for enabling CANFD
#define INTERFACE    "vcan0"    // The name of the interface that should be configured

#define VERBOSE      1          // Enables printing information during the recv-send loop


#endif //DUMMY_CANFD_CONFIG_H


/*******************************************************************************
 * END OF FILE
 ******************************************************************************/