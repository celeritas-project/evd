//----------------------------------*-C++-*----------------------------------//
// Copyright 2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file src/RootDataViewer.hh
//---------------------------------------------------------------------------//
#include "RootDataViewer.hh"

#include <TEveManager.h>
#include <assert.h>

//---------------------------------------------------------------------------//
/*!
 * Construct with ROOT input filename.
 */
RootDataViewer::RootDataViewer(UPTFile tfile)
    : MCTruthViewerInterface(), tfile_(std::move(tfile))
{
    assert(tfile_);
    ttree_.reset(tfile_->Get<TTree>("events"));
    assert(ttree_);
}

//---------------------------------------------------------------------------//
/*!
 * Add event from benchmarks/geant4-validation-app.
 *
 * If event id is negative, all events are drawn.
 */
void RootDataViewer::add_event(int const event_id)
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
void RootDataViewer::CreateEventTracks(
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
RootDataViewer::CreateTrackLine(rootdata::Track const& track,
                                int const event_id)
{
    std::string track_name = std::to_string(event_id) + "_"
                             + std::to_string(track.id) + "_"
                             + this->to_string((PDG)track.pdg);

    auto track_line
        = std::make_unique<TEveLine>((TEveLine::ETreeVarType_e::kTVT_XYZ));
    track_line->SetName(track_name.c_str());
    this->set_track_attributes(track_line.get(), (PDG)track.pdg);

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
