//---------------------------------*-C++-*-----------------------------------//
//! \file   Evd/src/Evd.hh
//! \author Stefano Tognini
//! \note   Copyright (c) 2020 Oak Ridge National Laboratory, UT-Battelle, LLC
//---------------------------------------------------------------------------//
//! \brief  Event Display for the Celeritas Project
//---------------------------------------------------------------------------//
#pragma once

#include <memory>

class TRint;
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
 * Sub volumes can be included manually by providing a given TGeoVolume. One 
 * can avoid drawing the full world volume, and only drawing the volumes found
 * inside it by doing
 * \code
 *  Evd evd("geometry.gdml", nullptr);
 *  evd.AddVolume(evd.GetTopVolume());
 *  evd.StartViewer();
 * \endcode
 *
 * Specific TGeoVolumes can be fetched with \c GetVolumeNode(...).
 * A list of available nodes inside a TGeoVolume can be loaded through
 * \c GetNodeList(...). 
 * The level of details drawn is defined by \c SetVisLevel(...).
 * 
 * Class is expected to be expanded to read Celeritas output events.
 */
class Evd
{
  public:
    //! Construct with user-defined inputs
    //! simulation_input is not necessary and can be passed as nullptr
    Evd(const char* gdml_input, const char* simulation_input);
    ~Evd();

    //! Return top volume found in the geometry
    TGeoVolume* GetTopVolume();
    //! Return a given node within a given TGeoVolume
    TGeoVolume* GetVolumeNode(TGeoVolume* geoVolume, const char* node);
    //! Return the list of node names found in a given TGeoVolume
    std::vector<std::string> GetNodeList(TGeoVolume* geoVolume);

    //! Add World volume to the Evd Viewer
    void AddWorldVolume();
    //! Add volume to the Evd viewer
    void AddVolume(TGeoVolume* geoVolume);
    //! Extra function tailored for the CMS geometry
    void AddCMSVolume(TGeoVolume* geoVolume);
    //! Add simulated event to the Evd
    void AddEvent(const int& event, const int& trackLimit);

    //! Change the visualization level (higher values -> more details)
    void SetVisLevel(const int& visLevel);

    //! Start Evd GUI
    void StartViewer();

  private:
    //! Splash logo
    void PrintEvdLogo();

    //! Internal loading functions
    void LoadGeometry(const char* gdml_input);
    void LoadSimulation(const char* simulation_input);
    void StartOrthoViewer();

  private:
    std::unique_ptr<TRint> root_app_;
    std::unique_ptr<TFile> root_file_;
    int                    vis_level_;
    bool                   has_elements_;
    const double           cm = 10; // TGeoManager uses cm while GDML uses mm
};
