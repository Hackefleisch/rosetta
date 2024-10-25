// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/chemical/Orbital.fwd.hh
/// @author Phil Bradley


#ifndef INCLUDED_core_chemical_Orbital_fwd_hh
#define INCLUDED_core_chemical_Orbital_fwd_hh

#include <utility/pointer/owning_ptr.hh>
#include <utility/vector1.fwd.hh>

namespace core {
namespace chemical {

class Orbital;

typedef  utility::pointer::shared_ptr< Orbital >  OrbitalOP;
typedef  utility::pointer::shared_ptr< Orbital const >  OrbitalCOP;
typedef  utility::vector1< OrbitalOP >  OrbitalOPs;
typedef  utility::vector1< OrbitalCOP >  OrbitalCOPs;

OrbitalOP deep_copy( Orbital const & source);

} // chemical
} // core


#endif // INCLUDED_core_chemical_Orbital_FWD_HH
