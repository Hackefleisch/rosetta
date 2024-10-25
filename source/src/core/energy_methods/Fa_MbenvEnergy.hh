// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/energy_methods/Fa_MbenvEnergy.hh
/// @author Patrick Barth


#ifndef INCLUDED_core_energy_methods_Fa_MbenvEnergy_hh
#define INCLUDED_core_energy_methods_Fa_MbenvEnergy_hh

// Unit headers
#include <core/energy_methods/Fa_MbenvEnergy.fwd.hh>
#include <core/scoring/MembraneTopology.fwd.hh>

// Package headers
#include <core/scoring/methods/ContextDependentOneBodyEnergy.hh>
#include <core/scoring/Membrane_FAPotential.fwd.hh>
#include <core/scoring/MembranePotential.fwd.hh>
//#include <core/scoring/MembranePotential.fwd.hh>
#include <core/scoring/memb_etable/MembEtable.fwd.hh>
#include <core/scoring/ScoreFunction.fwd.hh>
#include <core/conformation/Atom.fwd.hh>

// Project headers
#include <core/pose/Pose.fwd.hh>

#include <ObjexxFCL/FArray1.fwd.hh>

#include <utility/vector1.hh>


namespace core {
namespace energy_methods {



class Fa_MbenvEnergy : public core::scoring::methods::ContextDependentOneBodyEnergy {
public:
	typedef core::scoring::methods::ContextDependentOneBodyEnergy  parent;
public:

	Fa_MbenvEnergy( core::scoring::etable::MembEtable const & memb_etable_in );

	core::scoring::methods::EnergyMethodOP clone() const override;

	void residue_energy( conformation::Residue const & rsd, pose::Pose const & pose, core::scoring::EnergyMap & emap ) const override;

	void
	finalize_total_energy(
		pose::Pose & pose,
		core::scoring::ScoreFunction const &,
		core::scoring::EnergyMap & emap
	) const override;

	void
	setup_for_derivatives(
		pose::Pose & pose,
		core::scoring::ScoreFunction const & scfxn
	) const override;

	void
	eval_atom_derivative(
		id::AtomID const & id,
		pose::Pose const & pose,
		kinematics::DomainMap const & domain_map,
		core::scoring::ScoreFunction const & sfxn,
		core::scoring::EnergyMap const & emap,
		Vector & F1,
		Vector & F2
	) const override;

	/// @brief Fa_MbenvEnergy is context independent; indicates that no context graphs are required
	void indicate_required_context_graphs( utility::vector1< bool > & ) const override;


	core::scoring::MembraneTopology const & MembraneTopology_from_pose( pose::Pose const & ) const;
	core::scoring::Membrane_FAEmbed const & Membrane_FAEmbed_from_pose( pose::Pose const & ) const;
	core::scoring::MembraneEmbed const & MembraneEmbed_from_pose( pose::Pose const & ) const;


	void
	setup_for_scoring(
		pose::Pose & pose,
		core::scoring::ScoreFunction const &
	) const override;

private:


	Real
	eval_fa_mbenv(
		conformation::Atom const & atom1,
		Real const & f1
	) const;

private:

	//etable::MembEtable const & memb_etable_;

	//taken from the MembEtable
	ObjexxFCL::FArray1< Real > const & lk_dgrefce_;
	ObjexxFCL::FArray1< Real > const & memb_lk_dgrefce_;

	core::scoring::Membrane_FAPotential const & potential_;
	mutable Real fa_mbenv_weight_; // hold this while calculating derivatives.
	core::Size version() const override;

};

} // scoring
} // core


#endif // INCLUDED_core_energy_methods_Fa_MbenvEnergy_HH
