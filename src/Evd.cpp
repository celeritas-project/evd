//---------------------------------*-C++-*-----------------------------------//
//! \file   Evd/src/Evd.cpp
//! \author Stefano Tognini
//! \note   Copyright (c) 2020 Oak Ridge National Laboratory, UT-Battelle, LLC
//---------------------------------------------------------------------------//
//! \brief  Event Display for the Celeritas Project
//---------------------------------------------------------------------------//

// C++
#include <iostream>
#include <memory>

// ROOT
#include <TRint.h>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TGeoManager.h>
#include <TEveManager.h>
#include <TEveWindow.h>
#include <TEveViewer.h>
#include <TEveBrowser.h>
#include <TEveGeoNode.h>
#include <TEveTrack.h>
#include <TGLViewer.h>

// Project
#include "Evd.hh"

//---------------------------------------------------------------------------//
//! Constructor
Evd::Evd(const char* gdml_input, const char* simulation_input)
    : vis_level_(1), has_elements_(false)
{
    PrintEvdLogo();

    root_app_
        = std::make_unique<TRint>("Evd", nullptr, nullptr, nullptr, 0, kTRUE);
    root_app_->SetPrompt("Evd [%d] ");

    //! TEveManager creates a gEve pointer owned by ROOT
    TEveManager::Create();

    LoadGeometry(gdml_input);

    if (simulation_input)
        LoadSimulation(simulation_input);
}

//---------------------------------------------------------------------------//
//! Destructor
Evd::~Evd() = default;

//---------------------------------------------------------------------------//
//! Import gdml file into TGeoManager
void Evd::LoadGeometry(const char* gdml_input)
{
    if (!gdml_input)
    {
        std::cout << "[ERROR] No gdml file" << std::endl;
        return;
    }

    std::cout << std::endl;
    std::cout << "----------------------- Geometry -----------------------\n";
    std::cout << gdml_input << std::endl;
    std::cout << std::endl;

    //! TGeoManager creates a gGeoManager pointer owned by ROOT
    TGeoManager::SetVerboseLevel(0);
    TGeoManager::Import(gdml_input);
}

//---------------------------------------------------------------------------//
//! Load simulation ROOT file
void Evd::LoadSimulation(const char* rootFile)
{
    std::cout << std::endl;
    std::cout << "---------------------- Simulation ----------------------\n";
    std::cout << rootFile << std::endl;
    std::cout << std::endl;

    root_file_.reset(TFile::Open(rootFile));
}

//---------------------------------------------------------------------------//
//! Fetch node names within a given TGeoVolume
std::vector<std::string> Evd::GetNodeList(TGeoVolume* geoVolume)
{
    std::vector<std::string> list;
    TObjArray*               nodeList = geoVolume->GetNodes();

    for (auto node : *nodeList)
        list.push_back(node->GetName());

    return list;
}

//---------------------------------------------------------------------------//
//! Return the top volume of the geometry file
TGeoVolume* Evd::GetTopVolume()
{
    return gGeoManager->GetTopVolume();
}

//---------------------------------------------------------------------------//
//! Get a specific node from a given volume
TGeoVolume* Evd::GetVolumeNode(TGeoVolume* geoVolume, const char* node)
{
    return geoVolume->FindNode(node)->GetVolume();
}

//---------------------------------------------------------------------------//
//! Add World volume to the viewer
void Evd::AddWorldVolume()
{
    // Print info
    if (!has_elements_)
    {
        std::cout << "--------------------- Volumes added "
                     "--------------------"
                  << std::endl;
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
//! Add volume to the viewer
void Evd::AddVolume(TGeoVolume* geoVolume)
{
    // If no TGeoVolume*, stop
    if (!geoVolume)
        return;

    // If no nodes inside world, stop
    if (geoVolume->GetNtotal() == 1)
        return;

    // Print info
    if (!has_elements_)
    {
        std::cout << "--------------------- Volumes added "
                     "--------------------"
                  << std::endl;
    }

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
            std::cout << " | ";
        }
        std::cout << objectNode->GetVolume()->GetName() << std::endl;
    }
    has_elements_ = true;
}

//---------------------------------------------------------------------------//
//! Function tailored to better display the CMS detector
//! Skip CMS surrounding building and set LHC parts as invisible
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
    std::cout << "--------------------- Volumes added --------------------\n";
    std::cout << geoVolume->GetName() << std::endl;
    std::cout << " | " << cmseVol->GetName() << std::endl;
}

//---------------------------------------------------------------------------//
//! Draw events from the Geant4-Sandbox
//! Uses step data to draw lines connecting the steps
void Evd::AddEvent(const int event, const int trackLimit)
{
    std::cout << std::endl;
    std::cout << "---------------------- Event added ---------------------\n";
    std::cout << "Event " << event << std::endl;
    std::cout << "Printing ";
    (trackLimit == 0 ? std::cout << "all" : std::cout << trackLimit);
    std::cout << " tracks" << std::endl;
    std::cout << std::endl;
    std::cerr << "Loading";

    // Load ROOT TTrees
    TTree* eventTree = (TTree*)root_file_->Get("event");
    TTree* stepTree  = (TTree*)root_file_->Get("step");

    // Fetch the total number of entries (i.e. steps)
    long long numberOfEntries = stepTree->GetEntries();

    bool printStatus = false;
    if (numberOfEntries > 1000)
        printStatus = true;

    // Fetch track ID list for a given event
    std::vector<int>* trkIDlist = 0;
    eventTree->SetBranchAddress("trkIDlist", &trkIDlist);

    // Make the entries loop a bit quicker
    long long startingEntry = 0;
    if (event > 0)
    {
        for (int n = 0; n < event; n++)
        {
            eventTree->GetEntry(n);
            startingEntry += trkIDlist->size();
        }
    }

    // Reload the current event
    eventTree->GetEntry(event);

    // Create a std::vector of track lines to be added to the Evd
    std::vector<TEveLine*> trkLines;
    if (trackLimit > 0)
        trkLines.reserve(trackLimit * sizeof(TEveLine*));
    else
        trkLines.reserve(trkIDlist->size() * sizeof(TEveLine*));

    // Create the first line before entering the loop
    trkLines.push_back(new TEveLine(TEveLine::ETreeVarType_e::kTVT_XYZ));

    // Keep score of the number tracks
    unsigned int trk_i = 0;

    // Loop over entries (i.e. steps)
    for (long long i = startingEntry; i < numberOfEntries; i++)
    {
        // Print status (48 is the column width)
        if (printStatus && i % (numberOfEntries / 48) == 0)
            std::cerr << ".";

        // Fetch step entry
        stepTree->GetEntry(i);

        // Fetch event IDs
        int evtID = stepTree->GetLeaf("evtID")->GetValue();

        // Skipping unwanted events
        if (evtID > event)
            break;
        if (evtID != event)
            continue;

        // Stop if we reached the end of the event track list
        if (trk_i == trkIDlist->size())
            break;

        // Fetch track IDs
        int trkID = stepTree->GetLeaf("trkID")->GetValue();

        // Fetch step position
        double x = stepTree->GetLeaf("stepX")->GetValue() / cm;
        double y = stepTree->GetLeaf("stepY")->GetValue() / cm;
        double z = stepTree->GetLeaf("stepZ")->GetValue() / cm;

        // Skip steps that are too far from the CMS detector
        if (TMath::Abs(x) > 1500)
            continue;
        if (TMath::Abs(y) > 1500)
            continue;
        if (TMath::Abs(z) > 1500)
            continue;

        // If this trackID is one of the tracks of the chosen event
        if (trkID == trkIDlist->at(trk_i))
        {
            // Add step point for the track line
            trkLines.at(trk_i)->SetNextPoint(x, y, z);
        }

        // If a new track is found
        else if (trkID != trkIDlist->at(trk_i))
        {
            stepTree->GetEntry(i - 1);

            int PDG    = stepTree->GetLeaf("stepPDG")->GetValue();
            int absPDG = TMath::Abs(PDG);
            trkID      = stepTree->GetLeaf("trkID")->GetValue();

            // Electrons are green; muons are red; fotons are cyan;
            // others are gray
            if (absPDG == 11)
                trkLines.at(trk_i)->SetLineColor(kGreen);
            else if (absPDG == 13)
                trkLines.at(trk_i)->SetLineColor(kRed);
            else if (absPDG == 22)
                trkLines.at(trk_i)->SetLineColor(kBlue);
            else
                trkLines.at(trk_i)->SetLineColor(kGray);

            // Creating the line name
            // Line names are shown at the left side of Evd
            std::string lineName;
            lineName = std::to_string(PDG) + ", " + std::to_string(trkID);
            trkLines.at(trk_i)->SetName(lineName.c_str());
            trkLines.at(trk_i)->SetMarkerColor(kYellow);

            // Add line to the viewer
            gEve->AddElement(trkLines.at(trk_i));

            // Move to the next track ID in trkIDlist
            trk_i++;

            // If the maximum number of tracks to be drawn is reached, stop
            if (trackLimit != 0 && trk_i == trackLimit)
                break;

            // Start a new line
            TEveLine* line = new TEveLine(TEveLine::ETreeVarType_e::kTVT_XYZ);
            trkLines.push_back(line);

            // Add the first step point of the new the track line
            trkLines.at(trk_i)->SetNextPoint(x, y, z);
        }

        // If it is the last entry, we record it
        if (i == numberOfEntries - 1)
        {
            stepTree->GetEntry(i);

            int PDG    = stepTree->GetLeaf("stepPDG")->GetValue();
            int absPDG = TMath::Abs(PDG);
            trkID      = stepTree->GetLeaf("trkID")->GetValue();

            // Electrons are green; muons are red; fotons are cyan;
            // others are gray
            if (absPDG == 11)
                trkLines.at(trk_i)->SetLineColor(kGreen);
            else if (absPDG == 13)
                trkLines.at(trk_i)->SetLineColor(kRed);
            else if (absPDG == 22)
                trkLines.at(trk_i)->SetLineColor(kBlue);
            else
                trkLines.at(trk_i)->SetLineColor(kGray);

            // Create the line name
            // Line names are shown at the left side of Evd
            std::string lineName;
            lineName = std::to_string(PDG) + ", " + std::to_string(trkID);
            trkLines.at(trk_i)->SetName(lineName.c_str());
            trkLines.at(trk_i)->SetMarkerColor(kYellow);

            // Add line to the viewer
            gEve->AddElement(trkLines.at(trk_i));
        }
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

//---------------------------------------------------------------------------//
//! Set the level of details
//! The vis_level_ sets the number of levels deep in which daughter volumes 
//! are drawn.
void Evd::SetVisLevel(const int vis_level)
{
    vis_level_ = vis_level;
}

//---------------------------------------------------------------------------//
//! Start Evd GUI
void Evd::StartViewer()
{
    //! If no elements added to the viewer, stop
    if (!has_elements_)
    {
        std::cout << "No elements added. Abort viewer" << std::endl;
        root_app_->Terminate(10);
        return;
    }
    std::cout << std::endl;

    //! Set window name
    gEve->GetBrowser()->TRootBrowser::SetWindowName("Celeritas Event Display");

    //! Hide command line box
    gEve->GetBrowser()->HideBottomTab();

    //! Set viewer name
    gEve->GetDefaultViewer()->SetElementName("Main viewer");

    //! Set box clipping in the Main viewer
    TGLViewer* evdViewer = gEve->GetDefaultGLViewer();
    evdViewer->GetClipSet()->SetClipType(TGLClip::EType(0));

    //! Build 2nd tab with orthogonal viewers
    StartOrthoViewer();

    gEve->FullRedraw3D(kTRUE);

    //! Run Evd GUI
    root_app_->Run();
    root_app_->Terminate(0);
}

//---------------------------------------------------------------------------//
//! Create Evd ortho viewers (2nd tab in the GUI)
void Evd::StartOrthoViewer()
{
    //! Create 4 window slots

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

    //! Draw the contents of the 4 window slots

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
//! Evd splash screen
void Evd::PrintEvdLogo()
{
    std::cout << " --------------------------------------------------------\n";
    std::cout << " | Evd            https://github.com/celeritas-project/ |\n";
    std::cout << " |                                                      |\n";
    std::cout << " | An Event Display for the Celeritas Project           |\n";
    std::cout << " | (c) 2020 Oak Ridge National Laboratory               |\n";
    std::cout << " --------------------------------------------------------\n";
    std::cout << std::endl;
}
