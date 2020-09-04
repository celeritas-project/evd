//---------------------------------*-C++-*-----------------------------------//
//! \file   Evd/main.cpp
//! \author Stefano Tognini
//! \note   Copyright (c) 2020 Oak Ridge National Laboratory, UT-Battelle, LLC
//---------------------------------------------------------------------------//
//! \brief  Event Display for the Celeritas Project
//---------------------------------------------------------------------------//

#include <iostream>
#include "Evd.hh"

int main(int argc, char* argv[])
{
    //! If no arguments, print help
    if (argc == 1)
    {
        std::cout << "Check README.md for information." << std::endl;
        return 1;
    }

    //! Fetch all terminal inputs
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

        if (arg_i == "-vis")
        {
            visLevel = std::stoi(argv[i + 1]);
            i        = i + 1;
        }
        else if (arg_i == "-e")
        {
            evt = std::stoi(argv[i + 1]);
            i   = i + 1;
        }
        else if (arg_i == "-n")
        {
            ntracks = std::stoi(argv[i + 1]);
            i       = i + 1;
        }
        else if (arg_i == "-cms")
        {
            isCMS = true;
        }
        else if (arg_i == "-noworld")
        {
            noWorld = true;
        }
        else if (arg_i.length() > 4
                 && arg_i.substr(arg_i.length() - 4) == "gdml")
        {
            geometryFile = argv[i];
        }
        else if (arg_i.length() > 4
                 && arg_i.substr(arg_i.length() - 4) == "root")
        {
            rootFile = argv[i];
        }
        else
        {
            std::cout << "[WARNING] Parameter " << arg_i << " not known. ";
            std::cout << "Skipping..." << std::endl;
        }
    }
    std::cout << std::endl;

    if (!geometryFile)
    {
        std::cout << "[ERROR] No GDML file specified. ";
        std::cout << "Check README.md for information." << std::endl;
        return 2;
    }

    //! Open Evd
    Evd evd(geometryFile, rootFile);

    //! If -cms flag is used, call the tailored cms method
    if (isCMS)
    {
        evd.AddCMSVolume(evd.GetTopVolume());
    }

    else
    {
        //! If no flag, also draw the world volume
        if (!noWorld)
        {
            evd.AddWorldVolume();
        }

        //! if -noworld is used, only draw volumes found inside world
        evd.AddVolume(evd.GetTopVolume());
    }

    //! Add simulated event
    if (rootFile)
    {
        evd.AddEvent(evt, ntracks);
    }

    //! Start GUI
    evd.StartViewer();
}
