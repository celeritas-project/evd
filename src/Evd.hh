//----------------------------------*-C++-*----------------------------------//
// Copyright 2021 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file Evd.hh
//! \brief Evd singleton
//---------------------------------------------------------------------------//
#pragma once

#include <string>
#include <vector>
#include <memory>

class TRint;
class TEveManager;
class TFile;
class TGeoVolume;

//---------------------------------------------------------------------------//
/*!
 * Evd is built using the Eve Environment [J. Phys.: Conf. Ser. 219 042055].
 *
 * Evd loads a gdml geometry and (optionally) a simulation root output file
 * in order to display detector geometry with simulated events.
 *
 * Example usage for viewing the full geometry of a given gdml file
 * \code
 *  Evd evd("geometry.gdml", nullptr);
 *  evd.AddWorldVolume();
 *  evd.StartViewer();
 * \endcode
 *
 * Sub-volumes can be manually included by providing a given TGeoVolume. One
 * can avoid drawing the full world volume, and only drawing the volumes found
 * inside it by doing
 * \code
 *  Evd evd("geometry.gdml", nullptr);
 *  evd.AddVolume(evd.GetTopVolume());
 *  evd.StartViewer();
 * \endcode
 *
 * A list of available nodes inside a TGeoVolume can be loaded through
 * \c GetNodeList(...).
 * The level of details drawn is defined by \c SetVisLevel(...), which should
 * be invoked before adding volumes.
 */
class Evd
{
  public:
    // Construct with user-defined inputs
    // simulation_input is not necessary and can be passed as nullptr
    Evd(const char* gdml_input, const char* simulation_input);
    ~Evd();

    // Add World volume to the Evd Viewer
    void AddWorldVolume();
    // Add volume to the Evd viewer
    void AddVolume(TGeoVolume* geoVolume);
    // Extra function tailored for the CMS geometry
    void AddCMSVolume(TGeoVolume* geoVolume);
    // Add simulated events to the Evd (benchmark/geant4-validation-app)
    void AddEvents();
    // Change the visualization level (higher values == more details)
    void SetVisLevel(const int visLevel);
    // TO BE ADDED
    void FindVolume(TGeoVolume& volume, std::string volume_name);
    // Return gEve reference
    TEveManager& GetEveManager();

    void AddVolume(std::string volume_name);

    // Start Evd GUI
    void StartViewer();

    // Return top volume found in the geometry
    TGeoVolume* GetTopVolume();
    // Return the list of node names found in a given TGeoVolume
    std::vector<std::string> GetNodeList(TGeoVolume* geoVolume);

  private:
    // Internal loading functions
    void LoadGeometry(const char* gdml_input);
    void LoadSimulation(const char* simulation_input);
    void StartOrthoViewer();

  private:
    std::unique_ptr<TRint> root_app_;
    std::unique_ptr<TFile> root_file_;
    int                    vis_level_;
    bool                   has_elements_;

  private:
    enum PDG
    {
        pdg_e_plus  = -11,
        pdg_e_minus = 11,
        pdg_gamma   = 22
    };
};
