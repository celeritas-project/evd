//----------------------------------*-C++-*----------------------------------//
// Copyright 2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file src/RSWViewer.hh
//---------------------------------------------------------------------------//
#pragma once

#include <memory>
#include <string>

#include "MCTruthViewerInterface.hh"
#include "RootUniquePtr.hh"

//---------------------------------------------------------------------------//
/*!
 * Draw event MC truth data from the benchmarks/geant4-validation-app.
 *
 * This is a secondary class, meant to be used along with \c MainViewer , which
 * *MUST* be initialized before this class is constructed.
 */
class RSWViewer final : public MCTruthViewerInterface
{
  public:
    //!@{
    //! \name Type aliases
    using UPTFile = UPRootExtern<TFile>;
    using UPTTree = UPRootExtern<TTree>;
    //!@}

    // Construct with ROOT input file
    RSWViewer(UPTFile tfile);

    // Add tracks for given event
    void add_event(int event_id) override;

  private:
    //// DATA ////

    UPTFile tfile_;
    UPTTree ttree_;
    long long* sorted_tree_index_;

    //// HELPER FUNCTIONS ////

    // Loop over tracks and add track lines to Eve
    void create_event_tracks(int const event_id);
};
