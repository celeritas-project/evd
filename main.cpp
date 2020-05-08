//---------------------------------*-C++-*-----------------------------------//
/*!
 * \file   Evd/main.cpp
 * \author Stefano Tognini
 * \brief  Event Display for the Celeritas Project.
 * \note   Copyright (c) 2020 Oak Ridge National Laboratory, UT-Battelle, LLC.
 */
//---------------------------------------------------------------------------//


// C++
#include <iostream>

// Project
#include "Evd.hh"


int main(int argc, char* argv[])
{
    //------------------------- The trivial solution ------------------------//
    if (argc == 1)
    {
        std::cout << "Check README.md for information." << std::endl;
        return 0;
    }
    
    
    //--------------------------- Terminal inputs ---------------------------//
    char * geometryFile = nullptr;
    char * rootFile = nullptr;
    int    evt = 0;
    int    ntracks = 1;
    int    visLevel = 1;
    bool   isAll = false;
    
    for (int i = 1; i < argc; i++)
    {
        std::string arg_i(argv[i]);
        
        if (arg_i == "-vis")
        {
            visLevel = std::stoi(argv[i+1]);
            i = i + 1;
        }
        else if (arg_i == "-e")
        {
            evt = std::stoi(argv[i+1]);
            i = i + 1;
        }
        else if (arg_i == "-n")
        {
            ntracks = std::stoi(argv[i+1]);
            i = i + 1;
        }
        else if (arg_i == "-all")
        {
            isAll = true;
        }
        else if (arg_i.length() > 4 &&
                 arg_i.substr(arg_i.length() - 4) == "gdml")
        {
            geometryFile = argv[i];
        }
        else if (arg_i.length() > 4 &&
                 arg_i.substr(arg_i.length() - 4) == "root")
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
        return 1;
    }
    
    
    //--------------------------------- Evd ---------------------------------//
    Evd evd;
    
    evd.LoadGeometry(geometryFile);
    evd.SetVisLevel(visLevel);
    TGeoVolume * topVolume = evd.GetTopVolume();

    if (isAll) evd.AddVolume(topVolume);
    else       evd.AddCMSVolume(topVolume);
    
    if (rootFile)
    {
        evd.LoadRoot(rootFile);
        evd.AddEvent(evt, ntracks);
    }
    
    evd.StartViewer();
}


