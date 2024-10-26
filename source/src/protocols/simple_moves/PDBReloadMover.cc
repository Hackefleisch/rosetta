// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   PDBReloadMover.cc
///
/// @brief
/// @author Javier Castellanos (javiercv@uw.edu)

// unit headers
#include <protocols/simple_moves/PDBReloadMover.hh>
#include <protocols/simple_moves/PDBReloadMoverCreator.hh>

// type headers

// project headers
#include <protocols/moves/Mover.hh>
#include <core/pose/Pose.hh>
#include <core/import_pose/import_pose.hh>

// utility header
#include <basic/Tracer.hh>

#include <sstream>

//option key includes
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/moves/mover_schemas.hh>

namespace protocols {
namespace simple_moves {

static basic::Tracer TR( "protocols.simple_moves.PDBReloadMover" );




PDBReloadMover::PDBReloadMover() :
	protocols::moves::Mover("PDBReloadMover")
{}

PDBReloadMover::~PDBReloadMover() = default;

protocols::moves::MoverOP
PDBReloadMover::clone() const
{
	return utility::pointer::make_shared< PDBReloadMover >( *this );
}

protocols::moves::MoverOP
PDBReloadMover::fresh_instance() const
{
	return utility::pointer::make_shared< PDBReloadMover >();
}

void
PDBReloadMover::apply( Pose & pose ) {
	std::ostringstream ss;
	pose.dump_pdb(ss);
	core::import_pose::pose_from_pdbstring(pose, ss.str());
}


void
PDBReloadMover::parse_my_tag( utility::tag::TagCOP const /*tag*/,
	basic::datacache::DataMap & /*data_map*/
)
{
}

std::string PDBReloadMover::get_name() const {
	return mover_name();
}

std::string PDBReloadMover::mover_name() {
	return "PDBReload";
}

void PDBReloadMover::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	// AMW: consciously blank; empty PMT.
	using namespace utility::tag;
	AttributeList attlist;

	protocols::moves::xsd_type_definition_w_attributes( xsd, mover_name(), "XRW TO DO", attlist );
}

std::string PDBReloadMoverCreator::keyname() const {
	return PDBReloadMover::mover_name();
}

protocols::moves::MoverOP
PDBReloadMoverCreator::create_mover() const {
	return utility::pointer::make_shared< PDBReloadMover >();
}

void PDBReloadMoverCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	PDBReloadMover::provide_xml_schema( xsd );
}


} // moves
} // protocols
