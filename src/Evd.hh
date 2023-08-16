//----------------------------------*-C++-*----------------------------------//
// Copyright 2020-2022 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file Evd.hh
//! \brief A geometry and event display viewer.
//---------------------------------------------------------------------------//
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <TRint.h>
#include <TEveManager.h>
#include <TFile.h>
#include <TGeoVolume.h>
#include <TEveTrack.h>

#include "RootData.hh"

//---------------------------------------------------------------------------//
/*!
 * Evd is built using the Eve Environment [J. Phys.: Conf. Ser. 219 042055].
 *
 * It loads a gdml geometry and (optionally) a simulation output. To view only
 * the geometry, the simulation output can be passed as a \c nullptr . Current
 * implementation is compatible with benckmarks/geant4-validation-app/ and
 * Celeritas RootStepWriter outputs.
 *
 * Example usage for viewing a gdml file with a simulated event:
 * \code
 *  Evd evd("geometry.gdml", "sim.root");
 *  evd.AddEvent(event_number);
 *  evd.StartViewer();
 * \endcode
 *
 * Sub-volumes can be manually included by providing a given \c TGeoVolume. One
 * can avoid drawing the full world volume, and only drawing the volumes found
 * inside it by doing
 * \code
 *  Evd evd("geometry.gdml", nullptr);
 *  evd.AddVolume(evd.GetTopVolume());
 *  evd.StartViewer();
 * \endcode
 *
 * A list of available nodes inside a \c TGeoVolume can be loaded through
 * \c GetNodeList(...) .
 * The level of details is defined by \c SetVisLevel(...) , which should be
 * invoked before adding volumes.
 */
class Evd
{
  public:
    // Construct with gdml. The simulation_input can be a nullptr
    Evd(std::string gdml_input, std::string simulation_input);

    // Add volume to the Evd viewer
    void AddVolume(TGeoVolume* geo_volume);

    // Extra function tailored for the CMS geometry
    void DrawCMSVolume();

    // Add simulated event from a MC truth file input
    // If negative, all events are drawn
    void AddEvent(const int event_id);

    // Change the visualization level (higher values == more details)
    void SetVisLevel(int vis_level);

    // Start Evd GUI
    void StartViewer();

    // Return gEve reference
    TEveManager& GetEveManager();
    // Return top volume found in the geometry
    TGeoVolume* GetTopVolume();
    // Return the list of node names found in a given TGeoVolume
    std::vector<std::string> GetNodeList(TGeoVolume* geo_volume);

  private:
    std::unique_ptr<TRint> root_app_;
    std::unique_ptr<TFile> root_file_;         //!< Event data
    long long*             sorted_tree_index_; //!< Sorted RSW steps tree
    int                    vis_level_;

    enum PDG
    {
        e_plus   = -11,
        e_minus  = 11,
        mu_minus = 13,
        gamma    = 22
    };

  private:
    using FirstLastIdMap = std::pair<int, int>;

    // Add World volume to the Evd Viewer
    void AddWorldVolume();
    // Initialize projections tab
    void StartOrthoViewer();
    // Add and event from a celeritas::RootStepWriter input file
    void AddRswEvent(const int event_id);
    // Add and event from a celeritas::RootStepWriter input file
    void AddRootDataEvent(const int event_id);
    // Return a pair of first/last event index
    FirstLastIdMap SetEventIdLimits(TTree* tree, const int event_id);
    // Create track line
    std::unique_ptr<TEveLine>
    CreateTrackLine(const rootdata::Track& track, const int event_id);
    // Loop over all rootdata::track and generate track lines
    void CreateEventTracks(const std::vector<rootdata::Track>& vec_tracks,
                           const int                           event_id);
    // Loop over the RSW steps tree and generate track lines
    void CreateEventTracks(TTree* steps_tree, const int event_id);

    // Convert PDG to string
    std::string to_string(PDG id);
    // Set up track attributes (currently color only)
    void set_track_attributes(TEveLine* track, PDG pdg);
};
