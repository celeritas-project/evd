//----------------------------------*-C++-*----------------------------------//
// Copyright 2023 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file src/RootUniquePtr.hh
//---------------------------------------------------------------------------//
#pragma once

#include <TFile.h>
#include <TTree.h>
#include <assert.h>

//---------------------------------------------------------------------------//
/*!
 * Custom deleter for ROOT classes.
 */
template<class T>
struct RootExternDeleter
{
    void operator()(T* ptr)
    {
        assert(ptr);
        delete ptr;
    }
};

//---------------------------------------------------------------------------//
// Type aliases
template<class T>
using UPRootExtern = std::unique_ptr<T, RootExternDeleter<T>>;

//---------------------------------------------------------------------------//
// EXPLICIT INSTANTIATIONS
//---------------------------------------------------------------------------//
template struct RootExternDeleter<TFile>;
template struct RootExternDeleter<TTree>;
