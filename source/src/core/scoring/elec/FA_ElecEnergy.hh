// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/scoring/methods/FA_ElecEnergy.hh
/// @brief  Electrostatic energy with a distance-dependant dielectric
/// @author Phil Bradley
/// @author Modifed by James Gleixner
/// @author Modified by Vikram K. Mulligan (vmullig@uw.edu) -- added data caching.

#ifndef INCLUDED_core_scoring_elec_FA_ElecEnergy_hh
#define INCLUDED_core_scoring_elec_FA_ElecEnergy_hh

// Unit Headers
#include <core/scoring/elec/FA_ElecEnergy.fwd.hh>

// Package headers

// Project headers
#include <core/chemical/ResidueType.fwd.hh>
#include <core/pose/Pose.fwd.hh>
#include <core/scoring/methods/ContextIndependentTwoBodyEnergy.hh>
#include <core/scoring/methods/EnergyMethodOptions.fwd.hh>
#include <core/scoring/trie/RotamerTrieBase.fwd.hh>
#include <core/scoring/etable/count_pair/CountPairFunction.fwd.hh>
#include <core/scoring/etable/coulomb/Coulomb.hh>
#include <core/scoring/trie/TrieCountPairBase.fwd.hh>
#include <core/scoring/elec/CPRepMapType.fwd.hh>
#include <core/chemical/RestypeDestructionEvent.fwd.hh>
#include <core/chemical/AtomType.hh> // for MAX_CHEMICAL_BOND_TO_HYDROGEN_LENGTH

// Utility headers
#include <utility/vector1.hh>

// C++ headers

#ifdef SERIALIZATION
#include <cereal/types/polymorphic.fwd.hpp>
#endif // SERIALIZATION


namespace core {
namespace scoring {
namespace elec {

struct weight_triple
{
	Real wbb_bb_;
	Real wbb_sc_;
	Real wsc_bb_; //fpd allow asymmetric weights
	Real wsc_sc_;
	Real w_intra_;
};

/// @brief A class to encapsulate the raw-pointer-based data caching
class CountPairRepMap {
public:

	CountPairRepMap() = default;

	// If you enable assignment or copying, you have to make sure to
	// add attaching/detatching of the instance to the ResidueType deletion observers
	CountPairRepMap( CountPairRepMap const & ) = delete;
	CountPairRepMap & operator=( CountPairRepMap const & ) = delete;

	~CountPairRepMap();

	bool
	has( chemical::ResidueType const & restype ) const { return cp_rep_map_.count( &restype ) != 0; }

	std::map<core::Size,core::Size> const &
	get_map( chemical::ResidueType const & restype );

	std::map<core::Size,core::Size> const &
	get_map( chemical::ResidueType const & restype ) const;

	void restype_destruction_observer( core::chemical::RestypeDestructionEvent const & event );

private:
	/// Return the byname repmap, loading it if we haven't already done so.
	CPRepMapType const & cp_byname();

private:
	// The raw pointers here are registered in the destruction observer for their respective ResidueType
	std::map< chemical::ResidueType const *, std::map<core::Size,core::Size> > cp_rep_map_;
	CPRepMapTypeCOP cp_rep_map_byname_;
};

class FA_ElecEnergy : public methods::ContextIndependentTwoBodyEnergy  {
public:
	typedef methods::ContextIndependentTwoBodyEnergy  parent;
public:

	FA_ElecEnergy() = delete; // Need to initialize with options

	FA_ElecEnergy( methods::EnergyMethodOptions const & options );

	FA_ElecEnergy( FA_ElecEnergy const & src );

	/// @brief Initilize constants.
	void
	initialize();

	/// clone
	methods::EnergyMethodOP
	clone() const override;

	/// stashes nblist if use_nblist is true
	void
	setup_for_minimizing(
		pose::Pose & pose,
		ScoreFunction const & sfxn,
		kinematics::MinimizerMapBase const & min_map
	) const override;

	void
	setup_for_derivatives( pose::Pose & pose, ScoreFunction const & ) const override;

	void
	setup_for_scoring( pose::Pose & pose, ScoreFunction const & ) const override;


	void
	setup_for_packing(
		pose::Pose & pose,
		utility::vector1< bool > const &,
		utility::vector1< bool > const &
	) const override;

	// Creates a rotamer trie for the input set of rotamers and stores the trie
	// in the rotamer set.
	void
	prepare_rotamers_for_packing(
		pose::Pose const & pose,
		conformation::RotamerSetBase & set ) const override;

	// Updates the cached rotamer trie for a residue if it has changed during the course of
	// a repacking
	void
	update_residue_for_packing( pose::Pose & pose, Size resid ) const override;

	/////////////////////////////////////////////////////////////////////////////
	// scoring
	/////////////////////////////////////////////////////////////////////////////

	void
	residue_pair_energy(
		conformation::Residue const & rsd1,
		conformation::Residue const & rsd2,
		pose::Pose const & pose,
		ScoreFunction const &,
		EnergyMap & emap
	) const override;

	/// @brief Returns true if we're using neighborlist-autoupdate
	bool
	minimize_in_whole_structure_context( pose::Pose const & pose ) const override;

	bool
	defines_score_for_residue_pair(
		conformation::Residue const & res1,
		conformation::Residue const & res2,
		bool res_moving_wrt_eachother
	) const override;

	bool
	use_extended_residue_pair_energy_interface() const override;

	void
	residue_pair_energy_ext(
		conformation::Residue const & rsd1,
		conformation::Residue const & rsd2,
		ResPairMinimizationData const & min_data,
		pose::Pose const & pose,
		ScoreFunction const & sfxn,
		EnergyMap & emap
	) const override;

	bool
	requires_a_setup_for_scoring_for_residue_opportunity_during_minimization( pose::Pose const & ) const override;

	void
	setup_for_scoring_for_residue(
		conformation::Residue const & rsd,
		pose::Pose const &,// pose,
		ScoreFunction const & sfxn,
		ResSingleMinimizationData & resdata
	) const override;

	bool
	requires_a_setup_for_derivatives_for_residue_opportunity( pose::Pose const &  ) const override;

	void
	setup_for_derivatives_for_residue(
		conformation::Residue const & rsd,
		pose::Pose const & pose,
		ScoreFunction const & sfxn,
		ResSingleMinimizationData & min_data,
		basic::datacache::BasicDataCache &
	) const override;

	void
	setup_for_minimizing_for_residue(
		conformation::Residue const & rsd,
		pose::Pose const & pose,
		ScoreFunction const & scorefxn,
		kinematics::MinimizerMapBase const & min_map,
		basic::datacache::BasicDataCache &,
		ResSingleMinimizationData & resdata
	) const override;

	void
	setup_for_minimizing_for_residue_pair(
		conformation::Residue const & rsd1,
		conformation::Residue const & rsd2,
		pose::Pose const & pose,
		ScoreFunction const & sfxn,
		kinematics::MinimizerMapBase const & minmap,
		ResSingleMinimizationData const & res1_data_cache,
		ResSingleMinimizationData const & res2_data_cache,
		ResPairMinimizationData & data_cache
	) const override;

	/// @brief Evaluate the atom derivative f1/f2 vectors for all atoms on rsd1
	/// in response to the atoms on rsd2, and all the atoms on rsd2 as they
	/// in response to the atoms on rsd1.  This method is used with the
	/// MinimizationGraph and when nblist_autoupdate is not in use.
	void
	eval_residue_pair_derivatives(
		conformation::Residue const & rsd1,
		conformation::Residue const & rsd2,
		ResSingleMinimizationData const &,
		ResSingleMinimizationData const &,
		ResPairMinimizationData const & min_data,
		pose::Pose const & pose, // provides context
		EnergyMap const & weights,
		utility::vector1< DerivVectorPair > & r1_atom_derivs,
		utility::vector1< DerivVectorPair > & r2_atom_derivs
	) const override;

	/// @brief Evaluate the derivative vectors for a particular atom in a given
	/// (asymmetric) pose when nblist_autoupdate is being used.  nblist_autoupdate
	/// cannot be used with symmetric poses, in rtmin, or in minpack.
	void
	eval_atom_derivative(
		id::AtomID const & atom_id,
		pose::Pose const & pose,
		kinematics::DomainMap const & domain_map,
		ScoreFunction const &,
		EnergyMap const & weights,
		Vector & F1,
		Vector & F2
	) const override;

	void
	eval_intrares_derivatives(
		conformation::Residue const & rsd,
		ResSingleMinimizationData const & min_data,
		pose::Pose const & pose,
		EnergyMap const & weights,
		utility::vector1< DerivVectorPair > & atom_derivs
	) const override;


	void
	backbone_backbone_energy(
		conformation::Residue const & rsd1,
		conformation::Residue const & rsd2,
		pose::Pose const & pose,
		ScoreFunction const & sfxn,
		EnergyMap & emap
	) const override;

	void
	backbone_sidechain_energy(
		conformation::Residue const & rsd1,
		conformation::Residue const & rsd2,
		pose::Pose const & pose,
		ScoreFunction const & sfxn,
		EnergyMap & emap
	) const override;

	void
	sidechain_sidechain_energy(
		conformation::Residue const & rsd1,
		conformation::Residue const & rsd2,
		pose::Pose const & pose,
		ScoreFunction const & sfxn,
		EnergyMap & emap
	) const override;

	void
	finalize_total_energy(
		pose::Pose & pose,
		ScoreFunction const &,
		EnergyMap & totals
	) const override;


	void
	eval_intrares_energy(
		conformation::Residue const & rsd,
		pose::Pose const & pose,
		ScoreFunction const &sf,
		EnergyMap & emap
	) const override;

	void
	evaluate_rotamer_pair_energies(
		conformation::RotamerSetBase const & set1,
		conformation::RotamerSetBase const & set2,
		pose::Pose const & pose,
		ScoreFunction const & sfxn,
		EnergyMap const & weights,
		ObjexxFCL::FArray2D< core::PackerEnergy > & energy_table
	) const override;


	//@brief overrides default rotamer/background energy calculation and uses
	// the trie-vs-trie algorithm instead
	void
	evaluate_rotamer_background_energies(
		conformation::RotamerSetBase const & set,
		conformation::Residue const & residue,
		pose::Pose const & pose,
		ScoreFunction const & sfxn,
		EnergyMap const & weights,
		utility::vector1< core::PackerEnergy > & energy_vector
	) const override;

	void
	evaluate_rotamer_intrares_energies(
		conformation::RotamerSetBase const & set,
		pose::Pose const & pose,
		ScoreFunction const & sfxn,
		utility::vector1< core::PackerEnergy > & energies
	) const override;

	bool
	defines_intrares_energy( EnergyMap const & /*weights*/ ) const override;

	/// @brief Interface function for class NeighborList.
	etable::count_pair::CountPairFunctionCOP
	get_intrares_countpair(
		conformation::Residue const &,
		pose::Pose const &,
		ScoreFunction const &sf
	) const;

	/// @brief Interface function for class NeighborList.
	etable::count_pair::CountPairFunctionCOP
	get_count_pair_function(
		Size const,
		Size const,
		pose::Pose const &,
		ScoreFunction const &
	) const;

	etable::count_pair::CountPairFunctionCOP
	get_count_pair_function(
		conformation::Residue const & rsd1,
		conformation::Residue const & rsd2
	) const;


	Distance
	atomic_interaction_cutoff() const override;

	bool
	divides_backbone_and_sidechain_energetics() const override
	{
		return true;
	}

	void indicate_required_context_graphs( utility::vector1< bool > & context_graphs_required ) const override;

	/// @brief fa_elec has pairwise energies, but not single atom energies
	bool
	has_atomistic_pairwise_energies() const override { return true; }

	void
	atomistic_pair_energy(
		core::Size , // Which atom in residue 1?
		conformation::Residue const &, // Residue 1
		core::Size , // Which atom in residue 2?
		conformation::Residue const &, // Residue 2
		pose::Pose const &,
		ScoreFunction const &,
		EnergyMap &
	) const override;

public:
	/// @brief  How close two heavy atoms have to be such that their hydrogen atoms might interact, squared.
	Real
	hydrogen_interaction_cutoff2() const
	{
		return ( hydrogen_interaction_cutoff() )*( hydrogen_interaction_cutoff() );
	}

	/// @brief How close two heavy atoms have to be such that their hydrogen atoms might interact
	Real
	hydrogen_interaction_cutoff() const
	{
		return ( coulomb().max_dis() + 2*core::chemical::MAX_CHEMICAL_BOND_TO_HYDROGEN_LENGTH );
	}

	bool eval_intrares_ST_only() const { return eval_intrares_ST_only_; }

	inline
	Real
	elec_weight( bool intra_res, bool at1isbb, bool at2isbb, weight_triple const & wts ) const {
		if ( intra_res ) {
			return wts.w_intra_;
		} else {
			return ( at1isbb ? ( at2isbb ? wts.wbb_bb_ : wts.wbb_sc_ ) : ( at2isbb ? wts.wsc_bb_ : wts.wsc_sc_ ) );
		}
	}


	//fpd countpair representatives: read tables from DB
	core::Size
	get_countpair_representative_atom(
		core::chemical::ResidueType const & restype,
		core::Size atm_i
	) const;

	inline
	etable::coulomb::Coulomb const &
	coulomb() const {return coulomb_; }

	/// Private methods
private:

	void
	setup_weight_triple(
		EnergyMap const & weights,
		weight_triple & wttrip
	) const;

	trie::RotamerTrieBaseOP
	create_rotamer_trie(
		conformation::RotamerSetBase const & rotset,
		pose::Pose const & pose
	) const;

	trie::RotamerTrieBaseOP
	create_rotamer_trie(
		conformation::Residue const & res,
		pose::Pose const & // will be need to create tries for disulfides
	) const;

	trie::TrieCountPairBaseOP
	get_count_pair_function_trie(
		conformation::RotamerSetBase const & set1,
		conformation::RotamerSetBase const & set2,
		pose::Pose const & pose,
		ScoreFunction const & sfxn
	) const;


	trie::TrieCountPairBaseOP
	get_count_pair_function_trie(
		conformation::Residue const & res1,
		conformation::Residue const & res2,
		trie::RotamerTrieBaseCOP trie1,
		trie::RotamerTrieBaseCOP trie2,
		pose::Pose const & pose,
		ScoreFunction const & sfxn
	) const;

	inline
	Real
	score_atom_pair(
		conformation::Residue const & res1,
		conformation::Residue const & res2,
		Size const at1,
		Size const at2,
		EnergyMap & emap,
		Real const cpweight,
		Real & d2
	) const;

	void
	set_nres_mono(
		core::pose::Pose const & pose
	) const;

	bool
	monomer_test(
		Size irsd,
		Size jrsd
	) const;


private:

	etable::coulomb::Coulomb coulomb_;

	bool exclude_protein_protein_;
	bool exclude_RNA_RNA_;
	bool exclude_RNA_protein_;
	bool exclude_monomer_;
	bool exclude_DNA_DNA_;

	bool eval_intrares_ST_only_; // hpark: intrares_elec for ST hydroxyl only
	bool count_pair_hybrid_;
	bool count_pair_full_; // full strength for everything

	//fpd: countpair representative atoms
	bool use_cp_rep_;
	CountPairRepMapOP cp_rep_map_;

	mutable Size nres_monomer_;

	core::Size version() const override;

};

} // namespace elec
} // namespace scoring
} // namespace core

#ifdef    SERIALIZATION
CEREAL_FORCE_DYNAMIC_INIT( core_scoring_elec_FA_ElecEnergy )
#endif // SERIALIZATION


#endif
