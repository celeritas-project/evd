//---------------------------------*-C++-*-----------------------------------//
/*!
 * \file   Evd/include/Evd.hh
 * \author Stefano Tognini
 * \brief  Event Display for the Celeritas Project.
 * \note   Copyright (c) 2020 Oak Ridge National Laboratory, UT-Battelle, LLC.
 */
//---------------------------------------------------------------------------//


#ifndef Evd_hh
#define Evd_hh


// ROOT
#include "TRint.h"
#include "TROOT.h"
#include "TMath.h"

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"

#include "TGeoManager.h"

#include "TEveManager.h"
#include "TEveWindow.h"
#include "TEveViewer.h"
#include "TEveBrowser.h"
#include "TEveGeoNode.h"
#include "TEveTrack.h"

#include "TGLViewer.h"


class Evd
{
public:
    Evd();
    ~Evd();
    
    void LoadGeometry(char const * gdmlFile);
    void LoadRoot(char const * rootFile);
    void PrintVolumeNodes(TGeoVolume * geoVolume);
    TGeoVolume * GetTopVolume();
    TGeoVolume * GetVolumeNode(TGeoVolume * geoVolume, char const * node);
    void AddVolume(TGeoVolume * geoVolume);
    void AddCMSVolume(TGeoVolume * geoVolume);
    void AddEvent(int const &event, int const &trackLimit);
    
    void AddEventTemp(const int &event, const int &trackLimit);

    void SetVisLevel(int const &visLevel);
    void StartViewer();
    
private:
    TRint       * b_app;
    TGeoManager * b_geoManager;
    TEveManager * b_eveManager;
    TFile       * b_rootFile;
    int           visLevel;
    double const  cm = 10; // TGeoManager uses cm while GDML uses mm
    
    void StartOrthoViewer();
    void PrintEvdLogo();
};


#endif /* Evd_hh */
