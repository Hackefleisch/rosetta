// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/scoring/methods/RNA_SuiteEnergy.cc
/// @brief  RNA Suite Energy
/// @author Fang-Chieh Chou

// Unit Headers
#include <core/energy_methods/RNA_SuiteEnergy.hh>
#include <core/energy_methods/RNA_SuiteEnergyCreator.hh>
#include <core/scoring/rna/RNA_EnergyMethodOptions.hh>

// Package Headers
#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/EnergyMap.hh>
#include <core/scoring/PolymerBondedEnergyContainer.hh>
#include <core/scoring/Energies.hh>
#include <core/scoring/DerivVectorPair.hh>
#include <core/scoring/methods/EnergyMethodOptions.hh>
#include <core/scoring/ScoringManager.hh>

// Project Headers
#include <core/conformation/Conformation.hh>
#include <core/conformation/Residue.hh>
#include <core/pose/Pose.hh>
#include <core/id/AtomID.hh>
#include <core/id/TorsionID.hh>
#include <basic/Tracer.hh>

// Utility headers
#include <utility/vector1.hh>
#include <utility/fixedsizearray1.hh>

//Numeric Headers
#include <numeric/deriv/dihedral_deriv.hh>

#include <core/scoring/rna/RNA_SuitePotential.hh> // AUTO IWYU For RNA_SuitePotential

static basic::Tracer TR( "core.energy_methods.RNA_SuiteEnergy" );


///////////////////////////////////////////////////////////////////////////////////
// Note:
//
// * The good stuff is in RNA_SuitePotential. Note that this potential is zero
//   when torsions are 'on-rotamer' and then jump to high values 'off-rotamer'
//   Values of on-rotamer bonuses can be set from files in suite_torsion directory/.
//
// * Does not currently include nu1,nu2,chi,2'-OH torsions -- just 'backbone',
//    so still should combine this with rna_torsion_sc score term.
//
// * Also -- may be double counting of each sugar delta contribution? Talk to
//    Fang about fix [ would need to subtract out log P(delta) ].
//
// * Recently revived Richardson-style suiteness computation to allow bonuses
//    in those rotamers with relatively flat basins 'on-rotamer'.
//
//    -- rhiju, 2014
//
///////////////////////////////////////////////////////////////////////////////////

namespace core {
namespace energy_methods {

using namespace core::chemical;

/// @details This must return a fresh instance,
/// never an instance already in use
core::scoring::methods::EnergyMethodOP
RNA_SuiteEnergyCreator::create_energy_method(
	core::scoring::methods::EnergyMethodOptions const & options
) const { return utility::pointer::make_shared< RNA_SuiteEnergy >( options.rna_options() ); }

core::scoring::ScoreTypes
RNA_SuiteEnergyCreator::score_types_for_method() const {
	using namespace core::scoring;
	ScoreTypes sts;
	sts.push_back( rna_suite );
	sts.push_back( suiteness_bonus );
	return sts;
}

/// ctor
RNA_SuiteEnergy::RNA_SuiteEnergy( core::scoring::rna::RNA_EnergyMethodOptions const & options ) :
	parent( utility::pointer::make_shared< RNA_SuiteEnergyCreator >() ),
	options_( options ),
	rna_suite_potential_( core::scoring::ScoringManager::get_instance()->get_rna_suite_potential( false, "" ) ), // utility::pointer::make_shared< core::scoring::rna::RNA_SuitePotential >( false ) ),
	rna_suite_potential_for_suiteness_bonus_(  core::scoring::ScoringManager::get_instance()->get_rna_suite_potential(true, options.suiteness_bonus() ) ) //utility::pointer::make_shared< core::scoring::rna::RNA_SuitePotential >( true, options.suiteness_bonus() ) )
{}
///////////////////////////////////////////////////////////////////////////////

// The following codes are copied from scoring/methods/Rama2BOffsetEnergy.cc
// I think this is to set up scoring cache in pose objects.
void
RNA_SuiteEnergy::setup_for_scoring(
	pose::Pose & pose,
	core::scoring::ScoreFunction const &
) const {
	using namespace core::scoring;
	using namespace core::scoring::methods;

	// create LR energy container
	core::scoring::methods::LongRangeEnergyType const & lr_type( long_range_type() );
	core::scoring::Energies & energies( pose.energies() );
	bool create_new_lre_container( false );

	if ( energies.long_range_container( lr_type ) == nullptr ) {
		create_new_lre_container = true;
	} else {
		core::scoring::LREnergyContainerOP lrc = energies.nonconst_long_range_container( lr_type );
		core::scoring::PolymerBondedEnergyContainerOP dec( utility::pointer::static_pointer_cast< core::scoring::PolymerBondedEnergyContainer > ( lrc ) );
		if ( !dec || !dec->is_valid( pose ) ) {
			create_new_lre_container = true;
		}
	}

	if ( create_new_lre_container ) {
		utility::vector1< ScoreType > s_types;
		s_types.push_back( rna_suite );
		s_types.push_back( suiteness_bonus );
		core::scoring::LREnergyContainerOP new_dec( utility::pointer::make_shared< core::scoring::PolymerBondedEnergyContainer >( pose, s_types ) );
		energies.set_long_range_container( lr_type, new_dec );
	}
}

void
RNA_SuiteEnergy::residue_pair_energy(
	conformation::Residue const & rsd1,
	conformation::Residue const & rsd2,
	pose::Pose const & pose,
	core::scoring::ScoreFunction const & scorefxn,
	core::scoring::EnergyMap & emap
) const {
	if ( rsd1.has_variant_type( REPLONLY ) ) return;
	if ( rsd2.has_variant_type( REPLONLY ) ) return;

	utility::fixedsizearray1< id::TorsionID, 7 > torsion_ids;
	Real score;
	utility::fixedsizearray1< Real, 7 > deriv;
	if ( scorefxn.has_nonzero_weight( core::scoring::rna_suite ) && rna_suite_potential_->eval_score( rsd1, rsd2, pose, torsion_ids, score, deriv ) ) {
		emap[ core::scoring::rna_suite ]       += score;
	}
	if ( scorefxn.has_nonzero_weight( core::scoring::suiteness_bonus ) && rna_suite_potential_for_suiteness_bonus_->eval_score( rsd1, rsd2, pose, torsion_ids, score, deriv ) ) {
		emap[ core::scoring::suiteness_bonus ] += score;
	}
	debug_assert( emap.is_finite() );
}

void
RNA_SuiteEnergy::eval_residue_pair_derivatives(
	conformation::Residue const & rsd1,
	conformation::Residue const & rsd2,
	core::scoring::ResSingleMinimizationData const &,
	core::scoring::ResSingleMinimizationData const &,
	core::scoring::ResPairMinimizationData const &,
	pose::Pose const & pose,
	core::scoring::EnergyMap const & weights,
	utility::vector1< core::scoring::DerivVectorPair> & r1_atom_derivs,
	utility::vector1< core::scoring::DerivVectorPair> & r2_atom_derivs
) const {
	using namespace core::scoring;
	if ( rsd1.has_variant_type( REPLONLY ) ) return;
	if ( rsd2.has_variant_type( REPLONLY ) ) return;

	eval_residue_pair_derivatives( rsd1, rsd2, pose, weights[rna_suite], r1_atom_derivs, r2_atom_derivs, rna_suite_potential_ );
	eval_residue_pair_derivatives( rsd1, rsd2, pose, weights[suiteness_bonus], r1_atom_derivs, r2_atom_derivs, rna_suite_potential_for_suiteness_bonus_ );

}

void
RNA_SuiteEnergy::eval_residue_pair_derivatives(
	conformation::Residue const & rsd1,
	conformation::Residue const & rsd2,
	pose::Pose const & pose,
	Real const & weight,
	utility::vector1< core::scoring::DerivVectorPair> & r1_atom_derivs,
	utility::vector1< core::scoring::DerivVectorPair> & r2_atom_derivs,
	core::scoring::rna::RNA_SuitePotentialCOP rna_suite_potential
) const {
	if ( rsd1.has_variant_type( REPLONLY ) ) return;
	if ( rsd2.has_variant_type( REPLONLY ) ) return;

	using namespace numeric::conversions;
	using namespace core::chemical::rna;
	using namespace core::pose::rna;
	using namespace core::id;

	if ( weight == 0.0 ) return;
	utility::fixedsizearray1<TorsionID, 7> torsion_ids;
	Real score;
	utility::fixedsizearray1< Real, 7 > deriv;
	if ( !rna_suite_potential->eval_score( rsd1, rsd2, pose, torsion_ids, score, deriv ) ) return;

	//utility::fixedsizearray1<Real,7> const & deriv( rna_suite_potential->get_deriv() );
	//utility::vector1<TorsionID> const & torsion_ids(
	// rna_suite_potential->get_torsion_ids() );

	conformation::Residue const & rsd_lo(
		( rsd1.seqpos() < rsd2.seqpos() ) ? rsd1 : rsd2 );
	conformation::Residue const & rsd_hi(
		( rsd1.seqpos() < rsd2.seqpos() ) ? rsd2 : rsd1 );
	utility::vector1< core::scoring::DerivVectorPair > & r_lo_derivs(
		( rsd1.seqpos() < rsd2.seqpos() ) ? r1_atom_derivs : r2_atom_derivs );
	utility::vector1< core::scoring::DerivVectorPair > & r_hi_derivs(
		( rsd1.seqpos() < rsd2.seqpos() ) ? r2_atom_derivs : r1_atom_derivs );
	Size const rsdnum_lo( rsd_lo.seqpos() ), rsdnum_hi( rsd_hi.seqpos() );

	for ( Size i = 1; i <= torsion_ids.size(); ++i ) {
		utility::vector1< id::AtomID > atom_ids;
		utility::vector1< Vector > f1s;
		utility::vector1< Vector > f2s;
		Real const deriv_i = degrees( deriv[i] );
		if ( !get_f1_f2( torsion_ids[i], pose, atom_ids, f1s, f2s ) ) continue;
		for ( Size j = 1; j <= atom_ids.size(); ++j ) {
			if ( atom_ids[j].rsd() == rsdnum_lo ) {
				// Not sure why the "degrees" is needed. But in practice it works
				// correctly.
				r_lo_derivs[ atom_ids[j].atomno() ].f1() +=
					deriv_i * f1s[j] * weight;
				r_lo_derivs[ atom_ids[j].atomno() ].f2() +=
					deriv_i * f2s[j] * weight;
			} else if ( atom_ids[j].rsd() == rsdnum_hi ) {
				r_hi_derivs[ atom_ids[j].atomno() ].f1() +=
					deriv_i * f1s[j] * weight;
				r_hi_derivs[ atom_ids[j].atomno() ].f2() +=
					deriv_i * f2s[j] * weight;
			} else {
				// Should not happen. Exit here just in case.
				utility_exit_with_message("Invalid Torsion!!");
			}
		}
	}
}

// Mostly copied from scoring/rna/RNA_TorsionPotential.cc
bool
RNA_SuiteEnergy::get_f1_f2(
	id::TorsionID const & torsion_id,
	pose::Pose const & pose,
	utility::vector1< id::AtomID > & atom_ids,
	utility::vector1< Vector > & f1s,
	utility::vector1< Vector > & f2s
) const {
	atom_ids.clear();
	f1s.clear();
	f2s.clear();

	conformation::Conformation const & conf( pose.conformation() );

	id::AtomID id1, id2, id3, id4;
	if ( conf.get_torsion_angle_atom_ids( torsion_id, id1, id2, id3, id4 ) ) return false;
	atom_ids.push_back( id1 );
	atom_ids.push_back( id2 );
	atom_ids.push_back( id3 );
	atom_ids.push_back( id4 );

	Vector const &
		x1( conf.xyz( id1 ) ), x2( conf.xyz( id2 ) ),
		x3( conf.xyz( id3 ) ), x4( conf.xyz( id4 ) );

	Real theta;
	Vector f1, f2;
	numeric::deriv::dihedral_p1_cosine_deriv( x1, x2, x3, x4, theta, f1, f2 );
	f1s.push_back( f1 );
	f2s.push_back( f2 );

	numeric::deriv::dihedral_p2_cosine_deriv( x1, x2, x3, x4, theta, f1, f2 );
	f1s.push_back( f1 );
	f2s.push_back( f2 );

	numeric::deriv::dihedral_p2_cosine_deriv( x4, x3, x2, x1, theta, f1, f2 );
	f1s.push_back( f1 );
	f2s.push_back( f2 );

	numeric::deriv::dihedral_p1_cosine_deriv( x4, x3, x2, x1, theta, f1, f2 );
	f1s.push_back( f1 );
	f2s.push_back( f2 );

	return true;
}

} //scoring
} //core

