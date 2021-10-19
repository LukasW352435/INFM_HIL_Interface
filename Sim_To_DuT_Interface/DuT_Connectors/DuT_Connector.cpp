//
// Created by Lukas on 19.10.2021.
//

#include "DuT_Connector.h"

DuT_Connector::DuT_Connector() {

}

DuT_Info DuT_Connector::get_DuT_Info() {
    DuT_Info info(
            "Test Connector",
            0x01000000,
            "DuT_Connector please override!");
    return info;
}