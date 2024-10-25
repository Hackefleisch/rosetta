// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/scoring/symmetry/SymmetricEnergies.hh
/// @brief  Symmetric Energies class to store cached energies and track the residue
/// neighbor relationships
/// @author Ingemar Andre

#ifndef INCLUDED_core_scoring_symmetry_SymmetricEnergies_hh
#define INCLUDED_core_scoring_symmetry_SymmetricEnergies_hh

// Unit headers
#include <core/scoring/symmetry/SymmetricEnergies.fwd.hh>

// Package headers
#include <core/scoring/Energies.hh>
#include <core/conformation/symmetry/SymmetryInfo.fwd.hh>
#include <core/conformation/symmetry/SymmetricConformation.fwd.hh>

// Project headers
#include <core/conformation/PointGraph.fwd.hh>
#include <core/scoring/ContextGraphTypes.hh>

// Numeric headers

#include <numeric/xyzMatrix.fwd.hh>

#ifdef    SERIALIZATION
// Cereal headers
#include <cereal/types/polymorphic.fwd.hpp>
#endif // SERIALIZATION


namespace core {
namespace scoring {
namespace symmetry {

class SymmetricEnergies : public Energies
{

public:

	typedef conformation::symmetry::SymmetricConformation SymmetricConformation;
	typedef conformation::symmetry::SymmetryInfoCOP SymmetryInfoCOP;

	typedef numeric::xyzMatrix< Real > Matrix;

public:
	/// ctor -- ensure correct initial state
	SymmetricEnergies();

	// Explicit copy ctor to avoid #include .hh's
	SymmetricEnergies( Energies const & src );

	SymmetricEnergies( Energies & src );

	// Explicit assignmnet operator to avoid #include .hh's
	Energies & operator = ( Energies const & rhs ) override;

	/// @details determine whether my type is the same as another Conformation's
	bool
	same_type_as_me( Energies const & other, bool recurse = true ) const override;

	~SymmetricEnergies() override;

	EnergiesOP
	clone() const override;

	void set_derivative_graph( MinimizationGraphOP dg );
	MinimizationGraphOP derivative_graph();
	MinimizationGraphCOP derivative_graph() const;

private:

	void update_neighbor_links( pose::Pose const & pose ) override;
	void fill_point_graph( pose::Pose const & pose, conformation::PointGraphOP pg ) const override;
	void require_context_graph_( scoring::ContextGraphType type, bool external ) const override;

private:
	MinimizationGraphOP derivative_graph_;

#ifdef    SERIALIZATION
public:
	template< class Archive > void save( Archive & arc ) const;
	template< class Archive > void load( Archive & arc );
#endif // SERIALIZATION

};

} // namespace symmetry
} // namespace scoring
} // namespace core

#ifdef    SERIALIZATION
CEREAL_FORCE_DYNAMIC_INIT( core_scoring_symmetry_SymmetricEnergies )
#endif // SERIALIZATION

#endif // INCLUDED_core_scoring_symmetry_SymmetricEnergies_HH
