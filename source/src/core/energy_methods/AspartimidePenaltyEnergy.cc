// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/energy_methods/AspartimidePenaltyEnergy.hh
/// @brief  This is a score term that penalizes sequences that are likely to result in aspartimide formation during peptide synthesis.
/// @details This is intended for peptide design applications only.  Sequences penalized are LASP-D*, LASP-LSER, LASP-LTHR, LASP-LGLN,
/// and the mirror-image equivalents (DASP-L*, DASP-DSER, DASP-DTHR, DASP-DGLN), plus D/LASP-GLY.
/// @author Vikram K. Mulligan (vmullig@uw.edu)

// Unit headers
#include <core/energy_methods/AspartimidePenaltyEnergy.hh>
#include <core/energy_methods/AspartimidePenaltyEnergyCreator.hh>

// Package headers
#include <core/scoring/EnergyMap.hh>
#include <core/scoring/methods/EnergyMethodOptions.hh>

// Project headers
#include <core/conformation/Residue.hh>
#include <core/chemical/AA.hh>
#include <core/id/DOF_ID.fwd.hh>
#include <core/id/TorsionID.hh>


#include <core/pose/Pose.fwd.hh>
#include <basic/Tracer.hh>

// Utility headers

// Numeric headers

// C++ headers

#include <core/scoring/methods/util.hh> // AUTO IWYU For determine_lo_and_hi_residues, creat...

namespace core {
namespace energy_methods {



static basic::Tracer TR( "core.energy_methods.AspartimidePenaltyEnergy" );

/// @brief EnergyMethod creator, called by the machinery that sets up the scorefunction.
///
core::scoring::methods::EnergyMethodOP
AspartimidePenaltyEnergyCreator::create_energy_method(
	core::scoring::methods::EnergyMethodOptions const & options
) const {
	return utility::pointer::make_shared< AspartimidePenaltyEnergy >( options.aspartimide_penalty_value() );
}

core::scoring::ScoreTypes
AspartimidePenaltyEnergyCreator::score_types_for_method() const {
	using namespace core::scoring;
	ScoreTypes sts;
	sts.push_back( aspartimide_penalty );
	return sts;
}

/// @brief Constructor.
///
AspartimidePenaltyEnergy::AspartimidePenaltyEnergy( ) :
	parent( utility::pointer::make_shared< AspartimidePenaltyEnergyCreator >() ),
	aspartimide_penalty_value_( 25.0 )
{}

/// @brief Constructor that sets penalty value.
/// @details The penalty value is the energetic hit for each aspartimide in the
/// sequence (which will be multiplied by the score function's weight, of course).
AspartimidePenaltyEnergy::AspartimidePenaltyEnergy( core::Real const &penalty_value ) :
	parent( utility::pointer::make_shared< AspartimidePenaltyEnergyCreator >() ),
	aspartimide_penalty_value_( penalty_value )
{}

/// @brief Destructor.
///
AspartimidePenaltyEnergy::~AspartimidePenaltyEnergy( ) = default;

/// @brief Copy this energy object and return an owning pointer to the copy.
///
core::scoring::methods::EnergyMethodOP
AspartimidePenaltyEnergy::clone() const {
	return utility::pointer::make_shared< AspartimidePenaltyEnergy >( *this );
}

/// @brief Method called before scoring a pose.
///
void
AspartimidePenaltyEnergy::setup_for_scoring(
	pose::Pose & pose,
	core::scoring::ScoreFunction const &/*sfxn*/
) const {
	create_long_range_energy_container( pose, core::scoring::aspartimide_penalty, long_range_type() );
}

/// @brief Are the two residues (rsd1, rsd2) two residues that should be scored by this scorefunction?
/// @details Returns true only if rsd2 is connected to the C-terminus of rsd1 by its N-terminal connection, or
/// vice versa.
bool
AspartimidePenaltyEnergy::defines_residue_pair_energy(
	pose::Pose const &pose,
	Size rsd1,
	Size rsd2
) const {
	bool res1_is_lo(false), res2_is_lo(false);
	core::scoring::methods::determine_lo_and_hi_residues( pose, rsd1, rsd2, res1_is_lo, res2_is_lo );
	runtime_assert_string_msg( !(res1_is_lo && res2_is_lo ), "Error in core::scoring::methods::AspartimidePenaltyEnergy::defines_residue_pair_energy(): The aspartimide_penalty energy term is incompatible with cyclic dipeptides (as is most of the rest of Rosetta)." );

	return (res1_is_lo || res2_is_lo);
}

core::scoring::methods::LongRangeEnergyType
AspartimidePenaltyEnergy::long_range_type() const {
	return core::scoring::methods::aspartimide_penalty_lr;
}

void
AspartimidePenaltyEnergy::residue_pair_energy(
	conformation::Residue const & rsd1,
	conformation::Residue const & rsd2,
	pose::Pose const & pose,
	core::scoring::ScoreFunction const &,
	core::scoring::EnergyMap & emap
) const {
	using namespace core::chemical;

	bool res1_is_lo(false), res2_is_lo(false);

	core::scoring::methods::determine_lo_and_hi_residues( pose, rsd1.seqpos(), rsd2.seqpos(), res1_is_lo, res2_is_lo );
	runtime_assert_string_msg( !(res1_is_lo && res2_is_lo ), "Error in core::scoring::methods::AspartimidePenaltyEnergy::residue_pair_energy(): The aspartimide_penalty energy term is incompatible with cyclic dipeptides (as is most of the rest of Rosetta)." );

	conformation::Residue const &res_lo = (res1_is_lo) ? rsd1 : rsd2;
	conformation::Residue const &res_hi = (res2_is_lo) ? rsd1 : rsd2;

	core::chemical::AA const aa_low( res_lo.type().aa() );
	core::chemical::AA const aa_hi( res_hi.type().aa() );

	if ( !first_res_types( aa_low ) ) return; //Do nothing if the first residue isn't D/L asp.
	if ( !second_res_types( aa_hi ) ) {
		if ( res_lo.type().is_l_aa() == res_hi.type().is_l_aa() || res_lo.type().is_d_aa() == res_hi.type().is_d_aa() ) return; //Do nothing if the second residue isn't ser/thr/gly/asn and both residues have the same chirality.
	}

	//Cases in which to do nothing: if the second residue isn't one of the residue types that we're penalizing, then return:
	if ( aa_low == aa_asp ) {
		if ( !(aa_hi == aa_asn || aa_hi == aa_gly || aa_hi == aa_ser || aa_hi == aa_thr || res_hi.type().is_d_aa()) ) return;
	} else if ( aa_low == aa_das ) {
		if ( !(aa_hi == aa_dan || aa_hi == aa_gly || aa_hi == aa_dse || aa_hi == aa_dth || res_hi.type().is_l_aa()) ) return;
	}

	//We've ruled out all of the cases in which we do nothing.  Now, since this IS a pair to be penalized, add the penalty value:
	emap[ core::scoring::aspartimide_penalty ] += aspartimide_penalty_value_;
}

/// @brief Does nothing, since this term is spatially invariant (i.e. has no DoF derivatives, because
/// the value depends only on residue identities).
Real
AspartimidePenaltyEnergy::eval_intraresidue_dof_derivative(
	conformation::Residue const & /*res_lo*/,
	core::scoring::ResSingleMinimizationData const & /*min_data*/,
	id::DOF_ID const & /*dof_id*/,
	id::TorsionID const & /*tor_id*/,
	pose::Pose const & /*pose*/,
	core::scoring::ScoreFunction const & /*sfxn*/,
	core::scoring::EnergyMap const & /*weights*/
) const
{
	return 0.0;
}

/// @brief Is the given AA for the first residue one of the possible types that this score term scores?
/// @details Returns true for aa_asp/aa_das, false otherwise.
bool
AspartimidePenaltyEnergy::first_res_types(
	core::chemical::AA const aa
) const {
	using namespace core::chemical;
	return ( aa == aa_asp || aa == aa_das );
}

/// @brief Is the given AA for the second residue one of the possible types that this score term scores?
/// @details Returns true for aa_gly/aa_asn/aa_ser/aa_thr/aa_dan/aa_dse/aa_dth, false otherwise.
bool
AspartimidePenaltyEnergy::second_res_types(
	core::chemical::AA const aa
) const {
	using namespace core::chemical;
	return (aa == aa_gly || aa == aa_asn || aa == aa_dan || aa == aa_ser || aa == aa_dse || aa == aa_thr || aa == aa_dth);
}


core::Size
AspartimidePenaltyEnergy::version() const {
	return 1; // Initial versioning
}

} // namespace energy_methods
} // namespace core
