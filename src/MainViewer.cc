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
 * Return the top volume of the geometry file.
 */
TGeoVolume* MainViewer::GetTopVolume()
{
    return gGeoManager->GetTopVolume();
}

//---------------------------------------------------------------------------//
/*!
 * Add World volume to the viewer.
 */
void MainViewer::AddWorldVolume()
{
    assert(gGeoManager->GetTopVolume());

    auto eve_node = new TEveGeoTopNode(gGeoManager, gGeoManager->GetTopNode());
    eve_node->SetVisOption(0);
    eve_node->SetVisLevel(vis_level_);
    gEve->AddGlobalElement(eve_node);
}

//---------------------------------------------------------------------------//
/*!
 * Add specific volume to the viewer.
 */
void MainViewer::AddVolume(std::string node_name)
{
    auto* top_volume = this->GetTopVolume();
    auto* object_list = top_volume->GetNodes();

    for (auto object : *object_list)
    {
        auto const object_name = object->GetName();
        auto* object_node = top_volume->FindNode(object_name);

        auto eve_node = new TEveGeoTopNode(gGeoManager, object_node);
        eve_node->SetVisOption(0);
        eve_node->SetVisLevel(vis_level_);
        gEve->AddGlobalElement(eve_node);
        std::cout << object_node->GetVolume()->GetName() << std::endl;
    }
}

//---------------------------------------------------------------------------//
/*!
 * Function tailored to better display the CMS detector (cms2018.gdml).
 * It skips CMS surrounding building and set some LHC parts as invisible.
 */
void MainViewer::AddCMSVolume()
{
    std::cout << "Using the -cms flag" << std::endl;

    auto geo_volume = this->GetTopVolume();
    auto cmse_node = geo_volume->FindNode("CMSE0x7f4a8f616d40");
    if (!cmse_node)
    {
        // Not CMS, stop
        std::cout << "[Warning] Not the CMS 2018 geometry, nothing to do\n";
        return;
    }

    auto cmse_vol = cmse_node->GetVolume();
    auto cmse_top_node = new TEveGeoTopNode(gGeoManager, cmse_node);
    cmse_top_node->SetVisLevel(vis_level_);
    gEve->AddGlobalElement(cmse_top_node);

    // Define list of elements that should be set to invisible
    std::vector<std::string> invisible_node_list;
    invisible_node_list.push_back("CMStoZDC0x7f4a9a757000");
    invisible_node_list.push_back("ZDCtoFP4200x7f4a9a757180");
    invisible_node_list.push_back("BEAM30x7f4a8f615040");
    invisible_node_list.push_back("BEAM20x7f4a9a75ae00");
    invisible_node_list.push_back("VCAL0x7f4a8f615540");
    invisible_node_list.push_back("CastorF0x7f4a8f615f80");
    invisible_node_list.push_back("CastorB0x7f4a8f616080");
    invisible_node_list.push_back("TotemT20x7f4a8f615ac0");
    invisible_node_list.push_back("OQUA0x7f4a8f616600");
    invisible_node_list.push_back("BSC20x7f4a8f616740");
    invisible_node_list.push_back("ZDC0x7f4a8f6168c0");

    // Set selected elements as invisible
    for (auto const& node : invisible_node_list)
    {
        auto cmse_subvol = cmse_vol->FindNode(node.c_str())->GetVolume();
        cmse_subvol->InvisibleAll();
        cmse_subvol->SetVisDaughters(0);
    }

    // Print info
    std::cout << "CMS surrounding building is not loaded" << std::endl;
    std::cout << "LHC elements are set to invisible" << std::endl;
    std::cout << "Volumes:" << std::endl;
    std::cout << geo_volume->GetName() << std::endl;
    std::cout << " | " << cmse_vol->GetName() << std::endl;
}

//---------------------------------------------------------------------------//
/*!
 * Set the level of details.
 * It is the number of levels deep in which daughter volumes are drawn.
 */
void MainViewer::SetVisLevel(int vis_level)
{
    vis_level_ = vis_level;
}

//---------------------------------------------------------------------------//
/*!
 * Return gEve singleton.
 */
TEveManager* MainViewer::GetEveManager()
{
    return gEve;
}

//---------------------------------------------------------------------------//
/*!
 * Start MainViewer GUI.
 */
void MainViewer::StartViewer()
{
    gEve->GetBrowser()->TRootBrowser::SetWindowName("Celeritas Event Display");
    gEve->GetDefaultViewer()->SetElementName("Main viewer");
    gEve->GetBrowser()->HideBottomTab();
    gEve->GetDefaultGLViewer()->GetClipSet()->SetClipType(TGLClip::EType(0));

    // Build 2nd tab with orthogonal viewers
    this->InitProjectionsTab();
    gEve->FullRedraw3D(kTRUE);

    // Return focus to the main viewer
    gEve->GetDefaultGLViewer();

    std::cout << std::endl;
    root_app_->Run();
    root_app_->Terminate(0);
}

//---------------------------------------------------------------------------//
// PRIVATE
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
/*!
 * Create MainViewer ortho viewers (2nd tab in the GUI).
 */
void MainViewer::InitProjectionsTab()
{
    //// Create 4 window slots

    // Create top window to contain all 4 slots
    TEveWindowSlot* slot
        = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
    TEveWindowPack* pack_master = slot->MakePack();
    pack_master->SetElementName("Projections");
    pack_master->SetHorizontal();
    pack_master->SetShowTitleBar(kFALSE);

    // Create slots on the left side
    slot = pack_master->NewSlot();
    auto pack_left = slot->MakePack();
    auto slot_left_top = pack_left->NewSlot();
    auto slot_left_bottom = pack_left->NewSlot();
    pack_left->SetShowTitleBar(kFALSE);

    // Create slots on the right side
    slot = pack_master->NewSlot();
    auto pack_right = slot->MakePack();
    auto slot_right_top = pack_right->NewSlot();
    auto slot_right_bottom = pack_right->NewSlot();
    pack_right->SetShowTitleBar(kFALSE);

    //// Setup content of the 4 window slots
    this->SpawnViewer(slot_left_top, "XY View", TGLViewer::kCameraOrthoXOY);
    this->SpawnViewer(slot_right_top, "ZY View", TGLViewer::kCameraOrthoZOY);
    this->SpawnViewer(slot_left_bottom, "XZ View", TGLViewer::kCameraOrthoXOZ);
    this->SpawnViewer(slot_right_bottom, "3D View", TGLViewer::kCameraPerspXOZ);
}

//---------------------------------------------------------------------------//
/*!
 * Setup viewer in the projections tab.
 */
void MainViewer::SpawnViewer(TEveWindowSlot* slot,
                             std::string title,
                             TGLViewer::ECameraType camera)
{
    slot->MakeCurrent();
    auto eve_view = gEve->SpawnNewViewer(title.c_str(), "");
    eve_view->GetGLViewer()->SetCurrentCamera(camera);
    eve_view->GetGLViewer()->SetStyle(TGLRnrCtx::kWireFrame);
    eve_view->AddScene(gEve->GetGlobalScene());
    eve_view->AddScene(gEve->GetEventScene());
}
