// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/simple_filters/PredictedBurialFnEvaluatorCreator.hh
/// @brief  Header for PredictedBurialFnEvaluatorCreator
/// @author Matthew O'Meara

// Unit Headers
#include <protocols/simple_filters/PredictedBurialFnEvaluatorCreator.hh>

// Package Headers

// Package Headers
#include <protocols/evaluation/PoseEvaluator.fwd.hh>
#include <protocols/evaluation/PoseEvaluator.hh>
#include <protocols/simple_filters/PredictedBurialEvaluator.hh>


// ObjexxFCL Headers

// Utility headers

#include <basic/options/option.hh>
#include <basic/Tracer.hh>

// due to template function


// option key includes
#include <basic/options/option_macros.hh>
#include <basic/options/keys/evaluation.OptionKeys.gen.hh>


#ifdef WIN32
#include <core/scoring/constraints/Constraint.hh>
#endif


static basic::Tracer tr( "protocols.evalution.PredictedBurialFnEvaluatorCreator" );

namespace protocols {
namespace simple_filters {

PredictedBurialFnEvaluatorCreator::~PredictedBurialFnEvaluatorCreator() = default;

void PredictedBurialFnEvaluatorCreator::register_options() {
	using namespace basic::options;
	if ( options_registered_ ) return;
	options_registered_ = true;

	OPT( evaluation::predicted_burial_fn );

}

void PredictedBurialFnEvaluatorCreator::add_evaluators( evaluation::MetaPoseEvaluator & eval ) const {
	using namespace core;
	using namespace basic::options;
	using namespace basic::options::OptionKeys;
	using protocols::evaluation::PoseEvaluatorOP;


	if ( option[ OptionKeys::evaluation::predicted_burial_fn ].user() ) {
		std::string const fn( option[ OptionKeys::evaluation::predicted_burial_fn ]() );
		eval.add_evaluation( utility::pointer::make_shared< PredictedBurialEvaluator >(fn) );
	}

}

std::string PredictedBurialFnEvaluatorCreator::type_name() const {
	return "PredictedBurialFnEvaluatorCreator";
}

} //namespace
} //namespace
