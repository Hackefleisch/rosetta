// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/ligand_docking/HeavyAtomFilter.cc
/// @brief Find packing defects at an interface using packstat score terms
/// @author Jacob Corn (jecorn@u.washington.edu)

// Unit headers
#include <protocols/ligand_docking/HeavyAtomFilter.hh>
#include <protocols/ligand_docking/HeavyAtomFilterCreator.hh>


// Project Headers
#include <core/types.hh>
#include <core/pose/Pose.hh>
#include <core/conformation/Conformation.hh>
#include <utility/tag/Tag.hh>
#include <core/pose/util.hh>
#include <core/pose/chains_util.hh>

#include <basic/Tracer.hh>

//Auto Headers
#include <utility/excn/Exceptions.hh>
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/filters/filter_schemas.hh>


namespace protocols {
namespace ligand_docking {

static basic::Tracer heavy_atom_tracer( "protocols.ligand_docking.HeavyAtomFilter" );

bool
HeavyAtomFilter::apply( core::pose::Pose const & pose ) const {

	if ( report_sm(pose) > heavy_atom_limit_ ) {
		heavy_atom_tracer<< "Reached heavy atom limit"<< std::endl;
		return false;
	}
	return true;
}

core::Real
HeavyAtomFilter::report_sm( core::pose::Pose const & pose ) const {
	debug_assert(chain_.size()==1 );
	debug_assert(heavy_atom_limit_ >0 );
	core::Size const chain_id= core::pose::get_chain_id_from_chain(chain_, pose);
	core::Size const start = pose.conformation().chain_begin(chain_id);
	core::Size const end = pose.conformation().chain_end(chain_id);
	return core::pose::num_heavy_atoms(start,end,pose);
}

void
HeavyAtomFilter::parse_my_tag( utility::tag::TagCOP tag, basic::datacache::DataMap & )
{
	if ( ! (tag->hasOption("chain") && tag->hasOption("heavy_atom_limit") ) ) {
		throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "HeavyAtom filter needs a 'chain' and a 'heavy_atom_limit' option");
	}
	chain_ = tag->getOption<std::string>("chain");
	heavy_atom_limit_ = tag->getOption<core::Size>("heavy_atom_limit");

}



std::string HeavyAtomFilter::name() const {
	return class_name();
}

std::string HeavyAtomFilter::class_name() {
	return "HeavyAtom";
}

void HeavyAtomFilter::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	attlist
		+ XMLSchemaAttribute::required_attribute( "chain", xsct_char, "XRW TO DO" )
		+ XMLSchemaAttribute::required_attribute( "heavy_atom_limit", xsct_non_negative_integer, "XRW TO DO" );

	protocols::filters::xsd_type_definition_w_attributes( xsd, class_name(), "XRW TO DO", attlist );
}

std::string HeavyAtomFilterCreator::keyname() const {
	return HeavyAtomFilter::class_name();
}

protocols::filters::FilterOP
HeavyAtomFilterCreator::create_filter() const {
	return utility::pointer::make_shared< HeavyAtomFilter >();
}

void HeavyAtomFilterCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	HeavyAtomFilter::provide_xml_schema( xsd );
}



} // ligand_docking
} // protocols
