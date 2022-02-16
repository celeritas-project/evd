//----------------------------------*-C++-*----------------------------------//
// Copyright 2020-2022 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file RootInterfaceLinkDef.hh
//! \brief Classes added to the ROOT dictionary.
//---------------------------------------------------------------------------//
#ifdef __CINT__

// clang-format off
#pragma link C++ struct rootdata::Vector3+;
#pragma link C++ struct rootdata::SensitiveDetectorScore+;
#pragma link C++ struct rootdata::Step+;
#pragma link C++ struct rootdata::Track+;
#pragma link C++ struct rootdata::Event+;
#pragma link C++ struct rootdata::ExecutionTime+;
#pragma link C++ struct rootdata::DataLimits+;
// clang-format on

#endif
