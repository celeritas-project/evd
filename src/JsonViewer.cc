//------------------------------- -*- C++ -*- -------------------------------//
// Copyright Celeritas contributors: see top-level COPYRIGHT file for details
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file src/JsonViewer.cc
//---------------------------------------------------------------------------//
#include "JsonViewer.hh"

#include <fstream>
#include <TEveManager.h>
#include <assert.h>

//---------------------------------------------------------------------------//
/*!
 * Construct with JSON input filename.
 */
JsonViewer::JsonViewer(std::string input)
{
    assert(!input.empty());
    json_ = nlohmann::json::parse(std::ifstream(input));
    assert(!json_.is_null());
}

//---------------------------------------------------------------------------//
/*!
 * Add event from JSON.
 *
 * If event id is negative, all events are drawn.
 */
void JsonViewer::add_event(int const event_id)
{
    // Implement me
}

//---------------------------------------------------------------------------//
// PRIVATE
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
/*!
 * Create line from list of points.
 */
std::unique_ptr<TEveLine> JsonViewer::create_track_line(Track track)
{
    size_t event_id;  // \todo: load event id
    size_t track_id;  // \todo: load track id
    std::string track_name = std::to_string(event_id) + "_"
                             + std::to_string(track_id) + "_"
                             + this->to_string(PDG::optical_photon);

    auto track_line
        = std::make_unique<TEveLine>((TEveLine::ETreeVarType_e::kTVT_XYZ));
    track_line->SetName(track_name.c_str());
    this->set_track_attributes(track_line.get(), PDG::optical_photon);
    for (auto const& p : track)
    {
        track_line->SetNextPoint(p.pos[0], p.pos[1], p.pos[2]);
    }

    return track_line;
}

//---------------------------------------------------------------------------//
/*!
 * Add every track found in a given event.
 */
void JsonViewer::create_event_tracks(int const event_id)
{
    using TrackSlotId = size_t;

    // Map track_slot_ids to tracks
    std::unordered_map<TrackSlotId, Track> track_map;

    // Loop over json; For a given event_id
    {
        // todo: if event_id is negative, add all points

        auto const& point = json_.at("point");
        auto const id = point.at("track_slot_id").get<size_t>();
        if (track_map.find(id) == track_map.end())
        {
            track_map.insert({id, Track()});
        }

        auto iter = track_map.find(id);
        Point p;
        p.num_step = point.at("num_step").get<size_t>();
        p.pos[0] = point.at("x").get<double>();
        p.pos[1] = point.at("y").get<double>();
        p.pos[2] = point.at("z").get<double>();
        iter->second.push_back(std::move(p));
    }

    for (auto [id, track] : track_map)
    {
        // Sort track by step count
        std::sort(
            track.begin(), track.end(), [](Point const& lhs, Point const& rhs) {
                return lhs.num_step < rhs.num_step;
            });

        // Create and add line to event display
        auto line = this->create_track_line(track);
        gEve->AddElement(line.release());
    }
}
