// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file SwitchResidueTypeSetMover.cc
/// @brief switch between residue type sets (e.g. centroid and all atom)

// Unit headers
#include <protocols/simple_moves/SwitchResidueTypeSetMover.hh>
#include <protocols/simple_moves/SwitchResidueTypeSetMoverCreator.hh>

#include <core/chemical/ChemicalManager.hh>

#include <core/util/SwitchResidueTypeSet.hh>

#include <utility/tag/Tag.hh>

// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/moves/mover_schemas.hh>

#include <basic/Tracer.hh>
static basic::Tracer TR( "protocols.simple_moves.SwitchResidueTypeSetMover" );

namespace protocols {
namespace simple_moves {




SwitchResidueTypeSetMover::SwitchResidueTypeSetMover()
: moves::Mover("SwitchResidueTypeSetMover")
{}

SwitchResidueTypeSetMover::SwitchResidueTypeSetMover( std::string const & type_set_tag_in )
: moves::Mover("SwitchResidueTypeSetMover"),
	type_set_tag_( type_set_tag_in )
{}

std::string
SwitchResidueTypeSetMover::get_residue_type_set() const {
	return type_set_tag_;
}

void
SwitchResidueTypeSetMover::apply( Pose & pose )
{
	core::chemical::TypeSetMode mode( core::chemical::type_set_mode_from_string( type_set_tag_ ) );
	if ( mode != core::chemical::INVALID_t ) {
		core::util::switch_to_residue_type_set( pose, mode );
	} else {
		core::util::switch_to_residue_type_set( pose, type_set_tag_ );
	}
}


void
SwitchResidueTypeSetMover::show(std::ostream & output) const
{
	Mover::show(output);
	output << "Residue type set: " << get_residue_type_set() << std::endl;
}

moves::MoverOP
SwitchResidueTypeSetMover::clone() const
{
	return utility::pointer::make_shared< SwitchResidueTypeSetMover >( *this );
}

moves::MoverOP
SwitchResidueTypeSetMover::fresh_instance() const
{
	return utility::pointer::make_shared< SwitchResidueTypeSetMover >();
}

void
SwitchResidueTypeSetMover::parse_my_tag(
	utility::tag::TagCOP tag,
	basic::datacache::DataMap &
)
{
	if ( tag->hasOption("set") ) type_set_tag_ = tag->getOption<std::string>("set");
}

std::string SwitchResidueTypeSetMover::get_name() const {
	return mover_name();
}

std::string SwitchResidueTypeSetMover::mover_name() {
	return "SwitchResidueTypeSetMover";
}

void SwitchResidueTypeSetMover::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	// TO DO!
	using namespace utility::tag;
	AttributeList attlist; // TO DO: add attributes to this list
	attlist + XMLSchemaAttribute("set", xs_string, "XRW TO DO");

	protocols::moves::xsd_type_definition_w_attributes( xsd, mover_name(), "XRW TO DO", attlist );
}

std::string SwitchResidueTypeSetMoverCreator::keyname() const {
	return SwitchResidueTypeSetMover::mover_name();
}

protocols::moves::MoverOP
SwitchResidueTypeSetMoverCreator::create_mover() const {
	return utility::pointer::make_shared< SwitchResidueTypeSetMover >();
}

void SwitchResidueTypeSetMoverCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	SwitchResidueTypeSetMover::provide_xml_schema( xsd );
}


std::ostream &operator<< (std::ostream &os, SwitchResidueTypeSetMover const &mover)
{
	mover.show(os);
	return os;
}

} // simple_moves
} // protocols
