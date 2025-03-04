// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @author Sarel Fleishman (sarelf@uw.edu)
#include <protocols/protein_interface_design/filters/SequenceRecoveryFilter.hh>
#include <protocols/protein_interface_design/filters/SequenceRecoveryFilterCreator.hh>
#include <core/pose/Pose.hh>
#include <core/conformation/Conformation.hh>
#include <utility/tag/Tag.hh>
#include <basic/Tracer.hh>
#include <core/pack/task/TaskFactory.hh>
#include <core/pack/task/PackerTask.hh>
#include <protocols/rosetta_scripts/util.hh>
#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/ScoreFunctionFactory.hh>
#include <core/pose/symmetry/util.hh>
#include <protocols/jd2/util.hh>
#include <protocols/protein_interface_design/design_utils.hh>
#include <core/pose/ref_pose.hh>
#include <core/chemical/ResidueType.hh>
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/filters/filter_schemas.hh>

#include <basic/options/keys/OptionKeys.hh> // AUTO IWYU For OptionKeys,
#include <ObjexxFCL/string.functions.hh> // AUTO IWYU For string_of

namespace protocols {
namespace protein_interface_design {
namespace filters {

static basic::Tracer TR( "protocols.protein_interface_design.filters.SequenceRecoveryFilter" );

/// @brief default ctor
SequenceRecoveryFilter::SequenceRecoveryFilter() :
	parent( "SequenceRecovery" ),
	task_factory_( /* NULL */ ),
	reference_pose_( /* NULL */ ),
	scorefxn_( core::scoring::get_score_function() ),
	rate_threshold_( 0.0 ),
	mutation_threshold_( 100 ),
	mutations_( false ),
	verbose_( false )
{}

core::pack::task::TaskFactoryOP
SequenceRecoveryFilter::task_factory() const
{
	return task_factory_;
}

void
SequenceRecoveryFilter::task_factory( core::pack::task::TaskFactoryOP task_factory )
{
	task_factory_ = task_factory;
}

core::Real
SequenceRecoveryFilter::rate_threshold() const
{
	return( rate_threshold_ );
}

void
SequenceRecoveryFilter::rate_threshold( core::Real const rate )
{
	runtime_assert( rate_threshold_ >= 0 && rate_threshold_ <= 1.0 );
	rate_threshold_ = rate;
}

core::pose::PoseCOP
SequenceRecoveryFilter::reference_pose() const
{
	return reference_pose_;
}

void
SequenceRecoveryFilter::reference_pose( core::pose::PoseCOP pose )
{
	reference_pose_ = pose;
}

void
SequenceRecoveryFilter::reference_pose( core::pose::Pose const & pose )
{
	reference_pose_ = utility::pointer::make_shared< core::pose::Pose >( pose );
}

core::Size
SequenceRecoveryFilter::mutation_threshold() const
{
	return( mutation_threshold_ );
}

void
SequenceRecoveryFilter::mutation_threshold( core::Size const mut )
{
	mutation_threshold_ = mut;
}

bool
SequenceRecoveryFilter::mutations() const
{
	return( mutations_ );
}

void
SequenceRecoveryFilter::mutations( bool const muts )
{
	mutations_ = muts;
}

bool
SequenceRecoveryFilter::verbose() const
{
	return( verbose_ );
}

void
SequenceRecoveryFilter::verbose( bool const verb )
{
	verbose_ = verb;
}

bool
SequenceRecoveryFilter::write2pdb() const
{
	return( write2pdb_ );
}

void
SequenceRecoveryFilter::write2pdb( bool const write )
{
	write2pdb_ = write;
}

void
SequenceRecoveryFilter::scorefxn( core::scoring::ScoreFunctionCOP sfx )
{
	scorefxn_ = sfx->clone();
}

bool
SequenceRecoveryFilter::apply(core::pose::Pose const & pose ) const
{
	if ( mutations_ ) {
		core::Size const num_mutations( (core::Size) compute( pose, false ) );
		TR<<"The designed pose possesses "<<num_mutations<<" compared to the reference pose. ";
		if ( num_mutations <= mutation_threshold_ ) {
			TR<<"Success."<<std::endl;
			return true;
		} else {
			TR<<"Failing."<<std::endl;
			return false;
		}
	} else {
		core::Real const recovery_rate( compute( pose, false ) );
		TR<<"Sequence recovery rate evaluates to "<<recovery_rate<<". ";
		if ( recovery_rate <= rate_threshold_ ) {
			TR<<"Failing."<<std::endl;
			return false;
		} else {
			TR<<"Success."<<std::endl;
			return true;
		}
	}
}

core::Real
SequenceRecoveryFilter::compute( core::pose::Pose const & pose, bool const & write ) const{
	runtime_assert( task_factory() != nullptr );
	runtime_assert( reference_pose() != nullptr );
	core::Size total_residue_ref;
	core::pose::Pose asym_ref_pose;
	if ( core::pose::symmetry::is_symmetric( *reference_pose() ) ) {
		core::pose::symmetry::extract_asymmetric_unit( *reference_pose(), asym_ref_pose);
		for ( core::Size i = 1; i <= asym_ref_pose.size(); ++i ) {
			if ( asym_ref_pose.residue_type(i).name() == "VRT" ) {
				asym_ref_pose.conformation().delete_residue_slow(asym_ref_pose.size());
			}
		}
		total_residue_ref = asym_ref_pose.size();
	} else {
		total_residue_ref = reference_pose()->size();
		asym_ref_pose = *reference_pose();
	}
	core::Size total_residue;
	core::pose::Pose asym_pose;
	if ( core::pose::symmetry::is_symmetric( pose ) ) {
		core::pose::symmetry::extract_asymmetric_unit(pose, asym_pose);
		for ( core::Size i = 1; i <= asym_pose.size(); ++i ) {
			if ( asym_pose.residue_type(i).name() == "VRT" ) {
				asym_pose.conformation().delete_residue_slow(asym_pose.size());
			}
		}
		total_residue = asym_pose.size();
	} else {
		total_residue = pose.size();
		asym_pose = pose;
	}
	if ( total_residue_ref != total_residue ) {
		utility_exit_with_message( "Reference pose and current pose have a different number of residues" );
	}
	core::pack::task::PackerTaskOP packer_task( task_factory_->create_task_and_apply_taskoperations( pose ) );
	core::Size designable_count( 0 );
	core::Size packable_count( 0 );
	for ( core::Size resi=1; resi<=total_residue; ++resi ) {
		if ( packer_task->being_designed( resi ) ) {
			designable_count++;
		}
		if ( packer_task->being_packed( resi ) ) {
			packable_count++;
		}
	}
	if ( !designable_count ) {
		TR.Warning << "No designable residues identified in pose. Are you sure you have set the correct task operations?"<<std::endl;
		if ( !packable_count ) {
			utility_exit_with_message("No designable or packable residues identified in pose. Are you sure you have set the correct task operations?" );
		}
	}
	using namespace core::scoring;
	protocols::protein_interface_design::ReportSequenceDifferences rsd( scorefxn_ );
	rsd.calculate( asym_ref_pose, asym_pose );
	std::map< core::Size, std::string > const res_names1( rsd.res_name1() );
	std::map< core::Size, std::string > const res_names2( rsd.res_name2() );
	core::Size const mutated( res_names1.size() );
	// AMW: cppcheck notices that if there are packable residues but no designable residues
	// we divide by zero here.
	if ( designable_count == 0 ) utility_exit_with_message( "This is impossible -- how are there no designable positions? The rate calculation would divide by zero." );

	core::Real const rate( 1.0 - (core::Real) mutated / designable_count );
	TR<<"Your design mover mutated "<<mutated<<" positions out of "<<designable_count<<" designable positions. Sequence recovery is: "<<rate<<std::endl;
	if ( verbose_ ) {
		rsd.report( TR );
		TR.flush();
	}
	if ( write ) {
		write_to_pdb( res_names1, res_names2 );
	}
	if ( mutations_ ) {
		return( (core::Real) mutated );
	} else {
		return( rate );
	}
}

/// @brief Add each mutation to the output pdb if desired
void
SequenceRecoveryFilter::write_to_pdb( std::map< core::Size, std::string > const & res_names1, std::map< core::Size, std::string > const & res_names2 ) const {

	std::string user_name = this->get_user_defined_name();
	auto it_name1 = res_names1.begin();
	auto it_name2 = res_names2.begin();
	while ( it_name1 != res_names1.end() ) {
		std::string output_string = "SequenceRecoveryFilter " + user_name + ": " + it_name2->second + ObjexxFCL::string_of( it_name1->first ) + it_name1->second;
		protocols::jd2::add_string_to_current_job( output_string );
		++it_name1; ++it_name2;
	}

}

core::Real
SequenceRecoveryFilter::report_sm( core::pose::Pose const & pose ) const
{
	return( compute( pose, write2pdb() ) );
}

void
SequenceRecoveryFilter::report( std::ostream & out, core::pose::Pose const & pose ) const
{
	out<<"SequenceRecoveryFilter returns "<<compute( pose, false )<<std::endl;
}

void
SequenceRecoveryFilter::parse_my_tag( utility::tag::TagCOP tag,
	basic::datacache::DataMap & data
)
{
	TR << "SequenceRecoveryFilter"<<std::endl;
	task_factory( protocols::rosetta_scripts::parse_task_operations( tag, data ) );
	scorefxn( protocols::rosetta_scripts::parse_score_function( tag, data ) );
	rate_threshold( tag->getOption< core::Real >( "rate_threshold", 0.0 ) );
	mutation_threshold( tag->getOption< core::Size >( "mutation_threshold", 100 ) );
	mutations( tag->getOption< bool >( "report_mutations", false ) );
	verbose( tag->getOption< bool >( "verbose", false ) );
	write2pdb( tag->getOption< bool >( "write2pdb", false ) );

	using namespace basic::options;
	using namespace basic::options::OptionKeys;

	reference_pose( protocols::rosetta_scripts::legacy_saved_pose_or_input( tag, data, class_name() ) );
	TR<<std::endl;
}

protocols::filters::FilterOP
SequenceRecoveryFilter::fresh_instance() const{
	return utility::pointer::make_shared< SequenceRecoveryFilter >();
}

SequenceRecoveryFilter::~SequenceRecoveryFilter()= default;


protocols::filters::FilterOP
SequenceRecoveryFilter::clone() const{
	return utility::pointer::make_shared< SequenceRecoveryFilter >( *this );
}



std::string SequenceRecoveryFilter::name() const {
	return class_name();
}

std::string SequenceRecoveryFilter::class_name() {
	return "SequenceRecovery";
}

void SequenceRecoveryFilter::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	rosetta_scripts::attributes_for_parse_task_operations( attlist );
	rosetta_scripts::attributes_for_parse_score_function( attlist );
	attlist + XMLSchemaAttribute::attribute_w_default( "rate_threshold", xsct_real, "Rate of sequence recovery below which the filter fails", "0.0" )
		+ XMLSchemaAttribute::attribute_w_default( "mutation_threshold", xsct_non_negative_integer, "Raw number-of-mutations threshold above which the filter fails", "100" )
		+ XMLSchemaAttribute::attribute_w_default( "report_mutations", xsct_rosetta_bool, "Decide pass/fail based on the mutation threshold, not the rate threshold", "0" )
		+ XMLSchemaAttribute::attribute_w_default( "verbose", xsct_rosetta_bool, "Give a lot more logging output", "0" )
		+ XMLSchemaAttribute::attribute_w_default( "write2pdb", xsct_rosetta_bool, "Write each mutation as a string to the output PDB", "0" );

	core::pose::attributes_for_saved_reference_pose( attlist );

	protocols::filters::xsd_type_definition_w_attributes( xsd, class_name(), "XRW TO DO", attlist );
}

std::string SequenceRecoveryFilterCreator::keyname() const {
	return SequenceRecoveryFilter::class_name();
}

protocols::filters::FilterOP
SequenceRecoveryFilterCreator::create_filter() const {
	return utility::pointer::make_shared< SequenceRecoveryFilter >();
}

void SequenceRecoveryFilterCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	SequenceRecoveryFilter::provide_xml_schema( xsd );
}



} // filters
} // protein_interface_design
} // protocols
