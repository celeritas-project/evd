//----------------------------------*-C++-*----------------------------------//
// Copyright 2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file src/RootDataViewer.hh
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
 * *MUST* be initialized before this class is constructed.
 */
class RootDataViewer : public MCTruthViewerInterface
{
  public:
    //!@{
    //! \name Type aliases
    using SPTree = std::shared_ptr<TTree>
    //!@}
    // Construct with ROOT input file
    RootDataViewer(SPTree event_tree);

    // Add tracks for given event
    void add_event(int event_id) final;

  private:
    //// DATA ////
    std::shared_ptr<TTree> ttree_;

    //// HELPER FUNCTIONS ////

    // Create track line
    std::unique_ptr<TEveLine>
    CreateTrackLine(rootdata::Track const& track, int const event_id);
    // Loop over event tracks and generate track lines
    void CreateEventTracks(std::vector<rootdata::Track> const& vec_tracks,
                           int const event_id);
};
