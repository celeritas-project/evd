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

//---------------------------------------------------------------------------//
/*!
 * Construct with ROOT input filename.
 */
RSWViewer::RSWViewer(UPTFile tfile)
    : MCTruthViewerInterface(), tfile_(std::move(tfile))
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

    // Sort tree by a major and minor value
    ttree_->BuildIndex("track_id", "track_step_count");
    auto tree_index = (TTreeIndex*)ttree_->GetTreeIndex();
    sorted_tree_index_ = tree_index->GetIndex();

    // TODO: Make this more efficient. Now it has read the whole file
    this->CreateEventTracks(event_id);
}

//---------------------------------------------------------------------------//
// PRIVATE
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
/*!
 * Loop over steps tree, generate a TEveLine for each track id, and add it to
 * the viewer.
 */
void RSWViewer::CreateEventTracks(int const event_id)
{
    auto track_line = new TEveLine(TEveLine::ETreeVarType_e::kTVT_XYZ);

    // Store vertex
    auto const& pos = ttree_->GetLeaf("pre_pos");
    track_line->SetNextPoint(
        pos->GetValue(0), pos->GetValue(1), pos->GetValue(2));

    int current_trk_id = 0;
    for (int i = 0; i < ttree_->GetEntries(); i++)
    {
        ttree_->GetEntry(sorted_tree_index_[i]);

        int this_evt_id = ttree_->GetLeaf("event_id")->GetValue();
        if (event_id >= 0 && this_evt_id != event_id)
        {
            // Skip entry
            continue;
        }

        int this_trkid = ttree_->GetLeaf("track_id")->GetValue();
        if (this_trkid != current_trk_id)
        {
            // New track found
            // Set attributes, add full track to Eve
            {
                auto pdg = ttree_->GetLeaf("particle")->GetValue();
                std::string track_name = std::to_string(this_evt_id) + "_"
                                         + std::to_string(current_trk_id) + "_"
                                         + this->to_string((PDG)pdg);
                track_line->SetName(track_name.c_str());
                this->set_track_attributes(track_line, (PDG)pdg);
                gEve->AddElement(track_line);
            }
            // Reset track line, collect vertex of new track
            {
                track_line = new TEveLine(TEveLine::ETreeVarType_e::kTVT_XYZ);
                auto const& pos = ttree_->GetLeaf("pre_pos");
                track_line->SetNextPoint(
                    pos->GetValue(0), pos->GetValue(1), pos->GetValue(2));
            }
            current_trk_id = this_trkid;
        }

        else
        {
            // Add next step point
            auto const& pos = ttree_->GetLeaf("post_pos");
            track_line->SetNextPoint(
                pos->GetValue(0), pos->GetValue(1), pos->GetValue(2));
        }
    }
}
