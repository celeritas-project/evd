//---------------------------------*-C++-*-----------------------------------//
/*!
 * \file   Evd/src/Evd.cpp
 * \author Stefano Tognini
 * \brief  Event Display for the Celeritas Project.
 * \note   Copyright (c) 2020 Oak Ridge National Laboratory, UT-Battelle, LLC.
 */
//---------------------------------------------------------------------------//


// C++
#include <iostream>

// Project
#include "Evd.hh"


//------------------------------------ Evd ----------------------------------//
Evd::Evd() : visLevel(1)
{
    PrintEvdLogo();
    b_app = new TRint("Evd", nullptr, nullptr, nullptr, 0, kTRUE);
    b_app->SetPrompt("Evd [%d] ");
    
    TEveManager::Create();
    b_eveManager = gEve;
}


//----------------------------------- ~Evd ----------------------------------//
Evd::~Evd()
{}


//------------------------------- LoadGeometry ------------------------------//
void Evd::LoadGeometry(const char *gdmlFile)
{
    std::cout << std::endl;
    std::cout << "------------------- Loading geometry -------------------\n";
    std::cout << gdmlFile << std::endl;
    std::cout << std::endl;
    
    TGeoManager::SetVerboseLevel(0);
    TGeoManager::Import(gdmlFile);
    b_geoManager = gGeoManager;
}


//--------------------------------- LoadRoot --------------------------------//
void Evd::LoadRoot(const char *rootFile)
{
    std::cout << "------------------- Loading ROOT file ------------------\n";
    std::cout << rootFile << std::endl;
    std::cout << std::endl;
    
    this->rootFile = new TFile(rootFile);
}


//----------------------------- PrintVolumeNodes ----------------------------//
void Evd::PrintVolumeNodes(TGeoVolume * geoVolume)
{
    std::cout << "--------------------- Volume nodes ---------------------\n";
    std::cout << geoVolume->GetName() << std::endl;

    TObjArray * nodeList = geoVolume->GetNodes();
    
    if (nodeList)
    {
        for (TObject * node : * nodeList)
        {
            std::cout << " | " << node->GetName() << std::endl;
        }
        std::cout << std::endl;
    }
}


//------------------------------ GetTopVolume -------------------------------//
TGeoVolume * Evd::GetTopVolume()
{
    return b_geoManager->GetTopVolume();
}


//------------------------------ GetVolumeNode ------------------------------//
TGeoVolume * Evd::GetVolumeNode(TGeoVolume * geoVolume, const char * node)
{
    return geoVolume->FindNode(node)->GetVolume();
}


//-------------------------------- AddVolume --------------------------------//
void Evd::AddVolume(TGeoVolume * geoVolume)
{
    std::cout << "----------------- Volumes added to Evd -----------------\n";
    std::cout << geoVolume->GetName() << std::endl;
    
    TObjArray * objectList = geoVolume->GetNodes();

    for (TObject * object : * objectList)
    {
        const char * objectName = object->GetName();
        
        TEveGeoTopNode * eveNode =
        new TEveGeoTopNode(b_geoManager, geoVolume->FindNode(objectName));
        eveNode->SetVisLevel(visLevel);
        b_eveManager->AddGlobalElement(eveNode);
        
        TGeoVolume * eveVol = geoVolume->FindNode(objectName)->GetVolume();
        std::cout << " | " << eveVol->GetName() << std::endl;
    }
    std::cout << std::endl;
}


//------------------------------- AddCMSVolume ------------------------------//
void Evd::AddCMSVolume(TGeoVolume * geoVolume)
{
    TGeoVolume * cmseVol = geoVolume->FindNode("CMSE0x7f4a8f616d40")->GetVolume();
    TEveGeoTopNode * cmseTopNode =
    new TEveGeoTopNode(b_geoManager, geoVolume->FindNode("CMSE0x7f4a8f616d40"));

    cmseTopNode->SetVisLevel(visLevel);
    b_eveManager->AddGlobalElement(cmseTopNode);
    
    // Setting some elements as invisible
    std::vector<std::string> nodeSkipList;
    nodeSkipList.push_back("CMStoZDC0x7f4a9a757000");
    nodeSkipList.push_back("ZDCtoFP4200x7f4a9a757180");
    nodeSkipList.push_back("BEAM30x7f4a8f615040");
    nodeSkipList.push_back("BEAM20x7f4a9a75ae00");
    nodeSkipList.push_back("VCAL0x7f4a8f615540");
    nodeSkipList.push_back("CastorF0x7f4a8f615f80");
    nodeSkipList.push_back("CastorB0x7f4a8f616080");
    nodeSkipList.push_back("TotemT20x7f4a8f615ac0");
    nodeSkipList.push_back("OQUA0x7f4a8f616600");
    nodeSkipList.push_back("BSC20x7f4a8f616740");
    nodeSkipList.push_back("ZDC0x7f4a8f6168c0");
    
    for (std::string node : nodeSkipList)
    {
        TGeoVolume * cmseSubvol = cmseVol->FindNode(node.c_str())->GetVolume();
        cmseSubvol->InvisibleAll();
        cmseSubvol->SetVisDaughters(0);
    }
    
    std::cout << "------------ CMS detector only option enabled ----------\n";
    std::cout << "Some geometry elements are set to invisible" << std::endl;
    std::cout << std::endl;
    std::cout << "----------------- Volumes added to Evd -----------------\n";
    std::cout << geoVolume->GetName() << std::endl;
    std::cout << " | " << cmseVol->GetName() << std::endl;
    std::cout << std::endl;
}


//--------------------------------- AddEvent --------------------------------//
void Evd::AddEvent(const int &event, const int &trackLimit)
{
    std::cout << "--------------------- Loading event --------------------\n";
    std::cout << "Event " << event << std::endl;
    std::cout << "Printing ";
    (trackLimit == 0 ? std::cout << "all" : std::cout << trackLimit);
    std::cout << " tracks" << std::endl;
    std::cout << std::endl;
    std::cout << "Loading";

    // Loading ROOT TTrees
    TTree * eventTree = (TTree*)rootFile->Get("event");
    TTree * stepTree  = (TTree*)rootFile->Get("step");
    
    // Fetching trackID list for a given event
    std::vector<int> * trkIDlist = 0;
    eventTree->SetBranchAddress("trkIDlist", &trkIDlist);
    eventTree->GetEntry(event);

    // Creating a std::vector of track lines to be added to the Evd
    std::vector<TEveLine *> trkLines;
    trkLines.reserve(trkIDlist->size() * sizeof(TEveLine *));
    
    // Creating the first line before entering the loop
    trkLines.push_back(new TEveLine(TEveLine::ETreeVarType_e::kTVT_XYZ));
    
    // Finding the number of entries (i.e. steps)
    long long numberOfEntries = stepTree->GetEntries();
    
    // Keeping score of the number tracks
    unsigned int i_trkIDlist = 0;
    
    // Looping over entries (i.e. steps)
    for (long long i = 0; i < numberOfEntries; i++)
    {
        // Printing status (48 is just the column width)
        if (i % (numberOfEntries/48) == 0) std::cerr << ".";

        // Fetching step entry
        stepTree->GetEntry(i);
        
        // Fetching event and track IDs
        int evtID = stepTree->GetLeaf("evtID")->GetValue();
        int trkID = stepTree->GetLeaf("trkID")->GetValue();

        // Skipping unwanted events
        if (evtID != event) continue;
        if (evtID > event)  continue;
        
        // If this trackID is one of the tracks of the chosen event
        if (trkID == trkIDlist->at(i_trkIDlist))
        {
            // Fetching step position
            double x = stepTree->GetLeaf("stepX")->GetValue();
            double y = stepTree->GetLeaf("stepY")->GetValue();
            double z = stepTree->GetLeaf("stepZ")->GetValue();
            
            // Skipping steps that are too far from the CMS detector
            // This improves visualization and decreases load time
            if (TMath::Abs(x) > 1500) continue;
            if (TMath::Abs(y) > 1500) continue;
            if (TMath::Abs(z) > 1500) continue;

            // Adding step point for the track line
            trkLines.at(i_trkIDlist)->SetNextPoint(x, y, z);
        }
        
        // If either a new track is found or the last entry was reached
        else if (trkID != trkIDlist->at(i_trkIDlist) ||
                 i == numberOfEntries - 1)
        {
            // Setting line properties
            int PDG = stepTree->GetLeaf("stepPDG")->GetValue();
            int absPDG = TMath::Abs(PDG);
            
            // Electrons are green; muons are red; fotons are cyan;
            // others are gray
            if      (absPDG == 11) trkLines.at(i_trkIDlist)->SetLineColor(kGreen);
            else if (absPDG == 13) trkLines.at(i_trkIDlist)->SetLineColor(kRed);
            else if (absPDG == 22) trkLines.at(i_trkIDlist)->SetLineColor(kBlue);
            else trkLines.at(i_trkIDlist)->SetLineColor(kGray);
                            
            // Creating the line name
            // Line names are shown at the left side of Evd
            std::string lineName;
            lineName = std::to_string(PDG) + ", " + std::to_string(trkID);
            
            trkLines.at(i_trkIDlist)->SetName(lineName.c_str());
            trkLines.at(i_trkIDlist)->SetLineWidth(1);
            trkLines.at(i_trkIDlist)->SetMarkerColor(kYellow);

            // Adding line to the viewer
            int lineNsteps = trkLines.at(i_trkIDlist)->GetN();
            if (lineNsteps > 0)
                b_eveManager->AddElement(trkLines.at(i_trkIDlist));
            
            // If the maximum number of tracks to be drawn is reached, stop
            if (trackLimit != 0 && i_trkIDlist == trackLimit) break;

            // Starting a new line
            trkLines.push_back(new TEveLine(TEveLine::ETreeVarType_e::kTVT_XYZ));
            
            // Moving to the next track ID in trkIDlist
            i_trkIDlist++;
        }
    }
    std::cout << std::endl;
    std::cout << std::endl;
}


//------------------------------- SetVisLevel -------------------------------//
void Evd::SetVisLevel(const int &visLevel)
{
    this->visLevel = visLevel;
}


//------------------------------- StartViewer -------------------------------//
void Evd::StartViewer()
{
    // Setting window name
    b_eveManager->GetBrowser()->TRootBrowser::SetWindowName("Celeritas Event Display");

    // Hiding command line box
    b_eveManager->GetBrowser()->HideBottomTab();
    
    // Setting viewer name
    b_eveManager->GetDefaultViewer()->SetElementName("Main viewer");
    
    // Setting box clipping to the geometry in the Main viewer
    TGLViewer * evdViewer = b_eveManager->GetDefaultGLViewer();
    evdViewer->GetClipSet()->SetClipType(TGLClip::EType(2));
    
    // Building 2nd tab with orthogonal viewers
    StartOrthoViewer();
    
    // Restoring focus to the left tab after building right tab
    b_eveManager->GetBrowser()->GetTabLeft();
    
    // Updating
    b_eveManager->FullRedraw3D(kTRUE);

    b_app->Run();
    b_app->Terminate(0);
}


//----------------------------- StartOrthoViewer ----------------------------//
void Evd::StartOrthoViewer()
{
    // Creating 4 window slots
    
    // Creating top window to contain all 4 slots
    TEveWindowSlot * slot =
    TEveWindow::CreateWindowInTab(b_eveManager->GetBrowser()->GetTabRight());
    TEveWindowPack * packMaster = slot->MakePack();
    packMaster->SetElementName("Projections");
    packMaster->SetHorizontal();
    packMaster->SetShowTitleBar(kFALSE);

    // Creating slots on the left side
    slot = packMaster->NewSlot();
    TEveWindowPack * packLeft = slot->MakePack();
    packLeft->SetShowTitleBar(kFALSE);
    TEveWindowSlot * slotLeftTop    = packLeft->NewSlot();
    TEveWindowSlot * slotLeftBottom = packLeft->NewSlot();
    
    // Creating slots on the right side
    slot = packMaster->NewSlot();
    TEveWindowPack * packRight = slot->MakePack();
    packRight->SetShowTitleBar(kFALSE);
    TEveWindowSlot * slotRightTop    = packRight->NewSlot();
    TEveWindowSlot * slotRightBottom = packRight->NewSlot();
    
    
    // Drawing the contents of the 4 window slots

    // Top left slot
    slotLeftTop->MakeCurrent();
    TEveViewer * eveXYView;
    eveXYView = b_eveManager->SpawnNewViewer("XY View", "");
    eveXYView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
    eveXYView->GetGLViewer()->SetStyle(TGLRnrCtx::kWireFrame);
    eveXYView->AddScene(b_eveManager->GetGlobalScene());
    eveXYView->AddScene(b_eveManager->GetEventScene());
    
    // Top right slot
    slotRightTop->MakeCurrent();
    TEveViewer * eveZYView;
    eveZYView = b_eveManager->SpawnNewViewer("ZY View", "");
    eveZYView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoZOY);
    eveZYView->GetGLViewer()->SetStyle(TGLRnrCtx::kWireFrame);
    eveZYView->AddScene(b_eveManager->GetGlobalScene());
    eveZYView->AddScene(b_eveManager->GetEventScene());
    
    // Bottom left slot
    slotLeftBottom->MakeCurrent();
    TEveViewer * eveXZView;
    eveXZView = b_eveManager->SpawnNewViewer("XZ View", "");
    eveXZView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOZ);
    eveXZView->GetGLViewer()->SetStyle(TGLRnrCtx::kWireFrame);
    eveXZView->AddScene(b_eveManager->GetGlobalScene());
    eveXZView->AddScene(b_eveManager->GetEventScene());
    
    // Bottom right slot
    slotRightBottom->MakeCurrent();
    TEveViewer * eve3DView;
    eve3DView = b_eveManager->SpawnNewViewer("3D View", "");
    eve3DView->GetGLViewer()->SetStyle(TGLRnrCtx::kWireFrame);
    eve3DView->AddScene(b_eveManager->GetGlobalScene());
    eve3DView->AddScene(b_eveManager->GetEventScene());
}


//------------------------------- PrintEvdLogo ------------------------------//
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
