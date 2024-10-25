// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/features/ProteinRMSDNoSuperpositionFeatures.cc
/// @brief  report the root mean squared deviation (without superposition) between two poses
/// @author Matthew O'Meara (mattjomeara@gmail.com)
/// @author Kyle Barlow (kb@kylebarlow.com)

// Unit Headers
#include <protocols/features/ProteinRMSDNoSuperpositionFeatures.hh>

// Project Headers

// Platform Headers
#include <basic/Tracer.hh>
#include <core/pose/Pose.hh>
#include <core/types.hh>
#include <core/scoring/rms_util.hh>
#include <core/scoring/rms_util.tmpl.hh>
#include <protocols/rosetta_scripts/util.hh>

// Utility Headers
#include <utility/vector1.hh>
#include <utility/tag/Tag.hh>
#include <utility/sql_database/DatabaseSessionManager.fwd.hh>

// Basic Headers
#include <basic/database/sql_utils.hh>
#include <basic/database/schema_generator/PrimaryKey.hh>
#include <basic/database/schema_generator/ForeignKey.hh>
#include <basic/database/schema_generator/Column.hh>
#include <basic/database/schema_generator/Schema.hh>
#include <basic/database/schema_generator/DbDataType.hh>


// External Headers
#include <cppdb/frontend.h>

// Boost Headers

// C++ Headers
#include <string>

// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/features/feature_schemas.hh>
#include <protocols/features/ProteinRMSDNoSuperpositionFeaturesCreator.hh>


namespace protocols {
namespace features {

using std::string;
using core::Size;
using core::pose::Pose;
using core::pose::PoseCOP;
using core::pose::PoseOP;
using basic::datacache::DataMap;
using utility::vector1;
using utility::sql_database::sessionOP;
using utility::tag::TagCOP;
using cppdb::statement;

static basic::Tracer tr( "protocols.features.ProteinRMSDNoSuperpositionFeatures" );



PoseCOP
ProteinRMSDNoSuperpositionFeatures::reference_pose() const {
	return reference_pose_;
}

void
ProteinRMSDNoSuperpositionFeatures::reference_pose(
	PoseCOP pose
) {
	reference_pose_ = pose;
}

ProteinRMSDNoSuperpositionFeatures::ProteinRMSDNoSuperpositionFeatures(
	PoseCOP reference_pose ) :
	reference_pose_(std::move(reference_pose))
{}

void
ProteinRMSDNoSuperpositionFeatures::write_schema_to_db(
	sessionOP db_session
) const {
	write_protein_rmsd_table_schema(db_session);
}

void
ProteinRMSDNoSuperpositionFeatures::write_protein_rmsd_table_schema(
	sessionOP db_session
) const {
	using namespace basic::database::schema_generator;

	Column struct_id("struct_id", utility::pointer::make_shared< DbBigInt >());
	Column reference_tag("reference_tag", utility::pointer::make_shared< DbText >(255));
	Column protein_CA("protein_CA", utility::pointer::make_shared< DbReal >());
	Column protein_CA_or_CB("protein_CA_or_CB", utility::pointer::make_shared< DbReal >());
	Column protein_backbone("protein_backbone", utility::pointer::make_shared< DbReal >());
	Column protein_backbone_including_O("protein_backbone_including_O", utility::pointer::make_shared< DbReal >());
	Column protein_backbone_sidechain_heavyatom("protein_backbone_sidechain_heavyatom", utility::pointer::make_shared< DbReal >());
	Column heavyatom("heavyatom", utility::pointer::make_shared< DbReal >());
	Column nbr_atom("nbr_atom", utility::pointer::make_shared< DbReal >());
	Column all_atom("all_atom", utility::pointer::make_shared< DbReal >());

	Columns primary_key_columns;
	primary_key_columns.push_back(struct_id);
	primary_key_columns.push_back(reference_tag);
	PrimaryKey primary_key(primary_key_columns);

	Columns foreign_key_columns;
	foreign_key_columns.push_back(struct_id);
	vector1< std::string > reference_columns;
	reference_columns.push_back("struct_id");
	ForeignKey foreign_key(foreign_key_columns, "structures", reference_columns, true);

	Schema table("protein_rmsd_no_superposition", primary_key);
	table.add_foreign_key(foreign_key);
	table.add_column(reference_tag);
	table.add_column(protein_CA);
	table.add_column(protein_CA_or_CB);
	table.add_column(protein_backbone);
	table.add_column(protein_backbone_including_O);
	table.add_column(protein_backbone_sidechain_heavyatom);
	table.add_column(heavyatom);
	table.add_column(nbr_atom);
	table.add_column(all_atom);

	table.write(db_session);
}

utility::vector1<std::string>
ProteinRMSDNoSuperpositionFeatures::features_reporter_dependencies() const {
	utility::vector1<std::string> dependencies;
	dependencies.push_back("StructureFeatures");
	return dependencies;
}

void
ProteinRMSDNoSuperpositionFeatures::parse_my_tag(
	TagCOP const tag,
	basic::datacache::DataMap & data
) {
	runtime_assert(tag->getOption<string>("name") == type_name());

	reference_pose( protocols::rosetta_scripts::legacy_saved_pose_or_input( tag, data, class_name() ) );
}

Size
ProteinRMSDNoSuperpositionFeatures::report_features(
	Pose const & pose,
	vector1< bool > const & relevant_residues,
	StructureID struct_id,
	sessionOP db_session
){
	using namespace core::scoring;

	if ( !reference_pose_ || !reference_pose_->size() ) {
		utility_exit_with_message("No reference pose has been initialized.");
		return 0;
	}

	ObjexxFCL::FArray1D_bool subset_array (relevant_residues.size(), false);
	for ( core::Size i = 1; i <= relevant_residues.size(); ++i ) {
		if ( relevant_residues[i] ) subset_array[i-1] = true;
	}

	std::string statement_string = "INSERT INTO protein_rmsd_no_superposition (struct_id, reference_tag, protein_CA, protein_CA_or_CB, protein_backbone, protein_backbone_including_O, protein_backbone_sidechain_heavyatom, heavyatom, nbr_atom, all_atom) VALUES (?,?,?,?,?,?,?,?,?,?);";
	statement stmt(basic::database::safely_prepare_statement(statement_string,db_session));

	stmt.bind(1,struct_id);
	stmt.bind(2,find_tag(*reference_pose_));
	stmt.bind(3,rmsd_no_super_subset(*reference_pose_, pose, subset_array, is_protein_CA));
	stmt.bind(4,rmsd_no_super_subset(*reference_pose_, pose, subset_array, is_protein_CA_or_CB));
	stmt.bind(5,rmsd_no_super_subset(*reference_pose_, pose, subset_array, is_protein_backbone));
	stmt.bind(6,rmsd_no_super_subset(*reference_pose_, pose, subset_array, is_protein_backbone_including_O));
	stmt.bind(7,rmsd_no_super_subset(*reference_pose_, pose, subset_array, is_protein_sidechain_heavyatom));
	stmt.bind(8,rmsd_no_super_subset(*reference_pose_, pose, subset_array, is_heavyatom));
	stmt.bind(9,rmsd_no_super_subset(*reference_pose_, pose, subset_array, is_nbr_atom));
	stmt.bind(10,rmsd_no_super_subset(*reference_pose_, pose, subset_array, is_heavyatom));

	basic::database::safely_write_to_database(stmt);

	return 0;
}

std::string ProteinRMSDNoSuperpositionFeatures::type_name() const {
	return class_name();
}

std::string ProteinRMSDNoSuperpositionFeatures::class_name() {
	return "ProteinRMSDNoSuperpositionFeatures";
}

void ProteinRMSDNoSuperpositionFeatures::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{

	using namespace utility::tag;
	AttributeList attlist;

	rosetta_scripts::attributes_for_saved_reference_pose( attlist );

	protocols::features::xsd_type_definition_w_attributes( xsd, class_name(), "Record RMSD, the reference ID to which RMSD was computed as well as dummy variables determining what type of protein RMSD was computed; for this FeaturesReporter, no superposition is performed", attlist );
}

std::string ProteinRMSDNoSuperpositionFeaturesCreator::type_name() const {
	return ProteinRMSDNoSuperpositionFeatures::class_name();
}

protocols::features::FeaturesReporterOP
ProteinRMSDNoSuperpositionFeaturesCreator::create_features_reporter() const {
	return utility::pointer::make_shared< ProteinRMSDNoSuperpositionFeatures >();
}

void ProteinRMSDNoSuperpositionFeaturesCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	ProteinRMSDNoSuperpositionFeatures::provide_xml_schema( xsd );
}


} //namesapce
} //namespace
