// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/constraints_additional/ConstraintsEvaluatorCreator.hh
/// @brief  Header for ConstraintsEvaluatorCreator
/// @author Matthew O'Meara

// Unit Headers
#include <protocols/constraints_additional/ConstraintEvaluatorCreator.hh>

// Package Headers

// Package Headers
#include <protocols/evaluation/PoseEvaluator.fwd.hh>
#include <protocols/evaluation/PoseEvaluator.hh>
#include <protocols/constraints_additional/ConstraintEvaluator.hh>

// ObjexxFCL Headers
#include <ObjexxFCL/string.functions.hh>

// Utility headers
#include <utility/vector1.hh>

#include <basic/options/option.hh>
#include <basic/Tracer.hh>

// due to template function

// option key includes
#include <basic/options/option_macros.hh>
#include <basic/options/keys/evaluation.OptionKeys.gen.hh>

#ifdef WIN32
#include <core/scoring/constraints/Constraint.hh>
#endif


static basic::Tracer tr( "protocols.constraints_additional.ConstraintEvaluatorCreator" );

namespace protocols {
namespace constraints_additional {

ConstraintEvaluatorCreator::~ConstraintEvaluatorCreator() = default;

void ConstraintEvaluatorCreator::register_options() {
	using namespace basic::options;
	if ( options_registered_ ) return;
	options_registered_ = true;

	OPT( evaluation::constraints );
	OPT( evaluation::constraints_column );

}

void ConstraintEvaluatorCreator::add_evaluators( evaluation::MetaPoseEvaluator & eval ) const {
	using namespace core;
	using namespace basic::options;
	using namespace basic::options::OptionKeys;
	using protocols::evaluation::PoseEvaluatorOP;


	if ( option[ OptionKeys::evaluation::constraints ].user() ) {
		/*
		this creates Evaluators to evaluate different constraint sets against your decoys
		pls: provide also as many column-names to match your constraint sets
		---
		*/
		utility::vector1< std::string > const& cst_target( option[ OptionKeys::evaluation::constraints ]() );
		utility::vector1< std::string > const& cst_col_name( option[ OptionKeys::evaluation::constraints_column ]() );
		for ( core::Size ct = 1; ct <= cst_target.size(); ct ++ ) {
			std::string tag( ObjexxFCL::string_of( ct ) );
			if ( cst_col_name.size() >= ct ) tag = cst_col_name[ ct ];
			eval.add_evaluation( utility::pointer::make_shared< ConstraintEvaluator >( tag, cst_target[ ct ] ) );
		}
	}

}

std::string ConstraintEvaluatorCreator::type_name() const {
	return "ConstraintEvaluator";
}

} //namespace
} //namespace
