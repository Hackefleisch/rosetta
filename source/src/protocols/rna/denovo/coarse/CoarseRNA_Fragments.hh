// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

#ifndef INCLUDED_protocols_coarse_rna_CoarseRNA_Fragments_HH
#define INCLUDED_protocols_coarse_rna_CoarseRNA_Fragments_HH

#include <core/fragment/rna/RNA_Fragments.hh>
#include <core/fragment/rna/TorsionSet.fwd.hh>
#include <core/fragment/rna/RNA_FragmentHomologyExclusion.fwd.hh>
#include <core/pose/toolbox/AtomLevelDomainMap.fwd.hh>
#include <core/pose/Pose.fwd.hh>
#include <core/pose/MiniPose.fwd.hh>
#include <core/types.hh>
#include <utility/VirtualBase.hh>
#include <utility/vector1.hh>

// C++ Headers
#include <string>
#include <map>


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// Goal: to make a fragment object that can choose fragments
// "on the fly" for RNA ab inito folding.
//
// After reading in a set of torsions from, e.g., the ribosome crystal structure,
//  should be able to generate fragments of size 1, 2, or 3, with
//  exact sequence matches, partial Y/R matches, or ignoring sequence.
//
// Note: This is has not really been maintained since about 2011.
//
//
namespace protocols {
namespace rna {
namespace denovo {
namespace coarse {

typedef std::pair< std::string, std::string > SequenceSecStructPair;

class SourcePositions : public utility::VirtualBase{

public:

	SourcePositions(){}
	~SourcePositions() override;
	core::Size size() const { return source_positions_.size(); }
	void push_back( core::Size const & value ){ source_positions_.push_back( value ); }
	core::Size operator[]( core::Size num ){ return source_positions_[ num ]; }

private:

	utility::vector1< core::Size > source_positions_;

};

typedef utility::pointer::shared_ptr< SourcePositions > SourcePositionsOP;


/////////////////////////////////////////////////////////////////////////////////////////////////
class CoarseRNA_Fragments : public core::fragment::rna::RNA_Fragments {
public:

	//Constructor -- needs vall_torsions_file to get started.
	// CoarseRNA_Fragments();
	CoarseRNA_Fragments( std::string const & filename );
	~CoarseRNA_Fragments() override;

	void
	apply_random_fragment(
		core::pose::Pose & pose,
		core::Size const position,
		core::Size const size,
		core::Size const type,
		core::fragment::rna::RNA_FragmentHomologyExclusionCOP const & homology_exclusion,
		core::pose::toolbox::AtomLevelDomainMapCOP atom_level_domain_map,
		core::Size const symm_hack_arity ) const override;

	bool
	is_fullatom() override;

private:

	void
	insert_fragment(
		core::pose::Pose & ,//pose,
		core::Size const ,//position,
		core::fragment::rna::TorsionSet const & ,//torsion_set,
		core::pose::toolbox::AtomLevelDomainMapCOP /*atom_level_domain_map*/ ) const override {
		/// STUBBED OUT!
	}

	virtual
	void
	insert_fragment(
		core::pose::Pose & pose,
		core::Size const & insert_res,
		core::Size const & source_res,
		core::Size const & frag_size,
		core::fragment::rna::RNA_FragmentHomologyExclusionCOP const & homology_exclusion,
		core::pose::toolbox::AtomLevelDomainMapCOP atom_level_domain_map ) const;

	void
	find_source_positions( SequenceSecStructPair const & key ) const;

	core::Size
	pick_random_fragment(
		std::string const & RNA_string,
		std::string const & RNA_secstruct_string,
		core::Size const type /* = MATCH_YR */) const;

	core::Size
	pick_random_fragment(
		core::pose::Pose & pose,
		core::Size const position,
		core::Size const size,
		core::Size const type ) const;

	void
	initialize_frag_source_pose();

	std::string frag_source_secstruct_;

	std::string const frag_source_file_;
	core::pose::MiniPoseOP frag_source_pose_;
	mutable std::map< SequenceSecStructPair, SourcePositionsOP > source_positions_map_;

	std::map< std::string, core::Size > coarse_rna_name_to_num_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////


} //coarse
} //denovo
} //rna
} //protocols

#endif
