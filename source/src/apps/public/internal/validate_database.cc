// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file Rocco Moretti
/// @brief Perform validation tests on the Rosetta database, to see if it's internally consistent.


#include <core/pack/dunbrack/RotamerLibrary.hh>

#include <basic/Tracer.hh>
#include <devel/init.hh>

#include <basic/options/option.hh>
#include <basic/options/keys/corrections.OptionKeys.gen.hh>
#include <basic/options/keys/in.OptionKeys.gen.hh>

static basic::Tracer TR( "apps.public.validate_database" );

/// @brief validate the Dunbrack binaries for the current option set.

bool
validate_dunbrack_binaries() {
	using namespace basic::options;
	using namespace basic::options::OptionKeys;

	core::pack::dunbrack::RotamerLibrary* rotamer_library(  core::pack::dunbrack::RotamerLibrary::get_instance() );

	if ( ! rotamer_library->validate_dunbrack_binary() ) {
		TR.Error << "Failure validating the Dunbrack binary" << std::endl;

		TR.Error << "---------------------- Settings: --------------------------------" << std::endl;
		TR.Error << "Database Directory(s): " << std::endl;
		for ( core::Size ii(1); ii <= option[ in::path::database ]().size(); ++ii ) {
			TR.Error << "\t\t" << option[ in::path::database ](ii).name() << std::endl;
		}
		TR.Error << "No binary Dunlib: " << (option[ in::file::no_binary_dunlib ] ? " true " : " false " ) << std::endl;
		TR.Error << "Dun10: " << (option[ corrections::score::dun10 ] ? " true " : " false " ) << std::endl;
		TR.Error << "-correct " << (option[ corrections::correct ] ? " true " : " false " ) << std::endl;
		TR.Error << "-beta " << (option[ corrections::beta ] ? " true " : " false " ) << std::endl;
		TR.Error << "-beta_nov16 " << (option[ corrections::beta_nov16 ] ? " true " : " false " ) << std::endl;
		TR.Error << "-restore_talaris_behavior " << (option[ corrections::restore_talaris_behavior ] ? " true " : " false " ) << std::endl;
		TR.Error << "-shapovalov_lib_fixes_enable " << ( option[ corrections::shapovalov_lib_fixes_enable ] ? " true " : " false " ) << std::endl;
		TR.Error << "-shap_dun10_enable " << ( option[ corrections::shapovalov_lib::shap_dun10_enable ] ? " true " : " false " ) << std::endl;

		if ( option[ corrections::score::dun10 ] ) {
			if ( option[ corrections::shapovalov_lib_fixes_enable ] && option[ corrections::shapovalov_lib::shap_dun10_enable ] ) {
				TR.Error << "Shapovalov fixes directory: " << option[ corrections::shapovalov_lib::shap_dun10_dir ].value() << std::endl;
			} else {
				TR.Error << "Dunbrack 2010 directory: " << option[ corrections::score::dun10_dir ].value() << std::endl;
			}
		} else {
			TR.Error << "Dunbrack 2002 file: " << option[ corrections::score::dun02_file ].value() << std::endl;
		}
		TR.Error << "Dunbrack library binary file: " << rotamer_library->get_binary_name() << std::endl;
		TR.Error << "-----------------------------------------------------------------" << std::endl;

		return false;
	}
	return true;
}

int
main( int argc, char * argv [] )
{
	int exitcode( 0 );

	try {

		// initialize core
		devel::init(argc, argv);

		if ( ! validate_dunbrack_binaries() ) {
			exitcode = -1;
		}

	} catch (utility::excn::Exception const & e ) {
		e.display();
		return -1;
	}
	return exitcode;
}

