//----------------------------------*-C++-*----------------------------------//
// Copyright 2020-2022 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file Evd.cpp
//---------------------------------------------------------------------------//
#include "Evd.hh"

#include <iostream>
#include <assert.h>

#include <TMath.h>
#include <TTree.h>
#include <TBranch.h>
#include <TGeoManager.h>
#include <TEveWindow.h>
#include <TEveViewer.h>
#include <TEveBrowser.h>
#include <TEveGeoNode.h>
#include <TGLViewer.h>
#include <TObjArray.h>
#include <TObject.h>

//---------------------------------------------------------------------------//
/*!
 * Construct with geometry and simulation inputs.
 *
 * The simulation input file is from benchmarks/geant4-validation-app is not
 * mandatory and can be passed as a nullptr.
 */
Evd::Evd(const char* gdml_input, const char* simulation_input)
    : vis_level_(1), has_elements_(false)
{
    root_app_.reset(new TRint("evd", nullptr, nullptr, nullptr, 0, true));
    root_app_->SetPrompt("evd [%d] ");

    // TEveManager creates a gEve pointer owned by ROOT
    TEveManager::Create();
    LoadGeometry(gdml_input);

    if (simulation_input)
    {
        root_file_.reset(TFile::Open(simulation_input, "read"));
        std::cout << "Simulation input: " << simulation_input << std::endl;
    }
}

//---------------------------------------------------------------------------//
/*!
 * Load gdml file into TGeoManager.
 */
void Evd::LoadGeometry(const char* gdml_input)
{
    // TGeoManager creates a gGeoManager pointer owned by ROOT
    TGeoManager::SetVerboseLevel(0);
    TGeoManager::Import(gdml_input);
    std::cout << "Geometry input: " << gdml_input << std::endl;
}

//---------------------------------------------------------------------------//
/*!
 * Fetch node names within a given TGeoVolume.
 */
std::vector<std::string> Evd::GetNodeList(TGeoVolume* geo_volume)
{
    std::vector<std::string> list;
    TObjArray*               node_list = geo_volume->GetNodes();

    for (auto node : *node_list)
        list.push_back(node->GetName());

    return list;
}

//---------------------------------------------------------------------------//
/*!
 * Return the top volume of the geometry file.
 *
 * \code
 *  TGeoVolume* top_volume = evd.GetTopVolume();
 *  TGeoNode*   node = top_volume->Find("node_name");
 * \endcode
 */
TGeoVolume* Evd::GetTopVolume()
{
    return gGeoManager->GetTopVolume();
}

//---------------------------------------------------------------------------//
/*!
 * Add World volume to the viewer.
 */
void Evd::AddWorldVolume()
{
    assert(gGeoManager->GetTopVolume());

    std::cout << "Volumes:" << std::endl;
    std::cout << gGeoManager->GetTopVolume()->GetName() << std::endl;

    // Add node
    auto eve_node = new TEveGeoTopNode(gGeoManager, gGeoManager->GetTopNode());
    eve_node->SetVisOption(0);
    eve_node->SetVisLevel(vis_level_);
    gEve->AddGlobalElement(eve_node);
    has_elements_ = true;
}

//---------------------------------------------------------------------------//
/*!
 * Add volume to the viewer.
 */
void Evd::AddVolume(TGeoVolume* geo_volume)
{
    assert(geo_volume);
    assert(geo_volume->GetNtotal() == 1);

    // Add nodes
    TObjArray* object_list = geo_volume->GetNodes();

    for (auto object : *object_list)
    {
        const char* object_name = object->GetName();
        TGeoNode*   object_node = geo_volume->FindNode(object_name);

        auto eve_node = new TEveGeoTopNode(gGeoManager, object_node);
        eve_node->SetVisOption(0);
        eve_node->SetVisLevel(vis_level_);
        gEve->AddGlobalElement(eve_node);

        if (has_elements_)
        {
            std::cout << " | ";
        }
        std::cout << object_node->GetVolume()->GetName() << std::endl;
    }
    has_elements_ = true;
}

//---------------------------------------------------------------------------//
/*!
 * Function tailored to better display the CMS detector.
 * It skips CMS surrounding building and set some LHC parts as invisible.
 */
void Evd::AddCMSVolume(TGeoVolume* geo_volume)
{
    std::cout << "Using the -cms flag" << std::endl;

    auto cmse_node = geo_volume->FindNode("CMSE0x7f4a8f616d40");

    if (!cmse_node)
    {
        // Not CMS, stop
        std::cout << "[ERROR] No cms geometry found\n";
        return;
    }

    auto cmse_vol      = cmse_node->GetVolume();
    auto cmse_top_node = new TEveGeoTopNode(gGeoManager, cmse_node);
    cmse_top_node->SetVisLevel(vis_level_);
    gEve->AddGlobalElement(cmse_top_node);
    has_elements_ = true;

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
    for (const auto& node : invisible_node_list)
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
 * Add event from benchmarks/geant4-validation-app.
 *
 * If event index is negative, all events are drawn.
 */
void Evd::AddEvent(std::size_t event_idx)
{
    assert(root_file_);
    TTree* event_tree = (TTree*)root_file_->Get("events");
    assert(event_tree->GetEntries() > event_idx);

    rootdata::Event* event = nullptr;
    event_tree->SetBranchAddress("event", &event);

    // If negative, loop over all events; Otherwise, just draw selected event
    size_t first = (event_idx < 0) ? 0 : event_idx;
    size_t last  = (event_idx < 0) ? event_tree->GetEntries() : event_idx + 1;

    for (auto i = first; i < last; i++)
    {
        event_tree->GetEntry(i);
        this->CreateEventTracks(event->primaries, event->id);
        this->CreateEventTracks(event->secondaries, event->id);
    }
}

//---------------------------------------------------------------------------//
/*!
 * Set the level of details.
 * It is the number of levels deep in which daughter volumes are drawn.
 */
void Evd::SetVisLevel(int vis_level)
{
    vis_level_ = vis_level;
}

//---------------------------------------------------------------------------//
/*!
 * Return gEve reference.
 */
TEveManager& Evd::GetEveManager()
{
    return *gEve;
}

//---------------------------------------------------------------------------//
/*!
 * Start Evd GUI.
 */
void Evd::StartViewer()
{
    if (!has_elements_)
    {
        // No elements added to the viewer, stop
        std::cout << "No elements added. Abort viewer" << std::endl;
        root_app_->Terminate(10);
        return;
    }
    gEve->GetBrowser()->TRootBrowser::SetWindowName("Celeritas Event Display");
    gEve->GetDefaultViewer()->SetElementName("Main viewer");
    gEve->GetBrowser()->HideBottomTab();
    gEve->GetDefaultGLViewer()->GetClipSet()->SetClipType(TGLClip::EType(0));

    // Build 2nd tab with orthogonal viewers
    this->StartOrthoViewer();
    gEve->FullRedraw3D(kTRUE);

    // Return focus to the main viewer
    gEve->GetDefaultGLViewer();

    std::cout << std::endl;
    root_app_->Run();
    root_app_->Terminate(0);
}

//---------------------------------------------------------------------------//
/*!
 * Create Evd ortho viewers (2nd tab in the GUI).
 */
void Evd::StartOrthoViewer()
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
    slot                  = pack_master->NewSlot();
    auto pack_left        = slot->MakePack();
    auto slot_left_top    = pack_left->NewSlot();
    auto slot_left_bottom = pack_left->NewSlot();
    pack_left->SetShowTitleBar(kFALSE);

    // Create slots on the right side
    slot                   = pack_master->NewSlot();
    auto pack_right        = slot->MakePack();
    auto slot_right_top    = pack_right->NewSlot();
    auto slot_right_bottom = pack_right->NewSlot();
    pack_right->SetShowTitleBar(kFALSE);

    //// Draw the contents of the 4 window slots

    // Top left slot
    slot_left_top->MakeCurrent();
    auto eve_xy_view = gEve->SpawnNewViewer("XY View", "");
    eve_xy_view->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
    eve_xy_view->GetGLViewer()->SetStyle(TGLRnrCtx::kWireFrame);
    eve_xy_view->AddScene(gEve->GetGlobalScene());
    eve_xy_view->AddScene(gEve->GetEventScene());

    // Top right slot
    slot_right_top->MakeCurrent();
    auto eve_zy_view = gEve->SpawnNewViewer("ZY View", "");
    eve_zy_view->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoZOY);
    eve_zy_view->GetGLViewer()->SetStyle(TGLRnrCtx::kWireFrame);
    eve_zy_view->AddScene(gEve->GetGlobalScene());
    eve_zy_view->AddScene(gEve->GetEventScene());

    // Bottom left slot
    slot_left_bottom->MakeCurrent();
    auto eve_xz_view = gEve->SpawnNewViewer("XZ View", "");
    eve_xz_view->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOZ);
    eve_xz_view->GetGLViewer()->SetStyle(TGLRnrCtx::kWireFrame);
    eve_xz_view->AddScene(gEve->GetGlobalScene());
    eve_xz_view->AddScene(gEve->GetEventScene());

    // Bottom right slot
    slot_right_bottom->MakeCurrent();
    auto eve_3d_view = gEve->SpawnNewViewer("3D View", "");
    eve_3d_view->GetGLViewer()->SetStyle(TGLRnrCtx::kWireFrame);
    // eve_3d_view->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraPerspXOY);
    eve_3d_view->AddScene(gEve->GetGlobalScene());
    eve_3d_view->AddScene(gEve->GetEventScene());
}

//---------------------------------------------------------------------------//
/*!
 * Loop over a vector of tracks (either primaries or secondaries), generate a
 * TEveLine for each, and add them to the viewer.
 */
void Evd::CreateEventTracks(const std::vector<rootdata::Track>& vec_tracks,
                            const std::size_t                   event_id)
{
    for (const auto& track : vec_tracks)
    {
        auto track_line = this->CreateTrackLine(track, event_id);
        gEve->AddElement(track_line.release());
    }
}

//---------------------------------------------------------------------------//
/*!
 * Return a unique_ptr with a single TEveLine generated from the steps provided
 * by `rootdata::Track`.
 */
std::unique_ptr<TEveLine>
Evd::CreateTrackLine(const rootdata::Track& track, const std::size_t event_id)
{
    std::string track_name = std::to_string(event_id) + "_"
                             + std::to_string(track.id) + "_"
                             + this->to_string((PDG)track.pdg);

    auto track_line
        = std::make_unique<TEveLine>((TEveLine::ETreeVarType_e::kTVT_XYZ));
    track_line->SetName(track_name.c_str());
    this->set_track_attributes(track_line.get(), (PDG)track.pdg);

    // Store vertex
    const auto& vtx = track.vertex_position;
    track_line->SetNextPoint(vtx.x, vtx.y, vtx.z);

    for (const auto& step : track.steps)
    {
        // Store steps
        const auto& pos = step.position;
        track_line->SetNextPoint(pos.x, pos.y, pos.z);
    }

    return track_line;
}

//---------------------------------------------------------------------------//
/*!
 * Convert PDG to string.
 */
std::string Evd::to_string(PDG id)
{
    switch (id)
    {
        case PDG::gamma:
            return "gamma";
        case PDG::e_minus:
            return "e-";
        case PDG::e_plus:
            return "e+";
        case PDG::mu_minus:
            return "mu-";
        default:
            return "undefined";
    }
}

//---------------------------------------------------------------------------//
/*!
 * Set up attributes of a TEveLine for drawing tracks.
 */
void Evd::set_track_attributes(TEveLine* track, PDG pdg)
{
    switch (pdg)
    {
        case PDG::gamma:
            track->SetLineColor(kGreen);
            break;
        case PDG::e_minus:
            track->SetLineColor(kAzure + 1);
            break;
        case PDG::e_plus:
            track->SetLineColor(kRed);
            break;
        case PDG::mu_minus:
            track->SetLineColor(kOrange);
            break;
        default:
            track->SetLineColor(kGray);
    }
}
