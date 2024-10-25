// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file /protocols/rigid/RollMover.hh
/// @brief
/// @author
#ifndef INCLUDED_protocols_rigid_RollMover_hh
#define INCLUDED_protocols_rigid_RollMover_hh
// Unit Headers
#include <protocols/rigid/RollMover.fwd.hh>
// Project Headers
#include <core/pose/Pose.fwd.hh>
#include <protocols/moves/Mover.hh>
#include <core/pose/ResidueIndexDescription.fwd.hh>

// Utility Headers
#include <core/types.hh>

#include <numeric/xyzVector.hh>

namespace protocols {
namespace rigid {

/// @details
class RollMover : public protocols::moves::Mover {

public:

	/// @brief
	RollMover();

	RollMover(
		core::Size start_res,
		core::Size stop_res,
		core::Real min_angle,
		core::Real max_angle,
		numeric::xyzVector< core::Real > axis,
		numeric::xyzVector< core::Real > translate,
		bool legacy_mode = false
	);

	~RollMover() override;

	void apply( core::pose::Pose & pose ) override;
	void set_min_max_angles( core::Real min_angle, core::Real max_angle );

	/// @brief required in the context of the parser/scripting scheme
	moves::MoverOP fresh_instance() const override;

	/// @brief required in the context of the parser/scripting scheme
	moves::MoverOP clone() const override;


	void
	parse_my_tag(
		TagCOP /*tag*/,
		basic::datacache::DataMap & /*data*/
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

	core::pose::ResidueIndexDescriptionCOP start_res_;
	core::pose::ResidueIndexDescriptionCOP stop_res_;
	core::Real angle_ = 0.0;
	core::Real min_angle_ = 0.0;
	core::Real max_angle_ = 0.0;
	numeric::xyzVector< core::Real > axis_ = {0,0,0};
	/// @brief If true, overwrite the translate_ vector with the apply() Pose's center of mass.
	//bool use_com_ = false;
	numeric::xyzVector< core::Real > translate_ = {0,0,0};
	bool random_roll_ = false;
	core::Real random_roll_angle_ = 0.0;
	core::Real random_roll_trans_ = 0.0;

	bool legacy_mode_ = false;
};//end RollMover


}//namespace rigid
}//namespace protocols

#endif
