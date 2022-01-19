//----------------------------------*-C++-*----------------------------------//
// Copyright 2021 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file main.cpp
//! \brief Geometry and event display for Celeritas
//---------------------------------------------------------------------------//

#include <iostream>
#include <TFile.h>

#include "Evd.hh"

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        // No arguments, print help
        std::cout << "Check README.md for information." << std::endl;
        return 1;
    }

    // Fetch all terminal inputs
    char* geometryFile = nullptr;
    char* rootFile     = nullptr;
    int   evt          = 0;
    int   ntracks      = 1;
    int   visLevel     = 1;
    bool  isCMS        = false;
    bool  noWorld      = false;

    // >>> Loop over arguments
    for (int i = 1; i < argc; i++)
    {
        std::string arg_i(argv[i]);

        if (arg_i == "-vis")
        {
            // Set vis level
            visLevel = std::stoi(argv[i + 1]);
            i++;
        }

        else if (arg_i == "-e")
        {
            // Set event number
            evt = std::stoi(argv[i + 1]);
            i++;
        }

        else if (arg_i == "-n")
        {
            // Set number of tracks
            ntracks = std::stoi(argv[i + 1]);
            i++;
        }

        else if (arg_i == "-cms")
        {
            // Select cms option
            isCMS = true;
        }

        else if (arg_i == "-noworld")
        {
            // Select noworld option
            noWorld = true;
        }

        else if (arg_i.length() > 4
                 && arg_i.substr(arg_i.length() - 4) == "gdml")
        {
            // Fetch gdml file
            geometryFile = argv[i];
        }

        else if (arg_i.length() > 4
                 && arg_i.substr(arg_i.length() - 4) == "root")
        {
            // Fetch root simulation file
            rootFile = argv[i];
        }

        else
        {
            // Skip unknown parameters
            std::cout << "[WARNING] Parameter " << arg_i
                      << " not known. Skipping..." << std::endl;
        }
    }
    std::cout << std::endl;

    if (!geometryFile)
    {
        // No gdml file found, stop
        std::cout << "[ERROR] No GDML file specified. ";
        std::cout << "Check README.md for information." << std::endl;
        return 2;
    }

    // >>> Initialize Evd
    Evd evd(geometryFile, rootFile);
    evd.SetVisLevel(visLevel);

    if (isCMS)
    {
        // -cms flag used
        evd.AddCMSVolume(evd.GetTopVolume());
    }

    else
    {
        if (!noWorld)
        {
            // -noworld is false, draw the world volume
            evd.AddWorldVolume();
        }
        else
        {
            // Draw volumes found inside world
            evd.AddVolume(evd.GetTopVolume());
        }
    }

    if (rootFile)
    {
        // Add simulated event
        evd.AddEvents();
    }

    // Start GUI
    evd.StartViewer();

    return EXIT_SUCCESS;
}
