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

#include "RSWViewer.hh"
#include "RootDataViewer.hh"

//---------------------------------------------------------------------------//
/*!
 * Construct with ROOT input filename.
 */
EventViewer::EventViewer(std::string root_filename)
{
    UPRootExtern<TFile> tfile;
    tfile.reset(TFile::Open(root_filename.c_str(), "read"));
    assert(tfile->IsOpen());

    if (tfile->Get("events"))
    {
        viewer_.reset(new RootDataViewer(std::move(tfile)));
    }

    else if (tfile->Get("steps"))
    {
        viewer_.reset(new RSWViewer(std::move(tfile)));
    }

    else
    {
        std::cout << "[ERROR] " << root_filename << " has no known TTrees"
                  << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Simulation input: " << root_filename << std::endl;
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
