//----------------------------------*-C++-*----------------------------------//
// Copyright 2021 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file Evd.cpp
//---------------------------------------------------------------------------//
#include "Evd.hh"
#include "RootData.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

#include <TRint.h>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TGeoManager.h>
#include <TGeoVolume.h>
#include <TEveManager.h>
#include <TEveWindow.h>
#include <TEveViewer.h>
#include <TEveBrowser.h>
#include <TEveGeoNode.h>
#include <TEveTrack.h>
#include <TGLViewer.h>

//---------------------------------------------------------------------------//
/*!
 * Construct with data inputs.
 */
Evd::Evd(const char* gdml_input, const char* simulation_input)
    : vis_level_(1), has_elements_(false)
{
    root_app_
        = std::make_unique<TRint>("Evd", nullptr, nullptr, nullptr, 0, kTRUE);
    root_app_->SetPrompt("Evd [%d] ");

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
 * Default destructor.
 */
Evd::~Evd() = default;

//---------------------------------------------------------------------------//
/*!
 * Import gdml file into TGeoManager.
 */
void Evd::LoadGeometry(const char* gdml_input)
{
    // TGeoManager creates a gGeoManager pointer owned by ROOT
    TGeoManager::SetVerboseLevel(0);
    TGeoManager::Import(gdml_input);

    // Print info
    std::cout << "Geometry input: " << gdml_input << std::endl;
}

//---------------------------------------------------------------------------//
/*!
 * Fetch node names within a given TGeoVolume.
 */
std::vector<std::string> Evd::GetNodeList(TGeoVolume* geoVolume)
{
    std::vector<std::string> list;
    TObjArray*               nodeList = geoVolume->GetNodes();

    for (auto node : *nodeList)
        list.push_back(node->GetName());

    return list;
}

//---------------------------------------------------------------------------//
/*!
 * Return the top volume of the geometry file
 *
 * Method needed because gGeoManager is a private class member. Conversely,
 * any volume node can be accessed from a TGeoVolume* top_volume:
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
 * Add World volume to the viewer
 */
void Evd::AddWorldVolume()
{
    if (!gGeoManager->GetTopVolume())
        return;

    // Print info
    if (!has_elements_)
    {
        std::cout << "Volumes " << std::endl;
    }

    std::cout << gGeoManager->GetTopVolume()->GetName() << std::endl;

    // Add node
    TEveGeoTopNode* eveNode
        = new TEveGeoTopNode(gGeoManager, gGeoManager->GetTopNode());
    eveNode->SetVisLevel(vis_level_);
    gEve->AddGlobalElement(eveNode);
    has_elements_ = true;
}

//---------------------------------------------------------------------------//
/*!
 * Add volume to the viewer
 */
void Evd::AddVolume(TGeoVolume* geoVolume)
{
    // If no TGeoVolume*, stop
    if (!geoVolume)
        return;

    // If no nodes inside world, stop
    if (geoVolume->GetNtotal() == 1)
        return;

    // Add nodes
    TObjArray* objectList = geoVolume->GetNodes();

    for (auto object : *objectList)
    {
        const char* objectName = object->GetName();
        TGeoNode*   objectNode = geoVolume->FindNode(objectName);

        TEveGeoTopNode* eveNode = new TEveGeoTopNode(gGeoManager, objectNode);
        eveNode->SetVisLevel(vis_level_);
        gEve->AddGlobalElement(eveNode);

        if (has_elements_)
        {
            std::cout << "- ";
        }
        std::cout << objectNode->GetVolume()->GetName() << std::endl;
    }
    has_elements_ = true;
}

//---------------------------------------------------------------------------//
/*!
 * Function tailored to better display the CMS detector.
 * It skips CMS surrounding building and set some LHC parts as invisible
 */
void Evd::AddCMSVolume(TGeoVolume* geoVolume)
{
    std::cout << "Using the -cms flag" << std::endl;

    TGeoNode* cmseNode = nullptr;

    // If no CMS node found, stop
    if (!(cmseNode = geoVolume->FindNode("CMSE0x7f4a8f616d40")))
    {
        std::cout << "[ERROR] No cms geometry was found\n";
        return;
    }

    TGeoVolume*     cmseVol     = cmseNode->GetVolume();
    TEveGeoTopNode* cmseTopNode = new TEveGeoTopNode(gGeoManager, cmseNode);
    cmseTopNode->SetVisLevel(vis_level_);
    gEve->AddGlobalElement(cmseTopNode);
    has_elements_ = true;

    // Define list of elements that should be set to invisible
    std::vector<std::string> invisibleNodeList;
    invisibleNodeList.push_back("CMStoZDC0x7f4a9a757000");
    invisibleNodeList.push_back("ZDCtoFP4200x7f4a9a757180");
    invisibleNodeList.push_back("BEAM30x7f4a8f615040");
    invisibleNodeList.push_back("BEAM20x7f4a9a75ae00");
    invisibleNodeList.push_back("VCAL0x7f4a8f615540");
    invisibleNodeList.push_back("CastorF0x7f4a8f615f80");
    invisibleNodeList.push_back("CastorB0x7f4a8f616080");
    invisibleNodeList.push_back("TotemT20x7f4a8f615ac0");
    invisibleNodeList.push_back("OQUA0x7f4a8f616600");
    invisibleNodeList.push_back("BSC20x7f4a8f616740");
    invisibleNodeList.push_back("ZDC0x7f4a8f6168c0");

    // Set selected elements as invisible
    for (std::string node : invisibleNodeList)
    {
        TGeoVolume* cmseSubvol = cmseVol->FindNode(node.c_str())->GetVolume();
        cmseSubvol->InvisibleAll();
        cmseSubvol->SetVisDaughters(0);
    }

    // Print info
    std::cout << "CMS surrounding building is not loaded" << std::endl;
    std::cout << "LHC beam elements are set to invisible" << std::endl;
    std::cout << std::endl;
    std::cout << "Volumes" << std::endl;
    std::cout << geoVolume->GetName() << std::endl;
    std::cout << " | " << cmseVol->GetName() << std::endl;
}

//---------------------------------------------------------------------------//
/*!
 * Add event from benchmarks/geant4-validation-app.
 */
void Evd::AddEvents()
{
    TTree*           event_tree = (TTree*)root_file_->Get("event");
    rootdata::Event* event      = nullptr;
    event_tree->SetBranchAddress("event", &event);

    for (int i = 0; i < event_tree->GetEntries(); i++)
    {
        event_tree->GetEntry(i);
        for (const auto& primary : event->primaries)
        {
            TEveLine* track_line
                = new TEveLine(TEveLine::ETreeVarType_e::kTVT_XYZ);
            track_line->SetMarkerColor(kYellow);

            for (const auto& step : primary.steps)
            {
                const auto& pos = step.position;
                track_line->SetNextPoint(pos.x * 10, pos.y * 10, pos.z * 10);
            }

            std::string track_name = std::to_string(i) + "_";

            switch (primary.pdg)
            {
                case PDG::pdg_gamma:
                    track_name += "gamma";
                    track_line->SetName(track_name.c_str());
                    track_line->SetLineColor(kGreen);
                    break;
                case PDG::pdg_e_minus:
                    track_name += "e-";
                    track_line->SetName(track_name.c_str());
                    track_line->SetLineColor(kBlue);
                    break;
                case PDG::pdg_e_plus:
                    track_name += "e+";
                    track_line->SetName(track_name.c_str());
                    track_line->SetLineColor(kRed);
                    break;
            }

            gEve->AddElement(track_line);
        }

        for (const auto& secondary : event->secondaries)
        {
            TEveLine* track_line
                = new TEveLine(TEveLine::ETreeVarType_e::kTVT_XYZ);
            track_line->SetMarkerColor(kYellow);

            for (const auto& step : secondary.steps)
            {
                const auto pos = step.position;
                track_line->SetNextPoint(pos.x * 10, pos.y * 10, pos.z * 10);
            }

            std::string track_name = std::to_string(i) + "_";

            switch (secondary.pdg)
            {
                case PDG::pdg_gamma:
                    track_name += "gamma";
                    track_line->SetName(track_name.c_str());
                    track_line->SetLineColor(kGreen);
                    break;
                case PDG::pdg_e_minus:
                    track_name += "e-";
                    track_line->SetName(track_name.c_str());
                    track_line->SetLineColor(kBlue);
                    break;
                case PDG::pdg_e_plus:
                    track_name += "e+";
                    track_line->SetName(track_name.c_str());
                    track_line->SetLineColor(kRed);
                    break;
            }

            gEve->AddElement(track_line);
        }
    }
}

//---------------------------------------------------------------------------//
/*!
 * Set the level of details
 * This defines the number of levels deep in which daughter volumes are
 * drawn
 */
void Evd::SetVisLevel(const int vis_level)
{
    vis_level_ = vis_level;
}

//---------------------------------------------------------------------------//
/*!
 * Return gEve reference
 */
TEveManager& Evd::GetEveManager()
{
    return *gEve;
}

//---------------------------------------------------------------------------//
/*!
 * Start Evd GUI
 */
void Evd::StartViewer()
{
    // If no elements added to the viewer, stop
    if (!has_elements_)
    {
        std::cout << "No elements added. Abort viewer" << std::endl;
        root_app_->Terminate(10);
        return;
    }
    std::cout << std::endl;

    // Set window name
    gEve->GetBrowser()->TRootBrowser::SetWindowName("Celeritas Event Display");

    // Hide command line box
    gEve->GetBrowser()->HideBottomTab();

    // Set viewer name
    gEve->GetDefaultViewer()->SetElementName("Main viewer");

    // Set box clipping in the Main viewer
    TGLViewer* evdViewer = gEve->GetDefaultGLViewer();
    evdViewer->GetClipSet()->SetClipType(TGLClip::EType(0));

    // Build 2nd tab with orthogonal viewers
    StartOrthoViewer();

    gEve->FullRedraw3D(kTRUE);

    // Run Evd GUI
    root_app_->Run();
    root_app_->Terminate(0);
}

//---------------------------------------------------------------------------//
/*!
 * Create Evd ortho viewers (2nd tab in the GUI)
 */
void Evd::StartOrthoViewer()
{
    // Create 4 window slots

    // Create top window to contain all 4 slots
    TEveWindowSlot* slot
        = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
    TEveWindowPack* packMaster = slot->MakePack();
    packMaster->SetElementName("Projections");
    packMaster->SetHorizontal();
    packMaster->SetShowTitleBar(kFALSE);

    // Create slots on the left side
    slot                     = packMaster->NewSlot();
    TEveWindowPack* packLeft = slot->MakePack();
    packLeft->SetShowTitleBar(kFALSE);
    TEveWindowSlot* slotLeftTop    = packLeft->NewSlot();
    TEveWindowSlot* slotLeftBottom = packLeft->NewSlot();

    // Create slots on the right side
    slot                      = packMaster->NewSlot();
    TEveWindowPack* packRight = slot->MakePack();
    packRight->SetShowTitleBar(kFALSE);
    TEveWindowSlot* slotRightTop    = packRight->NewSlot();
    TEveWindowSlot* slotRightBottom = packRight->NewSlot();

    // Draw the contents of the 4 window slots

    // Top left slot
    slotLeftTop->MakeCurrent();
    TEveViewer* eveXYView;
    eveXYView = gEve->SpawnNewViewer("XY View", "");
    eveXYView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
    eveXYView->GetGLViewer()->SetStyle(TGLRnrCtx::kWireFrame);
    eveXYView->AddScene(gEve->GetGlobalScene());
    eveXYView->AddScene(gEve->GetEventScene());

    // Top right slot
    slotRightTop->MakeCurrent();
    TEveViewer* eveZYView;
    eveZYView = gEve->SpawnNewViewer("ZY View", "");
    eveZYView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoZOY);
    eveZYView->GetGLViewer()->SetStyle(TGLRnrCtx::kWireFrame);
    eveZYView->AddScene(gEve->GetGlobalScene());
    eveZYView->AddScene(gEve->GetEventScene());

    // Bottom left slot
    slotLeftBottom->MakeCurrent();
    TEveViewer* eveXZView;
    eveXZView = gEve->SpawnNewViewer("XZ View", "");
    eveXZView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOZ);
    eveXZView->GetGLViewer()->SetStyle(TGLRnrCtx::kWireFrame);
    eveXZView->AddScene(gEve->GetGlobalScene());
    eveXZView->AddScene(gEve->GetEventScene());

    // Bottom right slot
    slotRightBottom->MakeCurrent();
    TEveViewer* eve3DView;
    eve3DView = gEve->SpawnNewViewer("3D View", "");
    eve3DView->GetGLViewer()->SetStyle(TGLRnrCtx::kWireFrame);
    eve3DView->AddScene(gEve->GetGlobalScene());
    eve3DView->AddScene(gEve->GetEventScene());
}

//---------------------------------------------------------------------------//
/*!
 * TBD.
 */
void Evd::FindVolume(TGeoVolume& volume, std::string volume_name)
{
    auto object_list = volume.GetNodes();
    for (auto object : *object_list)
    {
        auto this_node = volume.FindNode(object->GetName());
        std::cout << this_node->GetVolume()->GetName() << std::endl;
        TEveGeoTopNode* eveNode = new TEveGeoTopNode(gGeoManager, this_node);
        eveNode->SetVisLevel(vis_level_);
        gEve->AddGlobalElement(eveNode);
        has_elements_ = true;
        return;
    }
}
