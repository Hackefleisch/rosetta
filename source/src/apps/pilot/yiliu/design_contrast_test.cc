// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   rosetta/pilot/yiliu/sqc_test.cc
///
/// @brief
/// @author Yi Liu


// C++ headers
#include <fstream>

// Unit Headers
#include <utility/excn/Exceptions.hh>
#include <basic/Tracer.hh>
#include <devel/init.hh>
#include <core/types.hh>

// Utility Headers
#include <utility/vector1.hh>
#include <utility/file/FileName.hh>


// Project Headers
#include <core/io/sequence_comparation/DesignContrast.hh>
#include <core/pose/Pose.hh>
#include <basic/options/option.hh>





// option key includes

#include <basic/options/keys/out.OptionKeys.gen.hh>

//Auto Headers
#include <core/import_pose/import_pose.hh>


using namespace core;
using namespace basic;

using namespace basic::options;
using namespace basic::options::OptionKeys;

using utility::vector1;
using utility::file::FileName;
using basic::Error;
using basic::Warning;
static basic::Tracer TR( "apps.pilot.yiliu.DC" );


///////////////////////////////////////////////////////////////////////////////

int main( int argc, char * argv [] )
{
	try {

		using namespace core;
		using namespace core::io;
		using core::import_pose::pose_from_file;

		devel::init(argc, argv);

		std::string out_path;
		vector1 <std::string> in_pdb_names, pdb_codes;
		sequence_comparation::DesignContrast dc;
		vector1 <pose::Pose> native_poses, decoy_poses;

		TR << "search for the path" << std::endl;
		if ( option[out::path::pdb].active() ) {
			out_path = option[out::path::pdb].value();
		} else {
			out_path = option[out::path::pdb].default_value();
		}
		TR << "found the path: " << out_path << std::endl;
		dc.setNames();
		dc.setPdbCodes();
		in_pdb_names = dc.getPdbNames();
		pdb_codes = dc.getPdbCodes();

		for ( Size i=1; i <= pdb_codes.size(); ++i ) {
			pose::Pose single_in_pose, single_out_pose;
			std::string out_pdb_name;
			out_pdb_name = out_path + pdb_codes[i] + "_0001.pdb";
			core::import_pose::pose_from_file(single_in_pose, in_pdb_names[i], core::import_pose::PDB_file);
			core::import_pose::pose_from_file(single_out_pose, out_pdb_name, core::import_pose::PDB_file);
			dc.setNeighbors(single_in_pose);
			native_poses.push_back(single_in_pose);
			decoy_poses.push_back(single_out_pose);
		}

		if ( option [ out::file::design_contrast ].active() ) {
			//  std::string sqc_file = option [ out::file::design_contrast ].default_value();
			std::string sqc_file = option [ out::file::design_contrast ].value();
			std::ofstream sqc;
			sqc.open(sqc_file.c_str());
			for ( Size j=1; j <=  decoy_poses.size(); ++j ) {
				dc.output_sqc_file(native_poses[j], decoy_poses[j], pdb_codes[j], sqc);
			}
		}
	} catch (utility::excn::Exception const & e ) {
		e.display();
		return -1;
	}
}

