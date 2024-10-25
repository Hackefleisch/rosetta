// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   apps/pilot/brunette/outputLayerDesignBurial
///
/// @brief  outputs in fasta like format >pdb name burial in format C:core
///       B:boundary  and S:surface, note uses the default SASA values.
//
/// @usage:

/// @author TJ Brunette


// Utility Headers
#include <basic/Tracer.hh>

// Core Headers
#include <core/chemical/util.hh>
#include <core/chemical/ResidueTypeSet.fwd.hh>
#include <core/conformation/Residue.hh>

#include <core/pose/Pose.hh>
#include <core/pose/util.hh>

#include <core/id/AtomID_Map.hh>
#include <core/import_pose/import_pose.hh>
#include <core/import_pose/pose_stream/util.hh>
#include <core/import_pose/pose_stream/MetaPoseInputStream.hh>

#include <core/scoring/sasa.hh>
#include <core/types.hh>
#include <devel/init.hh>
#include <utility/vector1.hh>

//protocols

//basic & utility
#include <utility/io/ozstream.hh>
#include <iostream>
#include <ObjexxFCL/format.hh>

using namespace ObjexxFCL::format;
using utility::vector1;
using core::Size;
using core::Real;
using core::pose::Pose;

static basic::Tracer tr( "ouputSasa" );

vector1< Real> calc_sasa(Pose const & pose, Real pore_radius){
	// define atom_map for main-chain and CB
	core::id::AtomID_Map< bool > atom_map;
	core::pose::initialize_atomid_map( atom_map, pose, false );
	for ( Size ir = 1; ir <= pose.size(); ++ir ) {
		for ( Size j = 1; j<=5; ++j ) {
			core::id::AtomID atom( j, ir );
			atom_map.set( atom, true );
		}
	}
	// calc sasa
	core::id::AtomID_Map< Real > atom_sasa;
	utility::vector1< Real > rsd_sasa;
	core::scoring::calc_per_atom_sasa( pose, atom_sasa, rsd_sasa, pore_radius, false, atom_map );
	return rsd_sasa;
}

std::string calc_burial(Pose const & pose, Real pore_radius, Real boundarySasaThreshold, Real surfaceSasaThreshold){
	std::string burial = "";
	vector1<Real> sasa_score;
	sasa_score = calc_sasa(pose,pore_radius);
	for ( core::uint ii = 1; ii <= pose.size(); ++ii ) {
		if ( sasa_score[ii] < boundarySasaThreshold ) {
			burial+='C';
		} else if ( sasa_score[ii] < surfaceSasaThreshold ) {
			burial+='B';
		} else {
			burial+='S';
		}
	}
	return(burial);
}


int main( int argc, char * argv [] ) {
	try{

		using namespace core::chemical;
		using namespace core::import_pose::pose_stream;
		using core::import_pose::pose_from_file;
		devel::init(argc, argv);
		MetaPoseInputStream input = streams_from_cmd_line();
		ResidueTypeSetCOP rsd_set( rsd_set_from_cmd_line() );
		Real PORE_RADIUS = 2.0;
		while ( input.has_another_pose() ) {
			core::pose::PoseOP input_poseOP;
			input_poseOP = utility::pointer::make_shared< core::pose::Pose >();
			input.fill_pose(*input_poseOP,*rsd_set);
			std::string tag = core::pose::tag_from_pose(*input_poseOP);
			std::string outFile = (tag + ".sasa");
			utility::io::ozstream output(outFile);
			output << ">" << tag << std::endl;
			vector1<Real> sasa =  calc_sasa(*input_poseOP,PORE_RADIUS);
			for ( Size ii=1; ii<=sasa.size(); ++ii ) {
				output << sasa[ii] << std::endl;
			}
			output.close();
			std::cout <<"finished " << tag << std::endl;
		}
	} catch (utility::excn::Exception const & e ) {
		e.display();
		return -1;
	}
	return 0;
}
