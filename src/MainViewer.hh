//----------------------------------*-C++-*----------------------------------//
// Copyright 2020-2022 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file src/MainViewer.hh
//! \brief A geometry and event display viewer.
//---------------------------------------------------------------------------//
#pragma once

#include <memory>
#include <string>
#include <TEveManager.h>
#include <TEveWindow.h>
#include <TGLViewer.h>
#include <TGeoVolume.h>
#include <TRint.h>

//---------------------------------------------------------------------------//
/*!
 * Evd is built using the Eve Environment [J. Phys.: Conf. Ser. 219 042055].
 *
 * The level of details is defined by \c SetVisLevel(...) and should be invoked
 * before adding any volume to the viewer.
 *
 * \code
 *  MainViewer evd("geometry.gdml");
 *  evd.SetVisLevel(3);
 *  evd.AddWorldVolume();
 *  evd.StartViewer();
 * \endcode
 *
 * Drawing only sub-volumes can be done by providing their name.
 * \code
 *  MainViewer evd("geometry.gdml");
 *  evd.SetVisLevel(3);
 *  evd.AddVolume("my_volume");
 *  evd.StartViewer();
 * \endcode
 *
 * \note

 */
class MainViewer
{
  public:
    // Construct with gdml
    MainViewer(std::string gdml_input);

    // Add World volume
    void AddWorldVolume();

    // Add specific volume
    void AddVolume(std::string node_name);

    // Extra function tailored for the CMS geometry
    void AddCMSVolume();

    // Set the visualization level
    void SetVisLevel(int vis_level);

    // Start Evd GUI
    void StartViewer();

    // Return gEve singleton
    TEveManager* GetEveManager();

    // Return top volume of the geometry
    TGeoVolume* GetTopVolume();

  private:
    //// DATA ////

    int vis_level_;
    std::unique_ptr<TRint> root_app_;

    //// HELPER FUNCTIONS ////

    void InitProjectionsTab();
    void SpawnViewer(TEveWindowSlot* slot,
                     std::string title,
                     TGLViewer::ECameraType camera);
};
