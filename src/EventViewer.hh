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

#include "MCTruthViewerInterface.hh"

//---------------------------------------------------------------------------//
/*!
 * Wrapper class to call different concrete implementations of
 * \c MCTruthViewerInterface .
 *
 * This class loads tracks/steps in the \c MainViewer . Since \c MainViewer is
 * responsible for initializing \c TEve , it *MUST* be initialized before
 * invoking this class.
 */
class EventViewer
{
  public:
    // Construct with input filename
    EventViewer(std::string filename);

    // Add event tracks
    void add_event(int event_id);

    // Draw step points along track
    void show_step_points(bool value);

  private:
    std::unique_ptr<MCTruthViewerInterface> viewer_;
};
