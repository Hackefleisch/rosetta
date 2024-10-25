// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/scoring/methods/RNA_JR_SuiteEnergy.hh
/// @brief  Jane Richardson style RNA Suite Energy
/// @author Fang-Chieh Chou

#ifndef INCLUDED_core_scoring_rna_RNA_JR_SuiteEnergy_hh
#define INCLUDED_core_scoring_rna_RNA_JR_SuiteEnergy_hh

// Unit Headers
#include <core/energy_methods/RNA_JR_SuiteEnergy.fwd.hh>

// Package headers
#include <core/scoring/methods/ContextIndependentTwoBodyEnergy.hh>
#include <core/scoring/ScoreFunction.fwd.hh>

// Project headers
#include <core/pose/Pose.fwd.hh>
#include <core/pose/rna/RNA_SuiteName.hh>

// Utility headers
#include <utility/vector1.hh>

namespace core {
namespace energy_methods {


class RNA_JR_SuiteEnergy : public core::scoring::methods::ContextIndependentTwoBodyEnergy  {
public:
	typedef core::scoring::methods::ContextIndependentTwoBodyEnergy parent;

	RNA_JR_SuiteEnergy();

	/// clone
	core::scoring::methods::EnergyMethodOP clone() const override;

	/////////////////////////////////////////////////////////////////////////////
	// scoring
	/////////////////////////////////////////////////////////////////////////////

	/// @brief Evaluate the intra-residue constraint energy for a given residue
	void
	eval_intrares_energy(
		conformation::Residue const &,
		pose::Pose const &,
		core::scoring::ScoreFunction const &,
		core::scoring::EnergyMap &
	) const override {};


	void
	residue_pair_energy(
		conformation::Residue const & rsd1,
		conformation::Residue const & rsd2,
		pose::Pose const & pose,
		core::scoring::ScoreFunction const &,
		core::scoring::EnergyMap & emap
	) const override;

	/// called during gradient-based minimization inside dfunc
	/**
	F1 and F2 are not zeroed -- contributions from this atom are
	just summed in
	**/
	void
	eval_atom_derivative(
		id::AtomID const &,
		pose::Pose const &,
		kinematics::DomainMap const &,
		core::scoring::ScoreFunction const &,
		core::scoring::EnergyMap const &,
		Vector &,
		Vector &
	) const override {};

	bool defines_intrares_energy( core::scoring::EnergyMap const & ) const override { return true; }

	bool
	defines_residue_pair_energy( core::scoring::EnergyMap const & ) const { return true; }

	Distance atomic_interaction_cutoff() const override { return 0; }

	void
	indicate_required_context_graphs( utility::vector1< bool > & ) const override {};
	/////////////////////////////////////////////////////////////////////////////
	// data
	/////////////////////////////////////////////////////////////////////////////

private:
	Size version() const override { return 0; }

	core::pose::rna::RNA_SuiteName suitename_;
};

} //scoring
} //core

#endif // INCLUDED_core_energy_methods_RNA_LJ_BaseEnergy_HH
