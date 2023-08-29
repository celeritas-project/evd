//----------------------------------*-C++-*----------------------------------//
// Copyright 2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file src/MCTruthViewerInterface.hh
//---------------------------------------------------------------------------//
#pragma once

#include <string>
#include <TEveTrack.h>

//---------------------------------------------------------------------------//
/*!
 * Interface to read any MCtruth data and add it to the Evd.
 *
 * Concrete implementations of this class are expected to be constructed
 * *after* \c MainViewer is initialized, since they would use ROOT's \c gEve
 * singleton to add any track/point to the viewer.
 *
 * \note
 * Maybe expand this to be an interface for hits.
 */
class MCTruthViewerInterface
{
  public:
    enum PDG
    {
        e_plus = -11,
        e_minus = 11,
        mu_minus = 13,
        gamma = 22
    };

    // Default destructor
    virtual ~MCTruthViewerInterface() = default;

    // Mandatory function to add tracks from a given event to Eve
    virtual void add_event(int event_id) = 0;

    // Draw step points along the track
    void show_step_points(bool value);

    // Convert PDG to string
    std::string to_string(PDG id);

    // Set up track attributes
    void set_track_attributes(TEveLine* track, PDG pdg);

  protected:
    // Allow construction only from concrete implementations
    MCTruthViewerInterface() = default;

  private:
    bool step_points_{false};
};
