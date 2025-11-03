//----------------------------------*-C++-*----------------------------------//
// Copyright 2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file src/EventViewer.cc
//---------------------------------------------------------------------------//
#include "EventViewer.hh"

#include <assert.h>
#include <stdlib.h>

#include "JsonViewer.hh"
#include "RSWViewer.hh"
#include "RootDataViewer.hh"

//---------------------------------------------------------------------------//
/*!
 * Construct with input filename.
 *
 * During construction only *ONE* of the concrete implementations of
 * \c MCTruthViewerInterface is selected.
 */
EventViewer::EventViewer(std::string filename)
{
    assert(filename.size() > 4);

    if (filename.substr(filename.length() - 4) == "root")
    {
        // ROOT input
        UPRootExtern<TFile> tfile;
        tfile.reset(TFile::Open(filename.c_str(), "read"));
        assert(tfile->IsOpen());

        if (tfile->Get("events"))
        {
            viewer_.reset(new RootDataViewer(std::move(tfile)));
        }
        else if (tfile->Get("steps"))
        {
            viewer_.reset(new RSWViewer(std::move(tfile)));
        }
    }
    else if (filename.substr(filename.length() - 5) == "jsonl")
    {
        // JSON input
        viewer_.reset(new JsonViewer(filename));
    }
    else
    {
        std::cout << "[ERROR] " << filename << " has no known reader class"
                  << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Simulation input: " << filename << std::endl;
}

//---------------------------------------------------------------------------//
/*!
 * Call concrete add event function.
 */
void EventViewer::add_event(int const event_id)
{
    viewer_->add_event(event_id);
}

//---------------------------------------------------------------------------//
/*!
 * Show/hide step points along tracks.
 */
void EventViewer::show_step_points(bool value)
{
    viewer_->show_step_points(value);
}
