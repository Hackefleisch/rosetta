// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/antibody/clusters/CDRClusterFeatures.cc
/// @brief
/// @author Jared Adolf-Bryfogle (jadolfbr@gmail.com)


#include <protocols/antibody/clusters/CDRClusterFeatures.hh>
#include <protocols/antibody/clusters/CDRCluster.hh>

#include <protocols/antibody/AntibodyInfo.hh>
#include <protocols/antibody/AntibodyEnumManager.hh>

#include <basic/options/option.hh>
#include <basic/options/keys/OptionKeys.hh>
#include <basic/options/keys/antibody.OptionKeys.gen.hh>

#include <basic/database/sql_utils.hh>
#include <basic/database/schema_generator/PrimaryKey.hh>
#include <basic/database/schema_generator/ForeignKey.hh>
#include <basic/database/schema_generator/Column.hh>
#include <basic/database/schema_generator/Schema.hh>
#include <basic/database/schema_generator/DbDataType.hh>
#include <utility/string_util.hh>
#include <utility/tag/Tag.hh>
#include <utility/excn/Exceptions.hh>
#include <basic/datacache/DataMap.fwd.hh>
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/features/feature_schemas.hh>
#include <protocols/antibody/clusters/CDRClusterFeaturesCreator.hh>

#include <sstream> // MANUAL IWYU

namespace protocols {
namespace antibody {
namespace clusters {
using namespace protocols::features;
using namespace protocols::antibody;
using utility::vector1;

CDRClusterFeatures::CDRClusterFeatures():
	FeaturesReporter(),
	numbering_scheme_(AHO_Scheme)
{
	for ( core::Size i = 1; i <= 6; ++i ) {
		auto cdr = static_cast< CDRNameEnum >(i);
		cdrs_.push_back(cdr);
	}
}

//CDRClusterFeatures::CDRClusterFeatures(CDRClusterFeatures const & src):
//FeaturesReporter(),
//numbering_scheme_(src.numbering_scheme_),
//cdrs_(src.cdrs_)
//{}

CDRClusterFeatures::~CDRClusterFeatures()= default;


void
CDRClusterFeatures::write_schema_to_db(utility::sql_database::sessionOP db_session) const {
	using namespace basic::database::schema_generator;

	Column struct_id("struct_id", utility::pointer::make_shared< DbBigInt >());
	Column chain("chain", utility::pointer::make_shared< DbText >());
	Column CDR("CDR", utility::pointer::make_shared< DbText >());
	Column length("length", utility::pointer::make_shared< DbInteger >());
	Column fullcluster("fullcluster", utility::pointer::make_shared< DbText >());
	Column dis("dis", utility::pointer::make_shared< DbReal >());
	Column normDis("normDis", utility::pointer::make_shared< DbReal >());
	Column normDis_deg("normDis_deg", utility::pointer::make_shared< DbReal >());
	Column resnum_begin("resnum_begin", utility::pointer::make_shared< DbInteger >());
	Column resnum_end("resnum_end", utility::pointer::make_shared< DbInteger >());
	Column sequence("sequence", utility::pointer::make_shared< DbText >());


	Columns primary_keys;

	primary_keys.push_back(struct_id);
	primary_keys.push_back(resnum_begin);
	primary_keys.push_back(resnum_end);

	PrimaryKey primary_key(primary_keys);
	ForeignKey foreign_key(struct_id, "residues", "struct_id", true);

	Schema table("CDR_clusters", primary_key);
	table.add_foreign_key(foreign_key);

	table.add_column(chain);
	table.add_column(CDR);
	table.add_column(length);
	table.add_column(fullcluster);
	table.add_column(dis);
	table.add_column(normDis);
	table.add_column(normDis_deg);
	table.add_column(sequence);
	table.write(db_session);
}

utility::vector1<std::string>
CDRClusterFeatures::features_reporter_dependencies() const {
	utility::vector1<std::string> dependencies;
	dependencies.push_back("StructureFeatures");
	return dependencies;
}

void
CDRClusterFeatures::parse_my_tag(utility::tag::TagCOP tag, basic::datacache::DataMap&) {
	using namespace basic::options;
	using namespace basic::options::OptionKeys;

	cdrs_.clear();
	AntibodyEnumManagerCOP enum_manager( utility::pointer::make_shared< AntibodyEnumManager >() );
	std::string cdrs = tag->getOption< std::string >("cdrs", "L1,L2,L3,H1,H2,H3");
	std::string scheme;

	if ( tag->hasOption("input_ab_scheme") ) {
		scheme = tag->getOption< std::string >("input_ab_scheme");
	} else {
		scheme = option [OptionKeys::antibody::input_ab_scheme]();
	}

	if ( ! enum_manager->numbering_scheme_is_present(scheme) ) {
		throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError,  "Numbering scheme not recognized: "+ scheme);
	}

	set_numbering_scheme( enum_manager->numbering_scheme_string_to_enum(scheme) );

	vector1< std::string > cdrsSP = utility::string_split(cdrs, ',');
	for ( core::Size i = 1; i <= cdrsSP.size(); ++i ) {
		if ( ! enum_manager->cdr_name_is_present(cdrsSP[i]) ) {
			throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "CDR not recognized: " + cdrsSP[i]);
		} else {
			cdrs_.push_back(enum_manager->cdr_name_string_to_enum(cdrsSP[i]));
		}
	}

}

void
CDRClusterFeatures::set_numbering_scheme(AntibodyNumberingSchemeEnum const & numbering_scheme) {
	numbering_scheme_ = numbering_scheme;
}

void
CDRClusterFeatures::set_cdrs_to_use(vector1<CDRNameEnum> cdrs) {
	cdrs_ = cdrs;
}

core::Size
CDRClusterFeatures::report_features(core::pose::Pose const & pose, utility::vector1< bool > const & , StructureID struct_id, utility::sql_database::sessionOP db_session) {
	using cppdb::statement;

	AntibodyInfoOP ab_info( new AntibodyInfo(pose, numbering_scheme_, North) );

	std::string stmt_string = "INSERT INTO CDR_clusters( struct_id, resnum_begin, resnum_end, chain, CDR, length, fullcluster, dis, normDis, normDis_deg, sequence) VALUES (?,?,?,?,?,?,?,?,?,?,?);";
	statement stmt(basic::database::safely_prepare_statement(stmt_string, db_session));

	for ( auto const & cdr : ab_info->get_all_cdrs_present() ) {
		CDRClusterCOP cluster = ab_info->get_CDR_cluster(cdr);

		//Short-circuit evaluation here:
		if ( ab_info->has_cluster_for_cdr(cdr) && (std::find(cdrs_.begin(), cdrs_.end(), cluster->cdr()) != cdrs_.end()) ) {

			// stmt.bind() binds by reference -- need to lifetime preserve any (non-primitive) return-by-value intermediate values
			auto const & sequence = ab_info->get_CDR_sequence_with_stem(cdr, pose, 0, 0, North);
			auto const & cluster_name = ab_info->get_cluster_name(cluster->cluster());
			auto const & cdr_name = ab_info->get_CDR_name(cdr);
			std::stringstream str_chain;
			str_chain << ab_info->get_CDR_chain(cdr);

			stmt.bind(1, struct_id);
			stmt.bind(2, ab_info->get_CDR_start(cdr, pose, North));
			stmt.bind(3, ab_info->get_CDR_end(cdr, pose, North));
			stmt.bind(4, str_chain);
			stmt.bind(5, cdr_name);
			stmt.bind(6, ab_info->get_CDR_length(cdr, pose, North));
			stmt.bind(7, cluster_name );
			stmt.bind(8, cluster->distance());
			stmt.bind(9, cluster->length_normalized_distance());
			stmt.bind(10, cluster->normalized_distance_in_degrees());
			stmt.bind(11, sequence);
			basic::database::safely_write_to_database(stmt);
		}
	}
	return 0;
}

std::string CDRClusterFeatures::type_name() const {
	return class_name();
}

std::string CDRClusterFeatures::class_name() {
	return "CDRClusterFeatures";
}

void CDRClusterFeatures::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	attlist
		+ XMLSchemaAttribute::attribute_w_default( "cdrs", xs_string, "Comma-separated list of CDRs for which to report features", "L1,L2,L3,H1,H2,H3" )
		+ XMLSchemaAttribute( "input_ab_scheme", xs_string, "Numbering scheme for input antibody" );

	protocols::features::xsd_type_definition_w_attributes( xsd, class_name(),
		"Author: Jared Adolf-Bryfogle (jadolfbr@gmail.com)\n"
		"Reports clusters for the specified CDRs", attlist );
}

std::string CDRClusterFeaturesCreator::type_name() const {
	return CDRClusterFeatures::class_name();
}

protocols::features::FeaturesReporterOP
CDRClusterFeaturesCreator::create_features_reporter() const {
	return utility::pointer::make_shared< CDRClusterFeatures >();
}

void CDRClusterFeaturesCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	CDRClusterFeatures::provide_xml_schema( xsd );
}






} //clusters
} //antibody
} //protocols
