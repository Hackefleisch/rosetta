// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/simple_filters/NMerPSSMEnergyFilter.cc
/// @brief
/// @author Indigo King (indigo.c.king@gmail.com)


//Unit Headers
#include <protocols/simple_filters/NMerPSSMEnergyFilter.hh>
#include <protocols/simple_filters/NMerPSSMEnergyFilterCreator.hh>

//Project Headers
#include <basic/Tracer.hh>
#include <core/scoring/ScoreType.hh>
#include <core/scoring/nmer/NMerPSSMEnergy.hh>
#include <core/pose/Pose.hh>
#include <utility/tag/Tag.hh>
#include <basic/datacache/DataMap.fwd.hh>
#include <ObjexxFCL/format.hh>
#include <utility/excn/Exceptions.hh>
#include <core/pose/selection.hh>
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/filters/filter_schemas.hh>

#include <core/scoring/EnergyMap.hh> // AUTO IWYU For EMapVector

namespace protocols {
namespace simple_filters {

using namespace core;
using namespace core::scoring;
using namespace ObjexxFCL::format;

static basic::Tracer TR( "protocols.simple_filters.NMerPSSMEnergyFilter" );



//default ctor
NMerPSSMEnergyFilter::NMerPSSMEnergyFilter() :
	protocols::filters::Filter( "NMerPSSMEnergy" )
{}

//full ctor, default ctor defined in header file
//TODO: allow setting energy_method_ funxns w/ input params (like in parse ctor)
NMerPSSMEnergyFilter::NMerPSSMEnergyFilter(
	core::Real const score_type_threshold,
	std::string const string_resnums
) :
	protocols::filters::Filter( "NMerPSSMEnergy" )
{
	score_type_threshold_ = score_type_threshold;
	string_resnums_ = string_resnums;
}

NMerPSSMEnergyFilter::~NMerPSSMEnergyFilter() = default;

void
NMerPSSMEnergyFilter::parse_my_tag(
	utility::tag::TagCOP tag,
	basic::datacache::DataMap & /*data*/
) {
	if ( ! tag->hasOption( "threshold" ) ) throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "Must specify 'threshold' for NMerPSSMEnergyFilter.");
	score_type_threshold_ = tag->getOption< core::Real >( "threshold" );

	if ( tag->hasOption( "pssm_fname" ) ) energy_method_.read_nmer_pssm( tag->getOption< std::string >( "pssm_fname" ) );
	if ( tag->hasOption( "pssm_list_fname" ) ) energy_method_.read_nmer_pssm_list( tag->getOption< std::string >( "pssm_list_fname" ) );
	//default blank string --> empty res_set_vec --> incl all residues
	string_resnums_ = tag->getOption< std::string >( "resnums", "" );// these are kept in memory until the pose is available (at apply time)
	energy_method_.nmer_length( tag->getOption< core::Size >( "nmer_length", 9 ) );
	energy_method_.nmer_pssm_scorecut( tag->getOption< core::Real >( "nmer_pssm_scorecut", 0.0 ) );
	energy_method_.gate_pssm_scores( tag->getOption< bool >( "gate_pssm_scores", false ) );
}

bool
NMerPSSMEnergyFilter::apply( core::pose::Pose const & pose ) const {
	core::Real const score( compute( pose ) );
	TR << "NMerPSSM score is " << score << ". ";
	if ( score <= score_type_threshold_ ) {
		TR<<"passing." << std::endl;
		return true;
	} else {
		TR<<"failing."<<std::endl;
		return false;
	}
}

void
NMerPSSMEnergyFilter::report( std::ostream & out, core::pose::Pose const & pose ) const {
	out<<"NMerPSSM score of " << compute( pose )<<'\n';
}

core::Real
NMerPSSMEnergyFilter::report_sm( core::pose::Pose const & pose ) const {
	return( compute( pose ) );
}

core::Real
NMerPSSMEnergyFilter::compute_residue(
	core::pose::Pose const & pose,
	core::Size const seqpos
) const {
	debug_assert( seqpos <= pose.size() );
	//TR<< "Calculating nmer_pssm energies at seqpos: " << seqpos << std::endl;
	using namespace core::scoring;
	EnergyMap emap; //we need to zero out the emap each time!
	energy_method_.residue_energy( pose.residue( seqpos ), pose, emap );
	return( emap[ nmer_pssm ] );
}

core::Real
NMerPSSMEnergyFilter::compute(
	core::pose::Pose const & pose
) const {

	utility::vector1< core::Size > const res_set_vec( core::pose::get_resnum_list_ordered( string_resnums_, pose ) );

	// sum over all res pos
	core::Real score( 0. );
	if ( res_set_vec.size() > 0 ) {
		for ( core::Size i_res_vec = 1; i_res_vec <= res_set_vec.size(); ++i_res_vec ) {
			core::Size const seqpos( res_set_vec[ i_res_vec ] );
			score += NMerPSSMEnergyFilter::compute_residue( pose, seqpos );
		}
	} else {
		for ( core::Size seqpos = 1; seqpos <= pose.size(); ++seqpos ) {
			score += NMerPSSMEnergyFilter::compute_residue( pose, seqpos );
		}
	}
	return( score );
}

std::string NMerPSSMEnergyFilter::name() const {
	return class_name();
}

std::string NMerPSSMEnergyFilter::class_name() {
	return "NMerPSSMEnergy";
}

void NMerPSSMEnergyFilter::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	attlist + XMLSchemaAttribute("threshold", xsct_real, "XRW TO DO")
		+ XMLSchemaAttribute("pssm_fname", xs_string, "XRW TO DO")
		+ XMLSchemaAttribute("pssm_list_fname", xs_string, "XRW TO DO")
		+ XMLSchemaAttribute::attribute_w_default("resnums", xs_string, "XRW TO DO", "XRW TO DO")
		+ XMLSchemaAttribute::attribute_w_default("nmer_length", xsct_non_negative_integer, "XRW TO DO", "9")
		+ XMLSchemaAttribute::attribute_w_default("nmer_pssm_scorecut", xsct_real, "XRW TO DO", "0.0")
		+ XMLSchemaAttribute::attribute_w_default("gate_pssm_scores", xsct_rosetta_bool, "XRW TO DO", "false");

	protocols::filters::xsd_type_definition_w_attributes( xsd, class_name(), "XRW TO DO", attlist );
}

std::string NMerPSSMEnergyFilterCreator::keyname() const {
	return NMerPSSMEnergyFilter::class_name();
}

protocols::filters::FilterOP
NMerPSSMEnergyFilterCreator::create_filter() const {
	return utility::pointer::make_shared< NMerPSSMEnergyFilter >();
}

void NMerPSSMEnergyFilterCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	NMerPSSMEnergyFilter::provide_xml_schema( xsd );
}


}
}
