// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file src/core/scoring/RMSDConstraint.cc
/// @brief Restrains a residue by its RMSD to a reference residue
/// @author Paul Eisenhuth (eisenhuth451@gmail.com)

#include <basic/Tracer.hh>


namespace core {
    namespace scoring {
        namespace constraints {

            static basic::Tracer TR("core.scoring.constraints");

        }
    }
}