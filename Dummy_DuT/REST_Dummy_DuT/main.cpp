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

#include <iostream>
#include <iterator>
#include <fstream>
#include <filesystem>

#include <cstdlib>
#include <restbed>
#include "apiImpl/DefaultApiImpl.h"

using namespace restbed;

#define PORT 9090

/**
 * <summary>
 * Helper struct to parse config line by line
 * </summary>
 */
struct Line {
    std::string data;

    explicit operator std::string const &() const { return data; }

    friend std::istream &operator>>(std::istream &stream, Line &line) {
        return std::getline(stream, line.data);
    }
};

int main(const int, const char **) {
    auto path = std::filesystem::canonical("/proc/self/exe").parent_path().string();
    std::cout << "Reading config from " << path << std::endl;
    std::ifstream file(path + "/config");
    std::set<std::string> operations(std::istream_iterator<Line>{file}, std::istream_iterator<Line>{});
    for (const auto &op: operations) {
        std::cout << "OP: " << op << std::endl;
    }

    std::cout << "Starting on port " << PORT << std::endl;
    auto defaultAPI = new dummy_dut::rest::impl::DefaultApiImpl(PORT, operations);

    return EXIT_SUCCESS;
}
