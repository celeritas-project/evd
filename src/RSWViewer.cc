//----------------------------------*-C++-*----------------------------------//
// Copyright 2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file src/RSWViewer.hh
//---------------------------------------------------------------------------//
#include "RSWViewer.hh"

#include <TEveManager.h>
#include <TLeaf.h>
#include <TTreeIndex.h>
#include <assert.h>
#include <stdlib.h>

//---------------------------------------------------------------------------//
/*!
 * Construct with ROOT input filename.
 */
RSWViewer::RSWViewer(UPTFile tfile) : tfile_(std::move(tfile))
{
    assert(tfile_);
    ttree_.reset(tfile_->Get<TTree>("steps"));
    assert(ttree_);
}

//---------------------------------------------------------------------------//
/*!
 * Add event from Celeritas RootStepWriter.
 *
 * If event id is negative, all events are drawn.
 */
void RSWViewer::add_event(int const event_id)
{
    assert(ttree_->GetEntries() > event_id);

    // Sort tree first by event id, then by track id
    ttree_->BuildIndex("event_id", "track_id");
    auto tree_index = (TTreeIndex*)ttree_->GetTreeIndex();
    sorted_tree_index_ = tree_index->GetIndex();

    // Fetch last event id
    auto const last_entry = sorted_tree_index_[ttree_->GetEntries() - 1];
    ttree_->GetEntry(last_entry);
    auto const last_evtid = ttree_->GetLeaf("event_id")->GetValue();
    if (last_evtid < event_id)
    {
        std::cout << "[ERROR] event id " << event_id
                  << " is not available. Last event id is " << last_evtid
                  << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        this->create_event_tracks(event_id);
    }
}

//---------------------------------------------------------------------------//
// PRIVATE
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
/*!
 * Loop over steps tree, generate a TEveLine for each track id, and add it to
 * the viewer.
 */
void RSWViewer::create_event_tracks(int const event_id)
{
    struct TrackPoint
    {
        int step_count;
        std::array<double, 3> pos;
    };
    using TrackPoints = std::vector<TrackPoint>;

    // Initialize data
    auto track_line = new TEveLine(TEveLine::ETreeVarType_e::kTVT_XYZ);
    TrackPoints track_points;
    int current_trk_id{-1};

    // Loop over entries
    for (int i = 0; i < ttree_->GetEntries(); i++)
    {
        ttree_->GetEntry(sorted_tree_index_[i]);

        int const entry_evt_id = ttree_->GetLeaf("event_id")->GetValue();
        if (event_id >= 0)
        {
            // Only draw a single event

            if (entry_evt_id < event_id)
            {
                // Did not reach event yet
                continue;
            }
            if (entry_evt_id > event_id)
            {
                // Surpassed event id; stop
                break;
            }
        }

        int entry_trk_id = ttree_->GetLeaf("track_id")->GetValue();
        if (entry_trk_id != current_trk_id)
        {
            //// New track found ////

            if (i > 0)
            {
                // Set attributes, add full track to Eve
                auto pdg = ttree_->GetLeaf("particle")->GetValue();
                std::string track_name = std::to_string(entry_evt_id) + "_"
                                         + std::to_string(current_trk_id) + "_"
                                         + this->to_string((PDG)pdg);
                track_line->SetName(track_name.c_str());
                this->set_track_attributes(track_line, (PDG)pdg);

                // Sort track by step count
                std::sort(track_points.begin(),
                          track_points.end(),
                          [](TrackPoint const& lhs, TrackPoint const& rhs) {
                              return lhs.step_count < rhs.step_count;
                          });

                // Add sorted points to TEveLine
                for (auto p : track_points)
                {
                    track_line->SetNextPoint(p.pos[0], p.pos[1], p.pos[2]);
                }

                // Add line to Eve
                gEve->AddElement(track_line);
            }

            // Reset everything, add new track vertex
            {
                track_line = new TEveLine(TEveLine::ETreeVarType_e::kTVT_XYZ);
                track_points.clear();
                auto const& pos = ttree_->GetLeaf("pre_pos");
                TrackPoint p;
                p.step_count = ttree_->GetLeaf("track_step_count")->GetValue();
                p.pos = {pos->GetValue(0), pos->GetValue(1), pos->GetValue(2)};
                track_points.push_back(std::move(p));

                current_trk_id = entry_trk_id;
            }
        }

        else
        {
            // Add next step point
            auto const& pos = ttree_->GetLeaf("post_pos");
            TrackPoint p;
            p.step_count = ttree_->GetLeaf("track_step_count")->GetValue();
            p.pos = {pos->GetValue(0), pos->GetValue(1), pos->GetValue(2)};
            track_points.push_back(std::move(p));
        }
    }
}
