// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file    core/io/pdb/alt_codes_io.hh
/// @brief   Database input/output function definitions for alternative PDB 3-letter-code data.
/// @author  Labonte <JWLabonte@jhu.edu>


// Project header

// Unit header
#include <core/io/alt_codes_io.hh>

// Utility header
#include <utility/io/util.hh>

// Basic header
#include <basic/Tracer.hh>

// C++ header
#include <sstream>


// Construct tracer.
static basic::Tracer TR( "core.io.pdb.alt_codes_io" );


namespace core {
namespace io {

// Return a mapping of alternative PDB 3-letter codes to a paired set including the Rosetta 3-letter code and,
// optionally, any HETNAM information for the residue desired.
AltCodeMap
read_alternative_3_letter_codes_from_database_file( std::string const & filename )
{
	using namespace std;
	using namespace utility;
	using namespace utility::io;

	vector1< string > const lines( get_lines_from_file_data( filename ) );
	AltCodeMap code_map;

	for ( auto line : lines ) {
		remove_inline_comments( line );
		istringstream line_word_by_word( line );

		string key;  // The map key is an alternative 3-letter code.
		string name3;  // This is the Rosetta 3-letter code.
		string hetnam( "" );  // This is the (optional) default HETNAM information.
		char connectivity( '\0' );  // This is the (optional) default main-chain connectivity.
		utility::vector1< std::string > patches;  // This is an (optional) list of patches for residue modifications.

		// The first two columns must be present.
		line_word_by_word >> key >> name3;
		if ( line_word_by_word.fail() ) { continue; }

		// The next columns are optional.
		line_word_by_word >> hetnam;
		if ( ! line_word_by_word.fail() ) {
			line_word_by_word >> connectivity;
			if ( ! line_word_by_word.fail() ) {
				std::string patch;
				line_word_by_word >> patch;
				while ( ! line_word_by_word.fail() ) {
					patches.push_back( patch );
					line_word_by_word >> patch;
				}
			}
		}

		code_map[ key ] = make_tuple( name3, hetnam, connectivity, patches );
	}


	if ( TR.Debug.visible() ) {
		TR.Debug << "Read " << code_map.size() << " 3-letter code mappings from " << filename << '.' << endl;
	}

	return code_map;
}

}  // namespace io
}  // namespace core
