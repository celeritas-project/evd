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
    bool show_steps{false};

    // Only the GDML input is necessary
    explicit operator bool() const { return !gdml_file.empty(); }
};

//---------------------------------------------------------------------------//
/*!
 * Execute with parsed input.
 */
void run(TerminalInput const& input)
{
    // Initialize main viewer
    MainViewer evd(input.gdml_file);
    evd.set_vis_level(input.vis_level);

    if (input.is_cms)
    {
        // Temporary option to hide cms building
        evd.add_cms_volume();
    }
    else
    {
        evd.add_world_volume();
    }

    if (!input.root_file.empty())
    {
        // Initialize event viewer
        EventViewer event_viewer(input.root_file);
        event_viewer.show_step_points(input.show_steps);
        event_viewer.add_event(input.event_id);
    }

    // Start GUI
    evd.start_viewer();
};

//---------------------------------------------------------------------------//
/*!
 * Parse terminal input parameters.
 */
TerminalInput parse(int argc, char* argv[])
{
    TerminalInput input;

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
        else if (arg_i == "-s")
        {
            // Draw step points
            input.show_steps = true;
        }
        else if (arg_i == "-cms")
        {
            // Temporary: select cms detector only
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
    return input;
}

//---------------------------------------------------------------------------//
/*!
 * Run Celeritas event display based on terminal input options.
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

    auto const input = parse(argc, argv);

    if (!input)
    {
        std::cout << "[ERROR] No GDML file specified. Check README.md for "
                     "information."
                  << std::endl;
        return EXIT_FAILURE;
    }

    run(input);

    return EXIT_SUCCESS;
}
