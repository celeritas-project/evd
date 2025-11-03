//------------------------------- -*- C++ -*- -------------------------------//
// Copyright Celeritas contributors: see top-level COPYRIGHT file for details
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file src/JsonViewer.hh
//---------------------------------------------------------------------------//
#pragma once

#include <fstream>
#include <memory>
#include <string>
#include <nlohmann/json.hpp>

#include "MCTruthViewerInterface.hh"

//---------------------------------------------------------------------------//
/*!
 * Draw event MC truth data from \c nlohmann-json output files.
 *
 * This is a secondary class meant to be used along with \c MainViewer , which
 * *MUST* be initialized before this class is constructed.
 */
class JsonViewer final : public MCTruthViewerInterface
{
  public:
    // Construct with Json input file
    JsonViewer(std::string input);

    // Add tracks for given event
    void add_event(int event_id) override;

  private:
    //// DATA ////

    // Input jsonl file
    std::ifstream input_;
    // JSONL parser (parsed for every new file line)
    nlohmann::json jsonl_;

    // Pre- and post-step information
    enum class StepPoint
    {
        pre,
        post,
        size_
    };

    // Track point from json
    struct Point
    {
        StepPoint step;
        size_t num_step;
        std::array<double, 3> pos;
    };
    // Define a track as a list of points
    using Track = std::vector<Point>;

    //// HELPER FUNCTIONS ////

    // Create line from list of points
    std::unique_ptr<TEveLine> create_track_line(Track track);

    // Loop over tracks and add track lines to Eve
    void create_event_tracks();

    // Load a Point object from the jsonl_ data
    Point load_point();
};
