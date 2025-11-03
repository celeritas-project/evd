//------------------------------- -*- C++ -*- -------------------------------//
// Copyright Celeritas contributors: see top-level COPYRIGHT file for details
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file src/JsonViewer.cc
//---------------------------------------------------------------------------//
#include "JsonViewer.hh"

#include <TEveManager.h>
#include <assert.h>

//---------------------------------------------------------------------------//
/*!
 * Construct with JSON input filename.
 */
JsonViewer::JsonViewer(std::string input)
{
    input_ = std::ifstream(input);
}

//---------------------------------------------------------------------------//
/*!
 * Add event from JSON.
 *
 * If event id is negative, all events are drawn.
 */
void JsonViewer::add_event(int const event_id = 0)
{
    this->create_event_tracks();
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
    std::string track_name = this->to_string(PDG::optical_photon);

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
void JsonViewer::create_event_tracks()
{
    Track track;
    std::string line;
    while (std::getline(input_, line))
    {
        jsonl_ = nlohmann::json::parse(line);
        auto const point = this->load_point();
        if (point.step == StepPoint::pre)
        {
            if (!track.empty())
            {
                // Create and add line to event display
                auto track_line = this->create_track_line(track);
                gEve->AddElement(track_line.release());
                track.clear();
            }
        }
        track.push_back(point);
    }
}

//---------------------------------------------------------------------------//
/*!
 * Load a Point object from existing data loaded into \c jsonl_ .
 */
JsonViewer::Point JsonViewer::load_point()
{
    Point p;
    p.step = StepPoint(jsonl_.at("step_point").get<size_t>());
    p.num_step = jsonl_.at("num_steps").get<size_t>();
    p.pos[0] = jsonl_.at("x").get<double>();
    p.pos[1] = jsonl_.at("y").get<double>();
    p.pos[2] = jsonl_.at("z").get<double>();
    return p;
}
