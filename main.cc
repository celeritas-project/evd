//----------------------------------*-C++-*----------------------------------//
// Copyright 2020-2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file main.cc
//! \brief Geometry and event display for Celeritas.
//---------------------------------------------------------------------------//
#include <iostream>
#include <string>

#include "EventViewer.hh"
#include "MainViewer.hh"

//---------------------------------------------------------------------------//
/*!
 * Terminal input options.
 */
struct TerminalInput
{
    std::string gdml_file;
    std::string root_file;
    std::size_t event_id{0};
    int vis_level{1};
    bool is_cms{false};
};

//---------------------------------------------------------------------------//
/*!
 * Execute with parsed inputs.
 */
static void run(TerminalInput& input)
{
    // Initialize main viewer
    MainViewer evd(input.gdml_file);
    evd.SetVisLevel(input.vis_level);

    if (input.is_cms)
    {
        // -cms flag used
        evd.AddCMSVolume();
    }
    else
    {
        // Draw volumes found inside world
        evd.AddWorldVolume();

        auto node = evd.GetTopVolume()->FindNode("test");
    }

    // Initialize event viewer

    if (!input.root_file.empty())
    {
        EventViewer event_viewer(input.root_file);
        event_viewer.AddEvent(input.event_id);
    }

    // Start GUI
    evd.StartViewer();
};

//---------------------------------------------------------------------------//
/*!
 * Run the event display based on input options.
 * See README for details.
 */
int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        // No arguments, print help
        std::cout << "Check README.md for information." << std::endl;
        return EXIT_FAILURE;
    }

    TerminalInput input;

    // >>> Loop over arguments
    for (int i = 1; i < argc; i++)
    {
        std::string arg_i(argv[i]);

        if (arg_i == "-vis")
        {
            // Set vis level
            input.vis_level = std::stoi(argv[i + 1]);
            i++;
        }

        else if (arg_i == "-e")
        {
            // Set event number
            input.event_id = std::stol(argv[i + 1]);
            i++;
        }

        else if (arg_i == "-cms")
        {
            // Select cms option
            input.is_cms = true;
        }

        else if (arg_i.length() > 4
                 && arg_i.substr(arg_i.length() - 4) == "gdml")
        {
            // Fetch gdml file
            input.gdml_file = argv[i];
        }

        else if (arg_i.length() > 4
                 && arg_i.substr(arg_i.length() - 4) == "root")
        {
            // Fetch root simulation file
            input.root_file = argv[i];
        }

        else
        {
            // Skip unknown parameters
            std::cout << "[WARNING] Parameter " << arg_i
                      << " not known. Skipping..." << std::endl;
        }
    }

    if (input.gdml_file.empty())
    {
        // No gdml file found, stop
        std::cout << "[ERROR] No GDML file specified. ";
        std::cout << "Check README.md for information." << std::endl;
        return EXIT_FAILURE;
    }

    run(input);

    return EXIT_SUCCESS;
}
