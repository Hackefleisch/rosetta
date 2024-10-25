// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file ResidueTypeConstraintMover.cc
/// @brief Assigns a ResidueTypeConstraint to a pose.
/// @author Doo Nam Kim (doonam.kim@gmail.com) (All I did is just making a simple mover using Sarel's ResidueTypeConstraint)

#include <protocols/constraint_movers/ResidueTypeConstraintMover.hh>
#include <protocols/constraint_movers/ResidueTypeConstraintMoverCreator.hh>

#include <core/pose/Pose.hh>
#include <core/scoring/constraints/ResidueTypeConstraint.hh>
#include <utility/tag/Tag.hh>

#include <basic/options/option.hh>

//Auto Headers

#include <utility/vector1.hh>
#include <basic/Tracer.hh>
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/moves/mover_schemas.hh>

static basic::Tracer TR( "protocols.constraint_movers.ResidueTypeConstraintMover" );

namespace protocols {
namespace constraint_movers {

using namespace core;
using namespace basic::options;
using namespace scoring;
using namespace constraints;

using namespace utility::tag;




// constructors
ResidueTypeConstraintMover::ResidueTypeConstraintMover()
: protocols::moves::Mover( ResidueTypeConstraintMover::mover_name() )
{
}

ResidueTypeConstraintMover::ResidueTypeConstraintMover( std::string const & type )
: protocols::moves::Mover(type)
{
}

// destructor
ResidueTypeConstraintMover::~ResidueTypeConstraintMover()= default;


void
ResidueTypeConstraintMover::apply( Pose & pose )
{
	for ( core::Size resnum=1; resnum<=pose.size(); ++resnum ) {
		for ( auto const & favored_res : AA_name3s_ ) {
			ResidueTypeConstraintOP res_favor( new core::scoring::constraints::ResidueTypeConstraint(pose, resnum, favored_res, favor_bonus_) );
			pose.add_constraint(res_favor);
		}
	}
}


protocols::moves::MoverOP ResidueTypeConstraintMover::clone() const { return utility::pointer::make_shared< protocols::constraint_movers::ResidueTypeConstraintMover >( *this ); }
protocols::moves::MoverOP ResidueTypeConstraintMover::fresh_instance() const { return utility::pointer::make_shared< ResidueTypeConstraintMover >(); }

void
ResidueTypeConstraintMover::parse_my_tag(
	TagCOP const tag,
	basic::datacache::DataMap &
)
{
	if ( tag->hasOption("AA_name3") ) {
		set_AA_name3( utility::string_split( tag->getOption< std::string >( "AA_name3" ), ',' ) ); // for example: ASP,GLU
	} else {
		TR << "please specifiy AA_name3 like SER,THR" << std::endl;
		// TODO: shouldn't we fail?
	}
	set_favor_bonus( tag->getOption< Real >( "favor_bonus", 0.5 ) );
	// since this mover does not necessarily favor native sequence only, I named it "favor_bonus" instead of "native_bonus" to make it more general
	// positively higher bonus gives more favorable selection to (a) specified residue(s)

}

std::string ResidueTypeConstraintMover::get_name() const {
	return mover_name();
}

std::string ResidueTypeConstraintMover::mover_name() {
	return "ResidueTypeConstraintMover";
}

void ResidueTypeConstraintMover::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	attlist
		+ XMLSchemaAttribute::required_attribute( "AA_name3", xs_string, "XRW TO DO" )
		+ XMLSchemaAttribute::attribute_w_default( "favor_bonus", xsct_real, "XRW TO DO", "0.5" );
	protocols::moves::xsd_type_definition_w_attributes( xsd, mover_name(), "XRW TO DO", attlist );
}

std::string ResidueTypeConstraintMoverCreator::keyname() const {
	return ResidueTypeConstraintMover::mover_name();
}

protocols::moves::MoverOP
ResidueTypeConstraintMoverCreator::create_mover() const {
	return utility::pointer::make_shared< ResidueTypeConstraintMover >();
}

void ResidueTypeConstraintMoverCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	ResidueTypeConstraintMover::provide_xml_schema( xsd );
}

} // moves
} // protocols
