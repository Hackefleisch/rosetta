// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file LegacyRepeatAssemblyMover.hh
///
/// @brief An AssemblyMover specific for repeating backbones. This Mover is currently pretty hacky and makes
/// assumes that every node is exactly 3 segments long, and that the assembly is a continuous assembly
/// @author Tim Jacobs


#ifndef INCLUDED_protocol_legacy_sewing_sampling_LegacyRepeatAssemblyMover_HH
#define INCLUDED_protocol_legacy_sewing_sampling_LegacyRepeatAssemblyMover_HH

// Unit Headers
#include <protocols/legacy_sewing/sampling/LegacyRepeatAssemblyMover.fwd.hh>

//Package headers
#include <protocols/legacy_sewing/conformation/Assembly.fwd.hh>
#include <protocols/legacy_sewing/sampling/SewGraph.fwd.hh>

//Protocol headers
#include <core/pose/Pose.fwd.hh>

#include <protocols/legacy_sewing/scoring/LegacyAssemblyScorer.fwd.hh>

//Utility headers
#include <numeric/xyzTransform.fwd.hh>

//Unit headers

#include <protocols/legacy_sewing/sampling/LegacyAssemblyMover.hh> // AUTO IWYU For LegacyAssemblyMover
#include <string> // AUTO IWYU For string
#include <protocols/legacy_sewing/conformation/Model.fwd.hh> // AUTO IWYU For SewSegment


namespace protocols {
namespace legacy_sewing  {

class LegacyRepeatAssemblyMover : public LegacyAssemblyMover {

public:

	typedef LegacyAssemblyMover parent;

	LegacyRepeatAssemblyMover();

	protocols::moves::MoverOP
	clone() const override;

	protocols::moves::MoverOP
	fresh_instance() const override;


	std::pair< bool, AssemblyOP >
	dfs_cycle_finder(
		ModelNode const * reference_node,
		utility::vector1<ModelNode const *> visited_nodes,
		AssemblyOP assembly
	) const;

	AssemblyOP
	generate_assembly() override;

	///adds single repeat unit data to score file
	void
	output_base_stats(
		AssemblyOP const & assembly
	);

	///adds remaining data to score file
	//void
	//output_stats(
	//        AssemblyOP const & assembly,
	//        core::pose::Pose & pose
	//);

	void
	add_repeats(
		AssemblyOP assembly
	) const;

	utility::vector1< numeric::xyzVector<core::Real> >
	get_segment_coords(
		SewSegment const & segment
	) const;

	void
	transform_segment_coords(
		SewSegment & segment,
		numeric::xyzTransform<core::Real> transformer,
		numeric::xyzVector<float> com,
		core::Size n_transformations
	) const;

	void
	parse_my_tag(
		TagCOP tag,
		basic::datacache::DataMap & data
	) override;

	core::pose::Pose
	refine_assembly(
		AssemblyOP & assembly
	) override;

	std::string
	get_name() const override;

	static
	std::string
	mover_name();

	static
	void
	provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );


private:

	core::Size num_repeating_segments_;
	core::Real threshold_score_of_complete_cycle_;
	core::Size num_repeats_;
	core::Size num_trials_;
	scoring::LegacyAssemblyScoreFunctionOP clash_scorefxn_;

};

} //legacy_sewing
} //protocols

#endif
