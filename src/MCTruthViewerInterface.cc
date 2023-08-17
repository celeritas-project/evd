//----------------------------------*-C++-*----------------------------------//
// Copyright 2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file src/MCTruthViewerInterface.cc
//---------------------------------------------------------------------------//
#include "MCTruthViewerInterface.hh"

//---------------------------------------------------------------------------//
/*!
 * Draw each step point along the track.
 */
void MCTruthViewerInterface::show_step_points(bool value)
{
    step_points_ = value;
}

//---------------------------------------------------------------------------//
/*!
 * Convert PDG to string.
 */
std::string MCTruthViewerInterface::to_string(PDG id)
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
            return "pdg-" + std::to_string(id);
    }
}

//---------------------------------------------------------------------------//
/*!
 * Set up attributes of a TEveLine for drawing tracks.
 */
void MCTruthViewerInterface::set_track_attributes(TEveLine* track, PDG pdg)
{
    switch (pdg)
    {
        case PDG::gamma:
            track->SetLineColor(kGreen);
            track->SetMarkerColor(kYellow);
            break;
        case PDG::e_minus:
            track->SetLineColor(kAzure + 1);
            track->SetMarkerColor(kRed);
            break;
        case PDG::e_plus:
            track->SetLineColor(kRed);
            track->SetMarkerColor(kBlue);
            break;
        case PDG::mu_minus:
            track->SetLineColor(kOrange);
            break;
        default:
            track->SetLineColor(kGray);
    }
}
