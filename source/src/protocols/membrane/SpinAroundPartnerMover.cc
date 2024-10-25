// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file       protocols/membrane/SpinAroundPartnerMover.cc
/// @brief      Spins the downstream partner around the upstream partner
/// @details Spins the downstream partner around the upstream partner in the
///    membrane to probe all kinds of interfaces. Both embedding normals
///    are approximately conserved, i.e. the partners aren't flipped
///    in the membrane.
/// @author     JKLeman (julia.koehler1982@gmail.com)

#ifndef INCLUDED_protocols_membrane_SpinAroundPartnerMover_cc
#define INCLUDED_protocols_membrane_SpinAroundPartnerMover_cc

// Unit Headers
#include <protocols/membrane/SpinAroundPartnerMover.hh>
#include <protocols/membrane/SpinAroundPartnerMoverCreator.hh>
#include <protocols/moves/Mover.hh>

// Project Headers
#include <protocols/membrane/TranslationRotationMover.hh>
#include <core/conformation/membrane/MembraneInfo.hh>
#include <protocols/membrane/geometry/EmbeddingDef.hh>
#include <protocols/membrane/util.hh>

// Package Headers
#include <core/pose/Pose.hh>
#include <core/types.hh>

// Utility Headers
#include <numeric/random/random.hh>
#include <numeric/xyzVector.hh>
#include <basic/options/option.hh>
#include <basic/options/keys/mp.OptionKeys.gen.hh>
#include <utility/tag/Tag.hh>
#include <basic/datacache/DataMap.fwd.hh>
#include <basic/Tracer.hh>

// C++ Headers

// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/moves/mover_schemas.hh>

#include <core/conformation/membrane/SpanningTopology.hh> // AUTO IWYU For SpanningTopology

static basic::Tracer TR( "protocols.membrane.SpinAroundPartnerMover" );

namespace protocols {
namespace membrane {

/////////////////////
/// Constructors  ///
/////////////////////

/// @brief Default Constructor
/// @details Defaults: jump = 1, sampling range = 100
///    Sampling range of 100 means that both x and y are sampled from
///    -100 to +100
SpinAroundPartnerMover::SpinAroundPartnerMover() :
	protocols::moves::Mover()
{
	set_defaults();
	register_options();
}

/// @brief Custom Constructor
/// @details User can specify jump number
SpinAroundPartnerMover::SpinAroundPartnerMover( core::Size jump_num ) :
	protocols::moves::Mover()
{
	set_defaults();
	register_options();

	jump_ = jump_num;
}

/// @brief Custom constructor
/// @details User can specify jump number and sampling range
SpinAroundPartnerMover::SpinAroundPartnerMover(
	core::Size jump_num,
	core::Size range
) : protocols::moves::Mover()
{
	set_defaults();
	register_options();

	jump_ = jump_num;
	rand_range_ = true;
	range_ = range;
}

/// @brief Copy Constructor
/// @details Create a deep copy of this mover
SpinAroundPartnerMover::SpinAroundPartnerMover( SpinAroundPartnerMover const & ) = default;

/// @brief Assignment Operator
SpinAroundPartnerMover &
SpinAroundPartnerMover::operator = ( SpinAroundPartnerMover const & src ) {

	// Abort self-assignment.
	if ( this == &src ) {
		return *this;
	}

	// Otherwise, create a new object
	return *( new SpinAroundPartnerMover( *this ) ); // ??????
}

/// @brief Destructor
SpinAroundPartnerMover::~SpinAroundPartnerMover() = default;

///////////////////////////////
/// Rosetta Scripts Methods ///
///////////////////////////////

/// @brief Create a Clone of this mover
protocols::moves::MoverOP
SpinAroundPartnerMover::clone() const {
	return ( utility::pointer::make_shared< SpinAroundPartnerMover >( *this ) );
}

/// @brief Create a Fresh Instance of this Mover
protocols::moves::MoverOP
SpinAroundPartnerMover::fresh_instance() const {
	return utility::pointer::make_shared< SpinAroundPartnerMover >();
}

/// @brief Pase Rosetta Scripts Options for this Mover
void
SpinAroundPartnerMover::parse_my_tag(
	utility::tag::TagCOP tag,
	basic::datacache::DataMap &
) {

	if ( tag->hasOption("jump") ) {
		jump_ = tag->getOption< core::Size >( "jump", 1 );
	}

	if ( tag->hasOption("rand_range") ) {
		rand_range_ = tag->getOption< bool >( "rand_range", false );
	}

	if ( tag->hasOption("range") ) {
		range_ = tag->getOption< core::Size >( "range", 25 );
	}

	if ( tag->hasOption("x") ) {
		x_ = tag->getOption< core::Real >( "x", 0.0 );
	}

	if ( tag->hasOption("y") ) {
		y_ = tag->getOption< core::Real >( "y", 0.0 );
	}

}

/// @brief Create a new copy of this mover
protocols::moves::MoverOP
SpinAroundPartnerMoverCreator::create_mover() const {
	return utility::pointer::make_shared< SpinAroundPartnerMover >();
}

/// @brief Return the Name of this mover (as seen by Rscripts)
std::string
SpinAroundPartnerMoverCreator::keyname() const {
	return SpinAroundPartnerMoverCreator::mover_name();
}

/// @brief Mover name for Rosetta Scripts
std::string
SpinAroundPartnerMoverCreator::mover_name() {
	return "SpinAroundPartnerMover";
}

/// @brief Provide xml schema for RosettaScripts compatibility
void SpinAroundPartnerMover::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	attlist + XMLSchemaAttribute( "jump", xsct_non_negative_integer, "Jump number")
		+ XMLSchemaAttribute( "rand_range", xsct_rosetta_bool, "Random range")
		+ XMLSchemaAttribute( "range", xsct_non_negative_integer, "Sampling range for this mover")
		+ XMLSchemaAttribute( "x", xsct_real, "X position for this mover")
		+ XMLSchemaAttribute( "y", xsct_real, "Y position for this mover");

	protocols::moves::xsd_type_definition_w_attributes( xsd, SpinAroundPartnerMoverCreator::mover_name(), "Spins the downstream partner around the upstream partner", attlist);
}

/////////////////////
/// Mover Methods ///
/////////////////////

/// @brief Get the name of this Mover (SpinAroundPartnerMover)
std::string
SpinAroundPartnerMover::get_name() const {
	return "SpinAroundPartnerMover";
}

/// @brief Get the jump number for this Mover
core::Size
SpinAroundPartnerMover::get_jump() const {
	return jump_;
}

/// @brief Get the random range for this Mover
bool
SpinAroundPartnerMover::get_rand_range() const {
	return rand_range_;
}

/// @brief Get the sampling range for this Mover
core::Size
SpinAroundPartnerMover::get_range() const {
	return range_;
}

/// @brief Get the x position for this Mover
core::Real
SpinAroundPartnerMover::get_x() const {
	return x_;
}

/// @brief Get the y position for this Mover
core::Real
SpinAroundPartnerMover::get_y() const {
	return y_;
}

/// @brief Set random range
void SpinAroundPartnerMover::random_range( bool yesno ) {
	rand_range_ = yesno;
}

/// @brief Set x position
void SpinAroundPartnerMover::set_x( core::Real x ) {
	x_ = x;
	rand_range_ = false;
}

/// @brief Set y position
void SpinAroundPartnerMover::set_y( core::Real y ) {
	y_ = y;
	rand_range_ = false;
}


/// @brief Flip the downstream partner in the membrane
void SpinAroundPartnerMover::apply( core::pose::Pose & pose ) {

	using namespace core;
	using namespace numeric;
	using namespace core::conformation::membrane;
	using namespace protocols::membrane::geometry;
	using namespace protocols::membrane;

	TR << "Spinning downstream partner around upstream partner in the membrane..." << std::endl;

	// starting foldtree
	TR << "Starting foldtree: Is membrane fixed? " << protocols::membrane::is_membrane_fixed( pose ) << std::endl;
	pose.fold_tree().show( TR );
	core::kinematics::FoldTree orig_ft = pose.fold_tree();

	// compute downstream empedding
	SpanningTopologyOP topo = pose.membrane_info()->spanning_topology();
	SpanningTopologyOP topo_up_( new SpanningTopology() );
	SpanningTopologyOP topo_down_( new SpanningTopology() );

	// split_topology_by_jump_noshift
	split_topology_by_jump_noshift( pose, jump_, topo, topo_up_, topo_down_ );

	// compute embedding for partners (compute structure-based embedding with split topologies)
	EmbeddingDefOP emb_up( compute_structure_based_embedding( pose, *topo_up_ ) );
	EmbeddingDefOP emb_down( compute_structure_based_embedding( pose, *topo_down_ ) );

	// get random x and y positions
	if ( rand_range_ == true ) {

		x_ = static_cast< core::Real > ( numeric::random::random_range( 0, range_ ) );
		y_ = static_cast< core::Real > ( numeric::random::random_range( 0, range_ ) );

		x_ -= ( range_ / 2.0 );
		y_ -= ( range_ / 2.0 );

		// reset if they are within 50A to the downstream center
		core::Real radius = sqrt( pow( ( emb_up->center().x()-x_ ), 2 ) + pow( ( emb_up->center().y()-y_ ), 2 ) );
		while ( radius <= 50.0 ) {
			x_ = static_cast< core::Real > ( numeric::random::random_range( 0, range_ ) );
			y_ = static_cast< core::Real > ( numeric::random::random_range( 0, range_ ) );
			x_ -= ( range_ / 2.0 );
			y_ -= ( range_ / 2.0 );
			radius = sqrt( pow( ( emb_up->center().x()-x_ ), 2 ) + pow( ( emb_up->center().y()-y_ ), 2 ) );
		}

		TR << "radius " << radius << std::endl;
	}

	// set new embedding vector of downstream partner
	core::Vector new_emb_center( x_, y_, 0 );
	TR << "translation: new emb center: " << new_emb_center.to_string() << std::endl;
	core::Vector trans_vector = new_emb_center - emb_down->center();
	TR << "translation: vector: " << trans_vector.to_string() << std::endl;
	TranslationMoverOP trans( new TranslationMover( trans_vector, jump_ ) );
	trans->apply( pose );

	// reset foldtree and show final one
	pose.fold_tree( orig_ft );
	TR << "Final foldtree: Is membrane fixed? " << protocols::membrane::is_membrane_fixed( pose ) << std::endl;
	pose.fold_tree().show( TR );

}// apply

/////////////////////
/// Setup Methods ///
/////////////////////

/// @brief Register Options from Command Line
void
SpinAroundPartnerMover::register_options() {

	using namespace basic::options;
	option.add_relevant( OptionKeys::mp::setup::spanfiles );

}

/// @brief Set default values
void
SpinAroundPartnerMover::set_defaults() {

	jump_ = 1;
	rand_range_ = true;
	range_ = 100;
	x_ = 50.0;
	y_ = 50.0;

}// set_defaults


} // membrane
} // protocols

#endif // INCLUDED_protocols_membrane_SpinAroundPartnerMover_cc
