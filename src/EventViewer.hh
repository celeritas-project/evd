//----------------------------------*-C++-*----------------------------------//
// Copyright 2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file src/EventViewer.hh
//---------------------------------------------------------------------------//
#pragma once

#include <memory>
#include <string>

#include "RootDataViewer.hh"

//---------------------------------------------------------------------------//
/*!
 * Draw event MC truth data from the benchmarks/geant4-validation-app.
 *
 * This is a secondary class, meant to be used along with \c MainViewer , which
 * *MUST* be initialized before invoking this class.
 */
class EventViewer
{
  public:
    // Construct with ROOT input file
    EventViewer(std::string root_filename);

    // Add tracks for given event
    void add_event(int event_id);

  private:
    std::unique_ptr<MCTruthViewerInterface> viewer_;
};
