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
#include <TEveManager.h>
#include <TEveTrack.h>
#include <TFile.h>
#include <TTree.h>

#include "RootData.hh"

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
    void AddEvent(int event_id);

  private:
    //// DATA ////
    std::unique_ptr<TFile> tfile_;
    std::unique_ptr<TTree> ttree_;
    enum PDG
    {
        e_plus = -11,
        e_minus = 11,
        mu_minus = 13,
        gamma = 22
    };

    //// HELPER FUNCTIONS ////

    // Create track line
    std::unique_ptr<TEveLine>
    CreateTrackLine(rootdata::Track const& track, int const event_id);
    // Loop over event tracks and generate track lines
    void CreateEventTracks(std::vector<rootdata::Track> const& vec_tracks,
                           int const event_id);
    // Convert PDG to string
    std::string to_string(PDG id);
    // Set up track attributes (currently color only)
    void SetTrackAttributes(TEveLine* track, PDG pdg);
};
