//----------------------------------*-C++-*----------------------------------//
// Copyright 2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file src/MainViewer.cc
//---------------------------------------------------------------------------//
#include "MainViewer.hh"

#include <iostream>
#include <vector>
#include <TEveBrowser.h>
#include <TEveGeoNode.h>
#include <TEveViewer.h>
#include <TGeoManager.h>
#include <TGeoNode.h>
#include <TObjArray.h>
#include <TObject.h>
#include <assert.h>

//---------------------------------------------------------------------------//
/*!
 * Construct with gdml geometry input.
 */
MainViewer::MainViewer(std::string gdml_input) : vis_level_(1)
{
    root_app_.reset(new TRint("evd", nullptr, nullptr, nullptr, 0, true));
    root_app_->SetPrompt("evd [%d] ");

    // TEveManager creates a gEve pointer owned by ROOT
    TEveManager::Create();
    TGeoManager::SetVerboseLevel(0);
    TGeoManager::Import(gdml_input.c_str());
    std::cout << "Geometry input: " << gdml_input << std::endl;
}

//---------------------------------------------------------------------------//
/*!
 * Add World volume to the viewer.
 */
void MainViewer::add_world_volume()
{
    assert(gGeoManager->GetTopVolume());

    auto eve_node = new TEveGeoTopNode(gGeoManager, gGeoManager->GetTopNode());
    eve_node->SetVisOption(0);
    eve_node->SetVisLevel(vis_level_);
    gEve->AddGlobalElement(eve_node);
}

//---------------------------------------------------------------------------//
/*!
 * Function tailored to better display the CMS detector (cms2018.gdml).
 * It skips CMS surrounding building and set some LHC parts as invisible.
 */
void MainViewer::add_cms_volume()
{
    auto* cmse_node = this->top_volume()->FindNode("CMSE0x7f4a8f616d40");
    if (!cmse_node)
    {
        // Not cms2018.gdml, skip
        std::cout << "[Warning] Not the CMS 2018 geometry" << std::endl;
        this->add_world_volume();
        return;
    }

    std::cout << "CMS building and LHC elements are set to invisible"
              << std::endl;

    // List of elements set as invisible
    char const* const invisible_node_list[] = {"CMStoZDC0x7f4a9a757000",
                                               "ZDCtoFP4200x7f4a9a757180",
                                               "BEAM30x7f4a8f615040",
                                               "BEAM20x7f4a9a75ae00",
                                               "VCAL0x7f4a8f615540",
                                               "CastorF0x7f4a8f615f80",
                                               "CastorB0x7f4a8f616080",
                                               "TotemT20x7f4a8f615ac0",
                                               "OQUA0x7f4a8f616600",
                                               "BSC20x7f4a8f616740",
                                               "ZDC0x7f4a8f6168c0"};

    // Set selected elements as invisible
    auto* cmse_vol = cmse_node->GetVolume();
    for (auto const& node : invisible_node_list)
    {
        auto* cmse_subvol = cmse_vol->FindNode(node)->GetVolume();
        cmse_subvol->InvisibleAll();
        cmse_subvol->SetVisDaughters(false);
    }

    auto* cmse_top_node = new TEveGeoTopNode(gGeoManager, cmse_node);
    cmse_top_node->SetVisLevel(vis_level_);
    gEve->AddGlobalElement(cmse_top_node);
}

//---------------------------------------------------------------------------//
/*!
 * Set the level of details.
 * It is the number of levels deep in which daughter volumes are drawn.
 */
void MainViewer::set_vis_level(int vis_level)
{
    vis_level_ = vis_level;
}

//---------------------------------------------------------------------------//
/*!
 * Start MainViewer GUI.
 */
void MainViewer::start_viewer()
{
    gEve->GetBrowser()->TRootBrowser::SetWindowName("Celeritas Event Display");
    gEve->GetDefaultViewer()->SetElementName("Main viewer");
    gEve->GetBrowser()->HideBottomTab();
    gEve->GetDefaultGLViewer()->GetClipSet()->SetClipType(TGLClip::EType(0));

    // Build 2nd tab with orthogonal viewers
    this->init_projections_tab();
    gEve->FullRedraw3D(true);

    // Return focus to the main viewer
    gEve->GetDefaultGLViewer();

    // TODO: add option to update z-axis pointing upwards or to the right
    // gEve->GetDefaultGLViewer()->SetCurrentCamera(TGLViewer::kCameraPerspXOY);

    std::cout << std::endl;
    root_app_->Run();
    root_app_->Terminate(EXIT_SUCCESS);
}

//---------------------------------------------------------------------------//
// PRIVATE
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
/*!
 * Return the top volume of the geometry file.
 */
TGeoVolume* MainViewer::top_volume()
{
    return gGeoManager->GetTopVolume();
}

//---------------------------------------------------------------------------//
/*!
 * Create ortho viewers (2nd tab in the GUI).
 */
void MainViewer::init_projections_tab()
{
    // Create top window to contain all 4 slots
    TEveWindowSlot* slot
        = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
    TEveWindowPack* pack_master = slot->MakePack();
    pack_master->SetElementName("Projections");
    pack_master->SetHorizontal();
    pack_master->SetShowTitleBar(false);

    // Create slots on the left side
    slot = pack_master->NewSlot();
    auto pack_left = slot->MakePack();
    auto slot_left_top = pack_left->NewSlot();
    auto slot_left_bottom = pack_left->NewSlot();
    pack_left->SetShowTitleBar(false);

    // Create slots on the right side
    slot = pack_master->NewSlot();
    auto pack_right = slot->MakePack();
    auto slot_right_top = pack_right->NewSlot();
    auto slot_right_bottom = pack_right->NewSlot();
    pack_right->SetShowTitleBar(false);

    // Setup content of the 4 window slots
    this->spawn_viewer(*slot_left_top, "XY View", TGLViewer::kCameraOrthoXOY);
    this->spawn_viewer(*slot_right_top, "ZY View", TGLViewer::kCameraOrthoZOY);
    this->spawn_viewer(
        *slot_left_bottom, "XZ View", TGLViewer::kCameraOrthoXOZ);
    this->spawn_viewer(
        *slot_right_bottom, "3D View", TGLViewer::kCameraPerspXOZ);
}

//---------------------------------------------------------------------------//
/*!
 * Setup projection tab viewer.
 */
void MainViewer::spawn_viewer(TEveWindowSlot& slot,
                              std::string title,
                              TGLViewer::ECameraType camera)
{
    slot.MakeCurrent();
    auto eve_view = gEve->SpawnNewViewer(title.c_str(), "");
    eve_view->GetGLViewer()->SetCurrentCamera(camera);
    eve_view->GetGLViewer()->SetStyle(TGLRnrCtx::kWireFrame);
    eve_view->AddScene(gEve->GetGlobalScene());
    eve_view->AddScene(gEve->GetEventScene());
}
