// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/energy_methods/ReferenceEnergy.hh
/// @brief  Reference energy method declaration
/// @author Andrew Leaver-Fay (aleaverfay@gmail.com)


#ifndef INCLUDED_core_energy_methods_ReferenceEnergy_hh
#define INCLUDED_core_energy_methods_ReferenceEnergy_hh

// Unit headers
#include <core/energy_methods/ReferenceEnergy.fwd.hh>

// Package headers
#include <core/scoring/methods/ContextIndependentOneBodyEnergy.hh>
#include <core/scoring/ScoreFunction.fwd.hh>

// Project headers
#include <core/chemical/AA.hh>
#include <core/pose/Pose.fwd.hh>
#include <core/id/TorsionID.fwd.hh>
#include <core/id/DOF_ID.fwd.hh>

#include <utility/vector1.hh>


// Utility headers


namespace core {
namespace energy_methods {


class ReferenceEnergy : public core::scoring::methods::ContextIndependentOneBodyEnergy
{
public:
	typedef core::scoring::methods::ContextIndependentOneBodyEnergy parent;

public:


	ReferenceEnergy( Real ordered_wat_penalty );


	ReferenceEnergy( utility::vector1< Real > const & aa_weights_in, Real ordered_wat_penalty );


	~ReferenceEnergy() override;

	core::scoring::methods::EnergyMethodOP
	clone() const override;

	bool
	is_d_aminoacid(
		core::chemical::AA const res_aa
	) const;

	core::chemical::AA
	get_l_equivalent(
		core::chemical::AA const d_aa
	) const;

	/////////////////////////////////////////////////////////////////////////////
	// methods for ContextIndependentOneBodyEnergies
	/////////////////////////////////////////////////////////////////////////////


	void
	residue_energy(
		conformation::Residue const & rsd,
		pose::Pose const & pose,
		core::scoring::EnergyMap & emap
	) const override;

	bool
	minimize_in_whole_structure_context( pose::Pose const & ) const override { return false; }


	virtual
	Real
	eval_dof_derivative(
		id::DOF_ID const & dof_id,
		id::TorsionID const & tor_id,
		pose::Pose const & pose,
		core::scoring::ScoreFunction const & sfxn,
		core::scoring::EnergyMap const & weights
	) const;

	/// @brief DunbrackEnergy is context independent; indicates that no
	/// context graphs are required
	void indicate_required_context_graphs( utility::vector1< bool > & ) const override;


private:
	utility::vector1< Real > aa_weights_;
	Real ordered_wat_penalty_;
	core::Size version() const override;

};

} // scoring
} // core


#endif
