// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file ResetFoldTree.cc
/// @details wipes out a fold tree making the first residue 0 and the last residue the length of the protein

// Unit headers
#include <protocols/simple_moves/ResetFoldTree.hh>
#include <protocols/simple_moves/ResetFoldTreeCreator.hh>
#include <basic/datacache/DataMap.fwd.hh>
#include <core/kinematics/FoldTree.hh>
#include <basic/Tracer.hh>


#include <core/pose/Pose.hh>
#include <core/pose/util.hh>
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/moves/mover_schemas.hh>

static basic::Tracer TR( "protocols.simple_moves.ResetFoldTree" );

namespace protocols {
namespace simple_moves {


void
ResetFoldTree::apply( Pose & pose )
{
	using core::kinematics::FoldTree;
	FoldTree ft;
	ft = pose.fold_tree();
	ft.clear();
	remove_virtual_residues(pose);
	ft.add_edge(1,pose.total_residue(),core::kinematics::Edge::PEPTIDE);
	pose.fold_tree(ft);

}

moves::MoverOP
ResetFoldTree::clone() const
{
	return utility::pointer::make_shared< ResetFoldTree >( *this );
}

moves::MoverOP
ResetFoldTree::fresh_instance() const
{
	return utility::pointer::make_shared< ResetFoldTree >();
}

void
ResetFoldTree::parse_my_tag(
	utility::tag::TagCOP,
	basic::datacache::DataMap &
)
{
	TR << "nothing to parse";
}

std::string ResetFoldTree::get_name() const {
	return mover_name();
}

std::string ResetFoldTree::mover_name() {
	return "ResetFoldTree";
}

void ResetFoldTree::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	protocols::moves::xsd_type_definition_w_attributes(
		xsd, mover_name(),
		"wipes out a fold tree making the first residue 0 and the last residue the length of the protein",
		attlist );
}

std::string ResetFoldTreeCreator::keyname() const {
	return ResetFoldTree::mover_name();
}

protocols::moves::MoverOP
ResetFoldTreeCreator::create_mover() const {
	return utility::pointer::make_shared< ResetFoldTree >();
}

void ResetFoldTreeCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	ResetFoldTree::provide_xml_schema( xsd );
}


} // simple_moves
} // protocols
