// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file core/io/silent/SilentStructFactory.cc
/// @brief Factory for creating various types of silent.
/// @author Greg Taylor <gktaylor@u.washington.edu>

// Unit headers
#include <core/io/silent/SilentStructFactory.hh>

// Package headers
#include <core/io/silent/SilentStruct.hh>
#include <core/io/silent/SilentStructCreator.hh>

// Project headers
#include <core/pose/Pose.hh>
#include <core/pose/util.hh>
#include <core/pose/full_model_info/FullModelInfo.hh>
#include <core/chemical/ResidueType.hh>

// Basic headers
#include <basic/Tracer.hh>

// Utility headers
#include <utility/vector1.hh>

// Boost headers

namespace core {
namespace io {
namespace silent {

static basic::Tracer tr( "core.io.silent" );

/// @details Private constructor insures correctness of singleton.
SilentStructFactory::SilentStructFactory() = default;

void
SilentStructFactory::factory_register( SilentStructCreatorCOP creator )
{
	ss_types_[ creator->keyname() ] = creator;
}

void
SilentStructFactory::show_available_silent_struct_types(
	std::ostream & out
) {
	out << "Available silent struct types:" << std::endl;
	for ( auto const & elem : ss_types_ ) {
		out << "\t" << elem.first << std::endl;
	}
}


bool
SilentStructFactory::has_silent_struct_type(
	std::string const & type_name
) {
	return ss_types_.find(type_name) != ss_types_.end();
}

SilentStructOP
SilentStructFactory::get_silent_struct( std::string const & type_name, SilentFileOptions const & opts )
{
	tr.Trace << "generate silent struct of type " << type_name << std::endl;
	SilentStructCreatorMap::const_iterator iter = ss_types_.find( type_name );
	if ( iter != ss_types_.end() ) {
		return iter->second->create_silent_struct( opts );
	} else {

		using std::string;
		using utility::vector1;
		string msg("SilentStructFactory::get_instance()->get_silent_struct:  ");
		msg += type_name + " does not name a known SilentStructType --> " +
			"check spelling or register new SilentStruct type in SilentStructFactory!";
		msg += "known types are:\n";
		vector1< string > types = get_ss_names();
		for ( string const & type : types ) {
			msg += type + "\n";
		}
		msg += "If you were using binary_rna, switch to binary\n";

		utility_exit_with_message( msg );
	}
	return nullptr;
}

utility::vector1< std::string >
SilentStructFactory::get_ss_names() const {
	using std::string;
	using utility::vector1;

	vector1< string > ss_names;
	for ( auto const & ss_type : ss_types_ ) {
		ss_names.push_back( ss_type.first );
	}

	return ss_names;
}

SilentStructCreatorCOP
SilentStructFactory::get_creator( std::string const & type_name )
{
	SilentStructCreatorMap::const_iterator iter = ss_types_.find( type_name );
	if ( iter != ss_types_.end() ) {
		return iter->second;
	} else {

		using std::string;
		using utility::vector1;
		string msg("SilentStructFactory::get_creator:  ");
		msg += type_name + " does not name a known SilentStructType --> " +
			"check spelling or register new SilentStruct type in SilentStructFactory!";

		msg += "known types are:\n";
		vector1< string > types = get_ss_names();
		for ( string const & elem : types ) {
			msg += elem + "\n";
		}

		utility_exit_with_message( msg );
	}
	return nullptr;
}

SilentStructOP SilentStructFactory::get_silent_struct_in( SilentFileOptions const & opts ) {
	return get_silent_struct( opts.in_silent_struct_type(), opts );
}

SilentStructOP
SilentStructFactory::get_silent_struct_out( SilentFileOptions const & opts )
{
	return get_silent_struct( opts.out_silent_struct_type(), opts );
}

SilentStructOP SilentStructFactory::get_silent_struct_out( core::pose::Pose const& pose, SilentFileOptions const & opts ) {

	bool already_binary( opts.out_silent_struct_type().find( "binary" ) != std::string::npos );
	bool const score_only( opts.out_silent_struct_type() == "score" );
	bool const score_jump( opts.out_silent_struct_type() == "score_jump" );

	// if the user has explicitly set the output type, honor the request
	if ( opts.out_silent_struct_type_set() ) {
		return get_silent_struct( opts.out_silent_struct_type(), opts ) ;
	}

	if ( !opts.binary_output() && pose.residue_type( 1 ).is_RNA() ) {
		return get_silent_struct( "rna", opts );
	}

	// if pose has RNA, OR a full_model_info, can't use protein silent struct.
	if ( core::pose::full_model_info::full_model_info_defined( pose ) ) {
		return get_silent_struct( "binary", opts );
	}

	// if not, decide for the user or use the default
	if ( !score_only && !score_jump && !already_binary && !core::pose::is_ideal_pose(pose) ) {
		std::string binary_string( "binary" );
		if ( opts.out_silent_struct_type() == "rna" ) {
			binary_string = "binary_rna";
		};
		tr.Info << "detected attempt to write non-ideal pose to silent-file..."
			<< "Automatically switching to " << binary_string << " silent-struct type" << std::endl;
		return get_silent_struct( binary_string, opts );
	}

	bool has_RNA = false;
	for ( Size ii = 1; ii <= pose.size(); ++ii ) {
		if ( pose.residue_type( ii ).is_RNA() ) {
			has_RNA = true;
			break;
		}
	}
	if ( has_RNA ) {
		return get_silent_struct( "binary", opts );
	}

	// use default
	return get_silent_struct( opts.out_silent_struct_type(), opts ) ;
}


} // silent
} // io
} // core
