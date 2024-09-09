//----------------------------------*-C++-*----------------------------------//
// Copyright 2023 UT-Battelle, LLC, and other Celeritas developers.
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
 * The level of details is defined by \c set_vis_level(...) and should be
 * invoked before starting the viewer.
 *
 * \code
 *  MainViewer evd("geometry.gdml");
 *  evd.set_vis_level(3);
 *  evd.add_world_volume();
 *  evd.start_viewer();
 * \endcode
 */
class MainViewer
{
  public:
    // Construct with gdml
    MainViewer(std::string gdml_input);

    // Add World volume
    void add_world_volume();

    // Set the visualization level
    void set_vis_level(int vis_level);

    // Start Evd GUI
    void start_viewer();

    // Extra function tailored for the CMS geometry
    void add_cms_volume();

  private:
    //// DATA ////

    int vis_level_;
    std::unique_ptr<TRint> root_app_;

    //// HELPER FUNCTIONS ////

    TGeoVolume* top_volume();
    void init_projections_tab();
    void spawn_viewer(TEveWindowSlot& slot,
                      std::string title,
                      TGLViewer::ECameraType camera);
};
