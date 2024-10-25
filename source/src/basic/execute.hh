// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   src/basic/execute.hh
/// @brief  Various functions to run external executables
/// @author Sergey Lyskov
///
/// Note: this code is placed in to 'basic' level instead of utility because we want to have access to Tracers so verbosity of all external calls could be controlled

#ifndef INCLUDED_basic_execute_hh
#define INCLUDED_basic_execute_hh

#include <string>
#include <vector>

namespace basic {


/// @brief Struct that hold result code + output of external process
struct ExecutionResult {
	int result; // result code, zero if no error and otherwise will contain error code
	std::string output;
};

/// @brief excute provided command_line though shell and return exit_code and output
ExecutionResult execute(std::string const & message, std::string const & command, std::vector<std::string> const & args, bool terminate_on_failure=true, bool silent=false);


} // namespace basic



#endif // INCLUDED_basic_execute_hh
