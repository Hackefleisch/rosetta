// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   test/core/scoring/aa_composition_energy/AACompositionEnergy.cxxtest.hh
/// @brief  Test suite for core::scoring::aa_composition_energy::AACompositionEnergy, an energy term for controlling
/// sequence composition during design.
/// @details See also the core::conformation::symmetry::MirrorSymmetricConformation unit tests.  These have
/// another example of AAComposition being set up from code (with constraints attached to the pose).
/// @author Vikram K. Mulligan (vmullig@uw.edu)

// Test headers
#include <cxxtest/TestSuite.h>
#include <core/scoring/aa_composition_energy/AACompositionEnergySetup.fwd.hh>

// Unit headers


// Package Headers
#include <test/core/init_util.hh>

#include <core/pose/annotated_sequence.hh>



#include <basic/Tracer.hh>

//Auto Headers
#include <utility/vector1.hh>

#include <core/pose/Pose.hh> // AUTO IWYU For Pose
#include <core/scoring/ScoreFunction.hh> // AUTO IWYU For ScoreFunction
#include <core/scoring/annealing/RotamerSets.fwd.hh> // AUTO IWYU For pack, rotamer_set


static basic::Tracer TR("core.scoring.aa_composition_energy.AACompositionEnergy.cxxtest");

// --------------- Test Class --------------- //

// using declarations
using namespace core;
using namespace core::pose;
using namespace core::scoring;
using namespace core::scoring::methods;

using namespace core::pack;
using namespace core::pack::rotamer_set;

class AACompositionEnergyTests_quadratic_interpolation : public CxxTest::TestSuite {

public:

	// --------------- Fixtures --------------- //

	// Shared initialization goes here.
	void setUp() {
	}

	// Shared finalization goes here.
	void tearDown() {
	}

	/// @brief Test linear interpolation of penalties when the FRACT_DELTA_START and FRACT_DELTA_END lines are used.
	/// @details This version uses quadratic tailfunctions.
	void test_interpolation_quadratic_tails() {
		core_init_with_additional_options("-score:aa_composition_setup_file core/scoring/aa_composition_energy/fractdelta_quadratic.comp -out:levels core.scoring.aa_composition_energy.AACompositionEnergy:500");

		if ( TR.visible() ) {
			TR << "Starting test_interpolation_quadratic_tails()." << std::endl;
		}

		// Set up score function
		ScoreFunction scorefxn;
		scorefxn.set_weight( aa_composition, 1 );

		utility::vector1 < core::Real > expected_results;
		expected_results.resize(31);
		expected_results[ 1] = 6.25;
		expected_results[ 2] = 7.592592593;
		expected_results[ 3] = 8.842592593;
		expected_results[ 4] = 10;
		expected_results[ 5] = 10.83333333;
		expected_results[ 6] = 11.66666667;
		expected_results[ 7] = 12.5;
		expected_results[ 8] = 13.33333333;
		expected_results[ 9] = 14.16666667;
		expected_results[10] = 15;
		expected_results[11] = 12.5;
		expected_results[12] = 10;
		expected_results[13] = 7.5;
		expected_results[14] = 5;
		expected_results[15] = 2.5;
		expected_results[16] = 0;
		expected_results[17] = 0.833333333;
		expected_results[18] = 1.666666667;
		expected_results[19] = 2.5;
		expected_results[20] = 3.333333333;
		expected_results[21] = 4.166666667;
		expected_results[22] = 5;
		expected_results[23] = 6.666666667;
		expected_results[24] = 8.333333333;
		expected_results[25] = 10;
		expected_results[26] = 11.66666667;
		expected_results[27] = 13.33333333;
		expected_results[28] = 15;
		expected_results[29] = 17.31481481;
		expected_results[30] = 19.81481481;
		expected_results[31] = 22.5;

		TR << "SEQUENCE\tEXPECTED\tACTUAL" << std::endl;
		for ( core::Size i=0; i<=30; ++i ) {
			std::string seq("");
			for ( core::Size j=1; j<=30; ++j ) {
				if ( j<=i ) seq+="A";
				else seq+="G";
			}
			Pose pose;
			make_pose_from_sequence( pose, seq, "fa_standard");
			TR << seq << "\t" << expected_results[i+1] << "\t" << scorefxn(pose) << std::endl;
			TS_ASSERT_DELTA(scorefxn(pose), expected_results[i+1], 1e-6);
		}

		return;
	}

};
