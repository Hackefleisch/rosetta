// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocol/ligand_docking/RandomConformers.cc
/// @brief  implementation of resfile reader and its command classes
/// @author Gordon Lemmon (glemmon@gmail.com), adapted from the ResfileReader code
/// by Steven Lewis (smlewi@gmail.com) and Andrew Leaver-Fay

// Project Headers
#include <core/pose/Pose.hh>
#include <core/conformation/Conformation.hh>

// Unit Headers
#include <protocols/ligand_docking/RandomConformers.hh>
#include <protocols/ligand_docking/RandomConformersCreator.hh>

#include <protocols/ligand_docking/RandomConformerMover.hh>

#include <core/pose/chains_util.hh>

// Utility Headers
#include <basic/Tracer.hh>
#include <core/types.hh>
#include <utility/tag/Tag.hh>

#include <utility/excn/Exceptions.hh>
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/moves/mover_schemas.hh>



namespace protocols {
namespace ligand_docking {

///////////////////////////////////////////////////////////////////////
/// @brief

static basic::Tracer random_conformer_tracer( "protocols.ligand_docking.ligand_options.RandomConformers", basic::t_debug );




RandomConformers::RandomConformers():
	//utility::VirtualBase(),
	Mover("RandomConformers")
{}

RandomConformers::RandomConformers(RandomConformers const & /*that*/) = default;

RandomConformers::~RandomConformers() = default;

protocols::moves::MoverOP RandomConformers::clone() const {
	return utility::pointer::make_shared< RandomConformers >( *this );
}

protocols::moves::MoverOP RandomConformers::fresh_instance() const {
	return utility::pointer::make_shared< RandomConformers >();
}


//void RandomConformers::set_chain(std::string chain)
//{
// chain_ = chain;
//}

/// @brief parse XML (specifically in the context of the parser/scripting scheme)
void
RandomConformers::parse_my_tag(
	utility::tag::TagCOP tag,
	basic::datacache::DataMap & /*datamap*/
)
{
	if ( tag->getName() != "RandomConformers" ) {
		throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "This should be impossible");
	}
	if ( ! tag->hasOption("chain") ) throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "'RandomConformers' mover requires chain tag");

	chain_ = tag->getOption<std::string>("chain");
}

void RandomConformers::apply(core::pose::Pose & pose) {
	core::Size chain_id= core::pose::get_chain_id_from_chain(chain_, pose);
	core::Size i = pose.conformation().chain_begin(chain_id);
	core::Size end = pose.conformation().chain_end(chain_id);

	for ( ; i != end; ++i ) {
		apply_residue(i, pose);
	}
}

void RandomConformers::apply_residue(core::Size const residue_id, core::pose::Pose & pose) {
	using namespace protocols::moves;
	using core::conformation::ResidueOP;
	RandomConformerMoverOP rcm( new RandomConformerMover(residue_id) );
	rcm->apply(pose);
	/// TODO accomplish the below code within the scripter
	// UnconstrainedTorsionsMoverOP utm =
	//   new UnconstrainedTorsionsMover(rcm, ligand_torsion_restraints_);
	// utm->apply(pose);
}

std::string RandomConformers::get_name() const {
	return mover_name();
}

std::string RandomConformers::mover_name() {
	return "RandomConformers";
}

void RandomConformers::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	attlist + XMLSchemaAttribute::required_attribute("chain", xs_string, "Chain ID");
	protocols::moves::xsd_type_definition_w_attributes( xsd, mover_name(), "Replace all residues in a chain "
		"with a randomly selected conformer from its respective rotamer library.", attlist );
}

std::string RandomConformersCreator::keyname() const {
	return RandomConformers::mover_name();
}

protocols::moves::MoverOP
RandomConformersCreator::create_mover() const {
	return utility::pointer::make_shared< RandomConformers >();
}

void RandomConformersCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	RandomConformers::provide_xml_schema( xsd );
}


} //namespace ligand_docking
} //namespace protocols
