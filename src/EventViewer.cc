//----------------------------------*-C++-*----------------------------------//
// Copyright 2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file src/EventViewer.cc
//---------------------------------------------------------------------------//
#include "EventViewer.hh"

#include <assert.h>

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
        // Load RSWViewer
    }

    std::cout << "Simulation input: " << root_filename << std::endl;
}

//---------------------------------------------------------------------------//
/*!
 * Add event from benchmarks/geant4-validation-app.
 *
 * If event id is negative, all events are drawn.
 */
void EventViewer::add_event(int const event_id)
{
    viewer_->add_event(event_id);
}
