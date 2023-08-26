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

#include "MCTruthViewerInterface.hh"
#include "RootData.hh"
#include "RootUniquePtr.hh"

//---------------------------------------------------------------------------//
/*!
 * Draw event MC truth data from the benchmarks/geant4-validation-app.
 *
 * This is a secondary class, meant to be used along with \c MainViewer , which
 * *MUST* be initialized before this class is constructed.
 */
class RootDataViewer final : public MCTruthViewerInterface
{
  public:
    //!@{
    //! \name Type aliases
    using UPTFile = UPRootExtern<TFile>;
    using UPTTree = UPRootExtern<TTree>;
    //!@}

    // Construct with ROOT input file
    RootDataViewer(UPTFile tfile);

    // Add tracks for given event
    void add_event(int event_id) override;

  private:
    //// DATA ////

    UPTFile tfile_;
    UPTTree ttree_;

    //// HELPER FUNCTIONS ////

    // Create track line
    std::unique_ptr<TEveLine>
    CreateTrackLine(rootdata::Track const& track, int const event_id);
    // Loop over event tracks and generate track lines
    void create_event_tracks(std::vector<rootdata::Track> const& vec_tracks,
                           int const event_id);
};
