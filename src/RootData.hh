//----------------------------------*-C++-*----------------------------------//
// Copyright 2021 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file RootData.hh
//! \brief Data structures for the ROOT output file.
//---------------------------------------------------------------------------//
#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>

namespace rootdata
{
//---------------------------------------------------------------------------//
/*!
 * Structs for generic usage.
 */
struct Vector3
{
    double x;
    double y;
    double z;

    double operator[](int i)
    {
        switch (i)
        {
            case 0:
                return this->x;
                break;
            case 1:
                return this->y;
                break;
            case 2:
                return this->z;
                break;
            default:
                return 0;
        }
    }
};

//---------------------------------------------------------------------------//
/*!
 * Structs for sensitive detector scoring. Sensitive detector names are stored
 * only once in a separate map.
 */
struct HitData
{
    unsigned int track_id;
    unsigned int parent_id;
    double       energy_deposition;
    Vector3      position;
};

struct SensitiveDetectorScore
{
    double       energy_deposition; //!< [MeV]
    unsigned int number_of_steps;
};

//---------------------------------------------------------------------------//
/*!
 * Structs for particles/tracks/events.
 */
enum class ProcessNameId
{
    transportation,
    ion_ioni,
    msc,
    h_ioni,
    h_brems,
    h_pair_prod,
    coulomb_scat,
    e_ioni,
    e_brems,
    photoelectric,
    compton,
    conversion,
    rayleigh,
    annihilation,
    mu_ioni,
    mu_brems,
    mu_pair_prod,
    unknown
};

struct Step
{
    ProcessNameId process_id;
    double        kinetic_energy; //!< [MeV]
    double        energy_loss;    //!< [MeV]
    Vector3       direction;      //!< Unit vector
    Vector3       position;       //!< [cm]
    double        global_time;    //!< [s]
};

struct Track
{
    int               pdg;
    int               id;
    int               parent_id;
    double            length;             //!< [MeV]
    double            energy_dep;         //!< [MeV]
    double            vertex_energy;      //!< [MeV]
    double            vertex_global_time; //!< [s]
    Vector3           vertex_direction;   //!< Unit vector
    Vector3           vertex_position;    //!< [cm]
    unsigned long     number_of_steps;
    std::vector<Step> steps;
};

struct Event
{
    int                                 id;
    std::vector<Track>                  primaries;
    std::vector<Track>                  secondaries;
    std::vector<SensitiveDetectorScore> sensitive_detectors;
};

struct Primary
{
    int     pdg;
    double  energy;
    Vector3 vertex;
    Vector3 momentum;
};

//---------------------------------------------------------------------------//
/*!
 * Struct for performance metrics. Time units must be specified when used.
 */
struct ExecutionTime
{
    double wall_total;
    double cpu_total;
    double wall_sim_run;
    double cpu_sim_run;
};

//---------------------------------------------------------------------------//
/*!
 * Struct for storing max values. Especially useful to simplify histogram
 * definitions during the analysis.
 */
struct DataLimits
{
    unsigned long max_num_primaries;
    unsigned long max_primary_num_steps;
    unsigned long max_secondary_num_steps;
    unsigned long max_num_secondaries;
    unsigned long max_steps_per_event;

    double max_primary_energy;
    double max_secondary_energy;

    double       max_sd_energy;
    unsigned int max_sd_num_steps;

    Vector3 min_vertex;
    Vector3 max_vertex;
};

//---------------------------------------------------------------------------//
// Free functions
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
/*!
 * Map Geant4 string names and our enums.
 */
const std::map<std::string, ProcessNameId> process_map = {
    // clang-format off
    {"Transportation", ProcessNameId::transportation},
    {"ionIoni",        ProcessNameId::ion_ioni},
    {"msc",            ProcessNameId::msc},
    {"hIoni",          ProcessNameId::h_ioni},
    {"hBrems",         ProcessNameId::h_brems},
    {"hPairProd",      ProcessNameId::h_pair_prod},
    {"CoulombScat",    ProcessNameId::coulomb_scat},
    {"eIoni",          ProcessNameId::e_ioni},
    {"eBrem",          ProcessNameId::e_brems},
    {"phot",           ProcessNameId::photoelectric},
    {"compt",          ProcessNameId::compton},
    {"conv",           ProcessNameId::conversion},
    {"Rayl",           ProcessNameId::rayleigh},
    {"annihil",        ProcessNameId::annihilation},
    {"muIoni",         ProcessNameId::mu_ioni},
    {"muBrems",        ProcessNameId::mu_brems},
    {"muPairProd",     ProcessNameId::mu_pair_prod}
    // clang-format on
};

//---------------------------------------------------------------------------//
/*!
 * Safely retrieve the correct process enum from a given string.
 */
static ProcessNameId to_process_name_id(const std::string& process_name)
{
    auto iter = process_map.find(process_name);
    if (iter == process_map.end())
    {
        return ProcessNameId::unknown;
    }
    return iter->second;
}

//---------------------------------------------------------------------------//
/*!
 * Safely retrieve the correct process string from a given enum.
 */
static std::string to_process_name(ProcessNameId process_name_id)
{
    auto result = std::find_if(process_map.begin(),
                               process_map.end(),
                               [process_name_id](const auto& process_map) {
                                   return process_map.second == process_name_id;
                               });

    if (result == process_map.end())
    {
        return "ProcessNameId " + std::to_string((int)process_name_id)
               + " not found";
    }

    return result->first;
}

//---------------------------------------------------------------------------//
} // namespace rootdata
