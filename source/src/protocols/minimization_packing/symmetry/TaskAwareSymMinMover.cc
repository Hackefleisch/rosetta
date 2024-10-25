// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.
//
/// @file
/// @brief
/// @author Neil King ( neilking@uw.edu )

// Unit headers
#include <protocols/minimization_packing/symmetry/TaskAwareSymMinMover.hh>
#include <protocols/minimization_packing/symmetry/TaskAwareSymMinMoverCreator.hh>

// project headers
#include <core/kinematics/MoveMap.hh>

#include <core/pack/task/PackerTask.hh>
#include <core/pack/task/TaskFactory.hh>

#include <core/pose/Pose.hh>
#include <core/pose/symmetry/util.hh>


#include <core/conformation/symmetry/SymmetryInfo.hh>

#include <core/scoring/ScoreFunction.fwd.hh>

#include <basic/datacache/DataMap.fwd.hh>

#include <protocols/rosetta_scripts/util.hh>
#include <protocols/minimization_packing/symmetry/SymMinMover.hh>

#include <utility>
#include <utility/tag/Tag.hh>
#include <utility/string_util.hh>
#include <basic/Tracer.hh>
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/moves/mover_schemas.hh>

static basic::Tracer TR( "protocols.minimization_packing.symmetry.TaskAwareSymMinMover" );

namespace protocols {
namespace minimization_packing {
namespace symmetry {

using namespace core;
using namespace utility;

// -------------  Mover Creator -------------


// -------------  Mover Creator -------------

TaskAwareSymMinMover::TaskAwareSymMinMover() :
	min_chi_(false),
	min_bb_(false),
	min_rb_(false),
	min_type_("lbfgs_armijo_nonmonotone"),
	tolerance_(1e-5),
	minmover_(/* 0 */),
	factory_(/* 0 */),
	designable_only_(true)
{ }

// constructor with TaskFactory
TaskAwareSymMinMover::TaskAwareSymMinMover(
	protocols::minimization_packing::MinMoverOP minmover_in,
	core::pack::task::TaskFactoryCOP factory_in
) :
	min_chi_(false),
	min_bb_(false),
	min_rb_(false),
	min_type_("lbfgs_armijo_nonmonotone"),
	tolerance_(1e-5),
	minmover_(std::move(minmover_in)),
	factory_(std::move(factory_in)),
	designable_only_(true)
{}
//protocols::minimization_packing::TaskAwareMinMover(
// minmover_in, factory_in) {}

protocols::moves::MoverOP
TaskAwareSymMinMover::clone() const {
	return utility::pointer::make_shared< TaskAwareSymMinMover >( *this );
}

protocols::moves::MoverOP
TaskAwareSymMinMover::fresh_instance() const {
	return utility::pointer::make_shared< TaskAwareSymMinMover >();
}


void
TaskAwareSymMinMover::apply(Pose & pose) {

	runtime_assert( factory_ != nullptr );

	// Initialize a MoveMap, set all moves to false
	core::kinematics::MoveMapOP movemap( new core::kinematics::MoveMap );
	movemap->set_jump(false);
	movemap->set_bb(false);
	movemap->set_chi(false);

	// Modify MoveMap to set allowable move types at positions defined by TaskOperations
	core::conformation::symmetry::SymmetryInfoCOP sym_info = core::pose::symmetry::symmetry_info(pose);
	core::Size nres_monomer = sym_info->num_independent_residues();
	core::pack::task::PackerTaskOP task = core::pack::task::TaskFactory::create_packer_task( pose );
	if ( factory_ != nullptr ) {
		task = factory_->create_task_and_apply_taskoperations( pose );
	} else {
		TR.Warning << "You have not provided any TaskOperations. A default will be used." << std::endl;
	}
	for ( core::Size i = 1; i <= nres_monomer; i++ ) {
		if ( designable_only_ ) {
			if ( task->design_residue( i ) ) {
				movemap->set_bb (i, min_bb_);
				movemap->set_chi(i, min_chi_);
			}
		} else {
			if ( task->pack_residue( i ) ) {
				movemap->set_bb (i, min_bb_);
				movemap->set_chi(i, min_chi_);
			}
		}
	}

	// Make MoveMap symmetric, apply it to minimize the pose
	core::pose::symmetry::make_symmetric_movemap( pose, *movemap );
	protocols::minimization_packing::symmetry::SymMinMover m1( movemap, scorefxn_, min_type_, tolerance_, true, false, false );
	m1.apply(pose);

	// If rigid body minimization is desired, minimize again with that
	if ( min_rb_ ) {
		if ( rb_symdofs_.size() == 0 ) {
			movemap->set_jump(true);
		} else {
			for ( core::Size i=1; i<=rb_symdofs_.size(); ++i ) {
				core::Size jid = sym_info->get_jump_num(rb_symdofs_[i]);
				movemap->set_jump(jid, true);
			}
		}
		core::pose::symmetry::make_symmetric_movemap( pose, *movemap );
		TR.Debug << *movemap << std::endl;
		protocols::minimization_packing::symmetry::SymMinMover m2( movemap, scorefxn_, min_type_, tolerance_, true, false, false );
		m2.apply(pose);
	}

}

void
TaskAwareSymMinMover::parse_my_tag( utility::tag::TagCOP tag,
	basic::datacache::DataMap & data
) {

	scorefxn_name_ = tag->getOption< std::string >( "scorefxn", "score12_symm" );
	min_chi_ = tag->getOption< bool >( "chi", true );
	min_bb_ = tag->getOption< bool >( "bb", false );
	min_rb_ = tag->getOption< bool >( "rb", false );
	if ( tag->hasOption("symdofs") ) {
		std::string symdofs_str = tag->getOption< std::string >( "symdofs" );
		rb_symdofs_ = utility::string_split( symdofs_str , ',' );
	}
	min_type_ = tag->getOption< std::string >( "type", "lbfgs_armijo_nonmonotone" );
	tolerance_ = tag->getOption< core::Real >( "tolerance", 1e-5 );
	designable_only_ = tag->getOption< bool >( "designable_only", true );
	// Get the ScoreFunction and TaskOperations from the basic::datacache::DataMap
	scorefxn_ = protocols::rosetta_scripts::parse_score_function( tag, data, scorefxn_name_ );
	factory_ = protocols::rosetta_scripts::parse_task_operations( tag, data );

}

std::string TaskAwareSymMinMover::get_name() const {
	return mover_name();
}

std::string TaskAwareSymMinMover::mover_name() {
	return "TaskAwareSymMinMover";
}

void TaskAwareSymMinMover::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	attlist + XMLSchemaAttribute::attribute_w_default( "scorefxn" , xs_string, "Score function to use.", "score12_symm" )
		+ XMLSchemaAttribute::attribute_w_default( "chi", xsct_rosetta_bool, "Whether to allow side chain minimization.", "true" )
		+ XMLSchemaAttribute::attribute_w_default( "bb", xsct_rosetta_bool, "Whether to allow backbone minimization.", "false" )
		+ XMLSchemaAttribute::attribute_w_default( "rb", xsct_rosetta_bool, "Whether to allow rigid body minimization.", "false" )
		+ XMLSchemaAttribute::attribute_w_default( "symdofs", xs_string, "The symmetric dofs that are allowed to move (if rb is enabled).", "" )
		+ XMLSchemaAttribute::attribute_w_default( "type", xs_string, "Minimization type. For example, can also be 'dfpmin_armijo_nonmonotone'.", "lbfgs_armijo_nonmonotone" )
		+ XMLSchemaAttribute::attribute_w_default( "tolerance", xsct_real, "Tolerance of minimization?", "1e-5" )
		+ XMLSchemaAttribute::attribute_w_default( "designable_only", xsct_rosetta_bool, "If true, only minimize designable residues.", "true" ) ;

	//protocols::rosetta_scripts::attributes_for_parse_score_function( attlist , "scorefxn" ) ;
	protocols::rosetta_scripts::attributes_for_parse_task_operations( attlist ) ;

	protocols::moves::xsd_type_definition_w_attributes( xsd, mover_name(), "(This is a devel Mover and not available in released versions.) A task-aware version of the SymMinMover that allows minimization of only certain sets of residues specified by user-defined task operations.", attlist );
}

std::string TaskAwareSymMinMoverCreator::keyname() const {
	return TaskAwareSymMinMover::mover_name();
}

protocols::moves::MoverOP
TaskAwareSymMinMoverCreator::create_mover() const {
	return utility::pointer::make_shared< TaskAwareSymMinMover >();
}

void TaskAwareSymMinMoverCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	TaskAwareSymMinMover::provide_xml_schema( xsd );
}


} // symmetry
} // minimization_packing
} // protocols
