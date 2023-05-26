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
 * implementation is compatible with benckmarks/geant4-validation-app/ outputs.
 *
 * Example usage for viewing a gdml file with a simulated event:
 * \code
 *  Evd evd("geometry.gdml", "sim.root");
 *  evd.AddWorldVolume();
 *  evd.AddEvent(event_number);
 *  evd.StartViewer();
 * \endcode
 *
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
    Evd(const char* gdml_input, const char* simulation_input);

    // Add World volume to the Evd Viewer
    void AddWorldVolume();
    // Add volume to the Evd viewer
    void AddVolume(TGeoVolume* geo_volume);
    // Extra function tailored for the CMS geometry
    void AddCMSVolume(TGeoVolume* geo_volume);
    // Add simulated event. If negative, all events are drawn
    void AddEvent(std::size_t event_idx);
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
    std::unique_ptr<TFile> root_file_;
    int                    vis_level_;
    bool                   has_elements_;

    enum PDG
    {
        e_plus   = -11,
        e_minus  = 11,
        mu_minus = 13,
        gamma    = 22
    };

  private:
    // Load geometry with selected visualization level
    void LoadGeometry(const char* gdml_input);
    // Initialize projections tab
    void StartOrthoViewer();
    // Create track line
    std::unique_ptr<TEveLine>
    CreateTrackLine(const rootdata::Track& track, const std::size_t event_id);
    // Loop over event tracks and generate track lines
    void CreateEventTracks(const std::vector<rootdata::Track>& vec_tracks,
                           std::size_t                         event_id);
    // Convert PDG to string
    std::string to_string(PDG id);
    // Set up track attributes (currently color only)
    void set_track_attributes(TEveLine* track, PDG pdg);
};
