// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/energy_methods/UnfoldedStateEnergy.cc
/// @brief  Unfolded state energy method implementation; energies based on eneriges of residues in fragments
/// @author Ron Jacak (ronj@email.unc.edu)

// Unit headers
#include <core/energy_methods/UnfoldedStateEnergy.hh>
#include <core/energy_methods/UnfoldedStateEnergyCreator.hh>

// Package headers
#include <core/scoring/methods/EnergyMethodOptions.hh>
#include <core/scoring/EnergyMap.hh>
#include <core/scoring/UnfoldedStatePotential.hh>
#include <core/scoring/ScoringManager.hh>
#include <basic/Tracer.hh>

// Project headers
#include <core/conformation/Residue.hh>

#include <utility/vector1.hh>


static basic::Tracer TR( "core.energy_methods.UnfoldedStateEnergy" );

namespace core {
namespace energy_methods {



/// @details This must return a fresh instance of the UnfoldedStateEnergy class, never an instance already in use
core::scoring::methods::EnergyMethodOP
UnfoldedStateEnergyCreator::create_energy_method( core::scoring::methods::EnergyMethodOptions const & options ) const {

	if ( !options.has_method_weights( core::scoring::unfolded ) ) {
		return utility::pointer::make_shared< UnfoldedStateEnergy >( options.unfolded_energies_type() );
	}

	utility::vector1< Real > const & v = options.method_weights( core::scoring::unfolded );
	debug_assert( v.size() == scoring::n_score_types );

	// convert the vector of Reals into an core::scoring::EnergyMap, because that's what the constructor for USE takes.
	// assumes that the vector of Reals coming in contains the weights for each of the score types in the
	// scoring namespace enumeration, and more importantly, in the same order.
	core::scoring::EnergyMap e;
	for ( Size ii=1; ii < scoring::n_score_types; ++ii ) {
		e[ ( core::scoring::ScoreType) ii ] = v[ii];
	}

	return utility::pointer::make_shared< UnfoldedStateEnergy >( options.unfolded_energies_type(), e );
}

core::scoring::ScoreTypes
UnfoldedStateEnergyCreator::score_types_for_method() const {
	using namespace core::scoring;
	ScoreTypes sts;
	sts.push_back( unfolded );
	sts.push_back( fa_intra_atr_ref );
	sts.push_back( fa_intra_rep_ref );
	sts.push_back( fa_intra_sol_ref );
	sts.push_back( pro_close_ref );
	sts.push_back( fa_dun_ref );
	sts.push_back( fa_dun_dev_ref );
	sts.push_back( fa_dun_rot_ref );
	sts.push_back( fa_dun_semi_ref );
	sts.push_back( rama_ref );
	sts.push_back( p_aa_pp_ref );
	sts.push_back( omega_ref );
	sts.push_back( mm_lj_intra_rep_ref );
	sts.push_back( mm_lj_intra_atr_ref );
	sts.push_back( mm_twist_ref );
	return sts;
}


/// UnfoldedStateEnergy class methods


UnfoldedStateEnergy::UnfoldedStateEnergy( std::string const & type ) :
	parent( utility::pointer::make_shared< UnfoldedStateEnergyCreator >() ),
	type_( type ),
	unf_state_potential_( core::scoring::ScoringManager::get_instance()->get_UnfoldedStatePotential( type ) ),
	score_type_weights_( unf_state_potential_.get_unfoled_potential_file_weights() )
{
	TR.Debug << "instantiating class with weights: " << score_type_weights_.show_nonzero() << std::endl;
}

UnfoldedStateEnergy::UnfoldedStateEnergy( std::string const & type, const core::scoring::EnergyMap & emap_in ):
	parent( utility::pointer::make_shared< UnfoldedStateEnergyCreator >() ),
	type_( type ),
	unf_state_potential_( core::scoring::ScoringManager::get_instance()->get_UnfoldedStatePotential( type ) ),
	score_type_weights_( emap_in )
{
	TR.Debug << "instantiating class with weights: " << score_type_weights_.show_nonzero()  << std::endl;
}

UnfoldedStateEnergy::~UnfoldedStateEnergy() = default;

core::scoring::methods::EnergyMethodOP
UnfoldedStateEnergy::clone() const {
	return utility::pointer::make_shared< UnfoldedStateEnergy >( type_, score_type_weights_ );
}

void
UnfoldedStateEnergy::residue_energy(
	conformation::Residue const & rsd,
	pose::Pose const &,
	core::scoring::EnergyMap & emap
) const
{
	// the value this function returns depends on how this class was constructed. if a set of weights was
	// passed in when the class was constructed, then this function will return a non-zero value.
	// if no weights were passed in, the member variable energy map will be just zeros and the function
	// will return 0.0. that's what we want during the first phase of optE. once a weight set has been
	// established, this class will get recreated with that set of weights and the function will return
	// meaningful energies.

	using namespace core::scoring;

	core::scoring::EnergyMap unweighted_unfolded_energies;
	unf_state_potential_.raw_unfolded_state_energymap( rsd.type().name3(), unweighted_unfolded_energies );

	// don't forget to include the weight for the entire term. no, wait, that weight should get applied later
	// in the scoring process. this method should just return the unweighted unfolded state energy.
	emap[ core::scoring::unfolded ] += unweighted_unfolded_energies.dot( score_type_weights_ );

	//Add split out unfolded/reference energies. Basically each component that would go into the combined unfolded term is given its own energy term(referred to with the "_ref" suffix).
	//Each of these terms has its own weight defined in the .wts file, which needs to be selected with optE.
	emap[ core::scoring::fa_intra_atr_ref ] += unweighted_unfolded_energies[ fa_intra_atr ];
	emap[ core::scoring::fa_intra_rep_ref ] += unweighted_unfolded_energies[ fa_intra_rep ];
	emap[ core::scoring::fa_intra_sol_ref ] += unweighted_unfolded_energies[ fa_intra_sol ];
	emap[ core::scoring::pro_close_ref ] += unweighted_unfolded_energies[ pro_close ];
	emap[ core::scoring::rama_ref ] += unweighted_unfolded_energies[ rama ];
	emap[ core::scoring::p_aa_pp_ref ] += unweighted_unfolded_energies[ p_aa_pp ];
	emap[ core::scoring::omega_ref ] += unweighted_unfolded_energies[ omega ];

	//Should these ones be combined into one term? They normally all get combined and have the same weight, but theoretically they might want to vary independently during optE? Easy to change if needed.
	emap[ core::scoring::fa_dun_ref ] += unweighted_unfolded_energies[ fa_dun ];
	emap[ core::scoring::fa_dun_dev_ref ] += unweighted_unfolded_energies[ fa_dun_dev ];
	emap[ core::scoring::fa_dun_rot_ref ] += unweighted_unfolded_energies[ fa_dun_rot ];
	emap[ core::scoring::fa_dun_semi_ref ] += unweighted_unfolded_energies[ fa_dun_semi ];

	//mm_std specific terms.
	emap[ core::scoring::mm_lj_intra_rep_ref ] += unweighted_unfolded_energies[ mm_lj_intra_rep ];
	emap[ core::scoring::mm_lj_intra_atr_ref ] += unweighted_unfolded_energies[ mm_lj_intra_atr ];
	emap[ core::scoring::mm_twist_ref ] += unweighted_unfolded_energies[ mm_twist ];

	//Leaving out two body terms since for the split unfolded energy they are covered in SplitUnfoldedTwoBodyEnergy, and I'm not sure if the standard unfolded energy should be optimized with separated energies. It's simple enough to add them later if need be.
	return;
}

void
UnfoldedStateEnergy::indicate_required_context_graphs( utility::vector1< bool > & ) const {}

core::Size
UnfoldedStateEnergy::version() const
{
	return 1; // Initial versioning
}


} // scoring
} // core
