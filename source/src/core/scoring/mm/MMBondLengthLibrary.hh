// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/scoring/mm/MMBondLengthLibrary.hh
/// @brief  Molecular mechanics bond length score class
/// @author Frank DiMaio (based on Colin Smith's MMBondAngle potential)


#ifndef INCLUDED_core_scoring_mm_MMBondLengthLibrary_hh
#define INCLUDED_core_scoring_mm_MMBondLengthLibrary_hh

// Project headers
#include <core/chemical/MMAtomTypeSet.fwd.hh>
#include <core/types.hh>

// Utility header
#include <utility/vector1.hh>
#include <utility/VirtualBase.hh>

// C++ headers
#include <map>

#include <utility/keys/Key2Tuple.fwd.hh>

#ifdef PYROSETTA
#include <utility/keys/Key2Tuple.hh>
#endif

namespace core {
namespace scoring {
namespace mm {

// all ints for now
typedef utility::keys::Key2Tuple< int, int > mm_bondlength_atom_pair;
typedef utility::keys::Key2Tuple< core::Real, core::Real > mm_bondlength_param_set;
typedef std::multimap< mm_bondlength_atom_pair, mm_bondlength_param_set > mm_bondlength_library;
typedef std::multimap< mm_bondlength_atom_pair, mm_bondlength_param_set >::const_iterator mm_bondlength_library_citer;
typedef std::pair< mm_bondlength_library_citer, mm_bondlength_library_citer > mm_bondlength_library_citer_pair;

class MMBondLengthLibrary  : public utility::VirtualBase
{

public:
	/// @brief Automatically generated virtual destructor for class deriving directly from VirtualBase
	~MMBondLengthLibrary() override;
	/// @brief ctor
	MMBondLengthLibrary( std::string filename, core::chemical::MMAtomTypeSetCOP mm_atom_set,  utility::vector1< std::string > extra_mm_param_dirs = utility::vector1< std::string > ());

	/// @brief lookup by atom type ints
	mm_bondlength_library_citer_pair
	lookup( int atom1, int atom2 ) const;

	/// @brief lookup by atom type strings
	mm_bondlength_library_citer_pair
	lookup( std::string atom1, std::string atom2 ) const;

	/// @brief pretty print / debug
	void pretty_print() const;
	void pretty_print( int atom1, int atom2 ) const;
	void pretty_print(  std::string atom1, std::string atom2 ) const;

private:

	/// @brief library containing all bond-length params
	mm_bondlength_library mm_bondlength_library_;

	core::chemical::MMAtomTypeSetCAP mm_atom_set_;

};


} // namespace mm
} // namespace scoring
} // namespace core


#endif // INCLUDED_core_mm_mm_bondangle_library_HH
