//----------------------------------*-C++-*----------------------------------//
// Copyright 2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file src/EventViewer.cc
//---------------------------------------------------------------------------//
#include "EventViewer.hh"

#include <assert.h>

#include "RootDataViewer.hh"

//---------------------------------------------------------------------------//
/*!
 * Construct with ROOT input filename.
 */
EventViewer::EventViewer(std::string root_filename)
{
    tfile_.reset(TFile::Open(root_filename.c_str(), "read"));
    assert(tfile_.IsOpen());

    if (tfile_->Get("events"))
    {
        // Load RootDataViewer
        auto ttree = std::make_shared<TTree>(tfile_->Get("events"));
        viewer_.reset(new RootDataViewer(ttree));
    }
    if (tfile_->Get("steps"))
    {
        // Load RSWViewer
    }

    assert(ttree_);
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
