// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file src/protocols/chemically_conjugated_docking/Gp_extra_bodies.hh
/// @brief contains helper quantification metrics for the original publication of the UBQ_Gp code
/// @author Steven Lewis and Hope Anderson

// Unit Headers

// Project Headers
#include <protocols/chemically_conjugated_docking/Gp_extra_bodies.hh>


#include <core/kinematics/FoldTree.hh>

#include <core/conformation/Conformation.hh>
#include <core/conformation/Residue.hh>

#include <core/pose/Pose.hh>

#include <core/import_pose/import_pose.hh>
#include <core/types.hh>

// Utility Headers
#include <basic/Tracer.hh>
#include <basic/options/option.hh>
#include <basic/options/keys/chemically_conjugated_docking.OptionKeys.gen.hh>
#include <utility/vector1.hh>

#include <string>
#include <set>

//tracer
static basic::Tracer TR("protocols.chemically_conjugated_docking.Gp_extra_bodies");

namespace protocols {
namespace chemically_conjugated_docking {


////////////////////////////extra bodies/////////////////////////////////////////////////
/// @brief The purpose of this code is to allow for static extra things
///in the system; for its original incarnations, to allow for a RING
///domain to occlude its site on E2 and a GAP (+MG +GDP) to occlude ras
utility::vector1< core::Size > add_extra_bodies( core::pose::Pose & pose) {

	utility::vector1< core::Size > extra_bodies_chains;

	//double check if extra bodies exist
	if ( basic::options::option[basic::options::OptionKeys::chemically_conjugated_docking::extra_bodies].user() == true ) {

		core::Size const atom_tree_root(pose.fold_tree().root());

		//load in each body into a pose vector
		utility::vector1< std::string > const extra_bodies_files( basic::options::option[basic::options::OptionKeys::chemically_conjugated_docking::extra_bodies]() );
		typedef utility::vector1< core::pose::Pose > PoseVec;
		PoseVec extra_bodies_vec(core::import_pose::poses_from_files(extra_bodies_files, core::import_pose::PDB_file));

		//iterate through bodies adding them
		for ( auto it : extra_bodies_vec ) { //each in extra_bodies

			//attach first residue by jump to atom tree root
			pose.conformation().append_residue_by_jump(
				it.residue(1), //first residue of new
				atom_tree_root, //to atom tree root
				pose.residue(atom_tree_root).atom_name(1), //first atom of tree root
				it.residue(1).atom_name(1), //to first atom of new
				true);

			//attach remaining residues chemically - must be safe for 1-residue poses
			for ( core::Size i(2); i <= it.size(); ++i ) pose.conformation().append_polymer_residue_after_seqpos(it.residue(i), pose.size(), false);

			extra_bodies_chains.push_back(pose.conformation().num_chains()); //add the chain numbers
		}

	}

	TR << "add_extra_bodies added chains:";

	for ( core::Size bodies(1); bodies <= extra_bodies_chains.size(); ++bodies ) {
		TR << " " << extra_bodies_chains[bodies];
	}
	TR << std::endl;

	return extra_bodies_chains;
}

void pack_extra_bodies(
	utility::vector1< core::Size > const & extra_bodies_chains,
	core::pose::Pose const & pose,
	std::set< core::Size > & region) {

	utility::vector1< core::Size > const & chain_endings(pose.conformation().chain_endings());

	for ( core::Size chain(1), end(chain_endings.size()); chain<=end; ++chain ) {
		TR << "chain ending: " << chain_endings[chain] << std::endl;
	}

	//for each extra bodies chain
	for ( core::Size chain(1), end(extra_bodies_chains.size()); chain<=end; ++chain ) {

		//for each residue in a chain
		core::Size const
			start(pose.conformation().chain_begin(extra_bodies_chains[chain])),
			stop(pose.conformation().chain_end(extra_bodies_chains[chain]));

		TR << "adding chain " << extra_bodies_chains[chain] << ":";
		for ( core::Size i(start); i<=stop; ++i ) {
			region.insert(i);
			TR << " " << i;
		}
		TR << std::endl;
	}

	return;
}

}//chemically_conjugated_docking
}//protocols
