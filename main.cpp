//----------------------------------*-C++-*----------------------------------//
// Copyright 2020-2022 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file main.cpp
//! \brief Geometry and event display for Celeritas.
//---------------------------------------------------------------------------//
#include <iostream>
#include "Evd.hh"

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        // No arguments, print help
        std::cout << "Check README.md for information." << std::endl;
        return EXIT_FAILURE;
    }

    // Fetch all terminal inputs
    std::string gdml_file;
    std::string root_file;
    int         evt{0};
    int         vis_level{1};
    bool        is_cms{false};

    // >>> Loop over arguments
    for (int i = 1; i < argc; i++)
    {
        std::string arg_i(argv[i]);

        if (arg_i == "-vis")
        {
            // Set vis level
            vis_level = std::stoi(argv[i + 1]);
            i++; // Skip one entry since we already read the value at i + 1
        }

        else if (arg_i == "-e")
        {
            // Set event number
            evt = std::stol(argv[i + 1]);
            i++; // Skip one entry since we already read the value at i + 1
        }

        else if (arg_i == "-cms")
        {
            // Select cms option
            is_cms = true;
        }

        else if (arg_i.length() > 4
                 && arg_i.substr(arg_i.length() - 4) == "gdml")
        {
            // Fetch gdml file
            gdml_file = argv[i];
        }

        else if (arg_i.length() > 4
                 && arg_i.substr(arg_i.length() - 4) == "root")
        {
            // Fetch root simulation file
            root_file = argv[i];
        }

        else
        {
            // Skip unknown parameters
            std::cout << "[WARNING] Parameter " << arg_i
                      << " not known. Skipping..." << std::endl;
        }
    }

    if (gdml_file.empty())
    {
        // No gdml file found, stop
        std::cout << "[ERROR] No GDML file specified. ";
        std::cout << "Check README.md for information." << std::endl;
        return 2;
    }

    // >>> Initialize Evd
    Evd evd(gdml_file, root_file);
    evd.SetVisLevel(vis_level);

    if (is_cms)
    {
        // -cms flag used
        evd.DrawCMSVolume();
    }

    if (!root_file.empty())
    {
        // Add simulated event
        evd.AddEvent(evt);
    }

    evd.StartViewer();

    return EXIT_SUCCESS;
}
