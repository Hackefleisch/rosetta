// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file  protocols/backrub/BackrubProtocolTests.cxxtest.hh
/// @brief  Unit tests for the protocol level BackrubProtocol
/// @author Kyle Barlow (kb@kylebarlow.com)


// Test headers
#include <cxxtest/TestSuite.h>

// Project Headers
#include <protocols/backrub/BackrubProtocol.hh>

// Core Headers

// Protocol Headers
#include <basic/Tracer.hh>

#include <core/init_util.hh> // AUTO IWYU For core_init

static basic::Tracer TR("BackrubProtocolTests");


class BackrubProtocolTests : public CxxTest::TestSuite {
	//Define Variables
private:

	protocols::backrub::BackrubProtocolOP test_instantiation_;

public:

	void setUp(){
		core_init();
		test_instantiation_ = utility::pointer::make_shared< protocols::backrub::BackrubProtocol >();
	}

	void tearDown(){
	}

	void test_parse_pivots_from_residue_selector(){
		core::select::residue_selector::ResidueSubset residue_subset;
		residue_subset.push_back( true );
		residue_subset.push_back( false );
		residue_subset.push_back( true );
		residue_subset.push_back( true );
		residue_subset.push_back( false );
		test_instantiation_->set_pivots_from_residue_subset( residue_subset );

		utility::vector1<core::Size> pivot_residues = test_instantiation_->get_pivot_residues();

		TS_ASSERT( pivot_residues.size() == 3 );
		TS_ASSERT( pivot_residues[1] == 1 );
		TS_ASSERT( pivot_residues[2] == 3 );
		TS_ASSERT( pivot_residues[3] == 4 );

	}


};
