//----------------------------------*-C++-*----------------------------------//
// Copyright 2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file src/EventViewer.cc
//---------------------------------------------------------------------------//
#include "EventViewer.hh"

#include <assert.h>

//---------------------------------------------------------------------------//
/*!
 * Construct with ROOT input filename.
 */
EventViewer::EventViewer(std::string root_filename)
{
    tfile_.reset(TFile::Open(root_filename.c_str(), "read"));
    assert(tfile_.IsOpen());
    ttree_.reset((TTree*)tfile_->Get("events"));
    assert(ttree_);

    std::cout << "Simulation input: " << root_filename << std::endl;
}

//---------------------------------------------------------------------------//
/*!
 * Add event from benchmarks/geant4-validation-app.
 *
 * If event id is negative, all events are drawn.
 */
void EventViewer::AddEvent(int const event_id)
{
    assert(ttree_->GetEntries() > event_id);

    rootdata::Event* event = nullptr;
    ttree_->SetBranchAddress("event", &event);

    // If negative, loop over all events; Otherwise, just draw selected event
    int const first = (event_id < 0) ? 0 : event_id;
    int const last = (event_id < 0) ? ttree_->GetEntries() : event_id + 1;

    for (auto i = first; i < last; i++)
    {
        ttree_->GetEntry(i);
        this->CreateEventTracks(event->primaries, event->id);
        this->CreateEventTracks(event->secondaries, event->id);
    }
}

//---------------------------------------------------------------------------//
// PRIVATE
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
/*!
 * Loop over a vector of tracks (either primaries or secondaries), generate a
 * TEveLine for each, and add them to the viewer.
 */
void EventViewer::CreateEventTracks(
    std::vector<rootdata::Track> const& vec_tracks, int const event_id)
{
    for (auto const& track : vec_tracks)
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
EventViewer::CreateTrackLine(rootdata::Track const& track, int const event_id)
{
    std::string track_name = std::to_string(event_id) + "_"
                             + std::to_string(track.id) + "_"
                             + this->to_string((PDG)track.pdg);

    auto track_line
        = std::make_unique<TEveLine>((TEveLine::ETreeVarType_e::kTVT_XYZ));
    track_line->SetName(track_name.c_str());
    this->SetTrackAttributes(track_line.get(), (PDG)track.pdg);

    // Store vertex
    auto const& vtx = track.vertex_position;
    track_line->SetNextPoint(vtx.x, vtx.y, vtx.z);

    for (auto const& step : track.steps)
    {
        // Store steps
        auto const& pos = step.position;
        track_line->SetNextPoint(pos.x, pos.y, pos.z);
    }

    return track_line;
}

//---------------------------------------------------------------------------//
/*!
 * Convert PDG to string.
 */
std::string EventViewer::to_string(PDG id)
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
void EventViewer::SetTrackAttributes(TEveLine* track, PDG pdg)
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
