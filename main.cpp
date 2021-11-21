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
    // If no arguments, print help
    if (argc == 1)
    {
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

    for (int i = 1; i < argc; i++)
    {
        std::string arg_i(argv[i]);

        // Set vis level
        if (arg_i == "-vis")
        {
            visLevel = std::stoi(argv[i + 1]);
            i        = i + 1;
        }
        // Set event number
        else if (arg_i == "-e")
        {
            evt = std::stoi(argv[i + 1]);
            i   = i + 1;
        }
        // Set number of tracks
        else if (arg_i == "-n")
        {
            ntracks = std::stoi(argv[i + 1]);
            i       = i + 1;
        }
        // Select cms option
        else if (arg_i == "-cms")
        {
            isCMS = true;
        }
        // Select noworld option
        else if (arg_i == "-noworld")
        {
            noWorld = true;
        }
        // Fetch gdml file
        else if (arg_i.length() > 4
                 && arg_i.substr(arg_i.length() - 4) == "gdml")
        {
            geometryFile = argv[i];
        }
        // Fetch root simulation file
        else if (arg_i.length() > 4
                 && arg_i.substr(arg_i.length() - 4) == "root")
        {
            rootFile = argv[i];
        }
        // Skip unknown parameters
        else
        {
            std::cout << "[WARNING] Parameter " << arg_i << " not known. ";
            std::cout << "Skipping..." << std::endl;
        }
    }
    std::cout << std::endl;

    // If no gdml file is found, stop
    if (!geometryFile)
    {
        std::cout << "[ERROR] No GDML file specified. ";
        std::cout << "Check README.md for information." << std::endl;
        return 2;
    }

    // Open Evd
    Evd evd(geometryFile, rootFile);
    evd.SetVisLevel(visLevel);

    //! If -cms flag is used, call the tailored cms method
    if (isCMS)
    {
        evd.AddCMSVolume(evd.GetTopVolume());
    }

    else
    {
        // If -noworld is false, draw the world volume
        if (!noWorld)
            evd.AddWorldVolume();
        else
            // Draw volumes found inside world
            evd.AddVolume(evd.GetTopVolume());
    }

    // Add simulated event
    if (rootFile)
    {
        evd.AddEvents();
    }

    // Start GUI
    evd.StartViewer();

    return EXIT_SUCCESS;
}
