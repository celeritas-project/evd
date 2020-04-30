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
    const char* geometryFile = nullptr;
    const char* rootFile = nullptr;
    int evt = 0;
    int ntracks = 1;
    int visLevel = 1;
    bool isAll = false;
    
    for (int i = 1; i < argc; i++)
    {
        std::string arg_i(argv[i]);
        
        if (arg_i == "-l")
        {
            geometryFile = argv[i+1];
        }
        if (arg_i == "-vis")
        {
            visLevel = std::stoi(argv[i+1]);
        }
        if (arg_i == "-r")
        {
            rootFile = argv[i+1];
        }
        if (arg_i == "-e")
        {
            evt = std::stoi(argv[i+1]);
        }
        if (arg_i == "-n")
        {
            ntracks = std::stoi(argv[i+1]);
        }
        if (arg_i == "-all")
        {
            isAll = true;
        }
    }
    
    if (!geometryFile)
    {
        std::cout << "[ERROR] No GDML file specified. ";
        std::cout << "Type -l [geometry.gdml] to load one." << std::endl;
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
        evd.LoadROOT(rootFile);
        evd.AddEvent(evt, ntracks);
    }
    
    evd.StartViewer();
}


