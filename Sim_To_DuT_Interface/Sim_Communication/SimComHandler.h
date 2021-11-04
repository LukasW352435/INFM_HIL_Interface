//
// Created by Lukas on 19.10.2021.
//

#ifndef SIM_TO_DUT_INTERFACE_SIM_COM_HANDLER_H
#define SIM_TO_DUT_INTERFACE_SIM_COM_HANDLER_H

#include <memory>
#include "../Events/SimEvent.h"
#include "../Sim_To_DuT_Interface.h"
#include <string>
#include "../Utility/SharedQueue.h"

class Sim_Com_Handler {
public:
    Sim_Com_Handler();
    void send_Event_to_Sim(SimEvent &event);
    void run();
private:
    void send_Event_to_Interface(SimEvent &event);
    std::shared_ptr<SharedQueue<SimEvent>> queueEventToInterface;
};

#endif //SIM_TO_DUT_INTERFACE_SIM_COM_HANDLER_H
