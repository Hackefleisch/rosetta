// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/stepwise/modeler/rna/phosphate/util.hh
/// @brief
/// @details
/// @author Rhiju Das, rhiju@stanford.edu


#ifndef INCLUDED_protocols_stepwise_modeler_rna_phosphate_PhosphateUtil_HH
#define INCLUDED_protocols_stepwise_modeler_rna_phosphate_PhosphateUtil_HH

#include <protocols/stepwise/modeler/rna/phosphate/PhosphateMove.fwd.hh>
#include <core/pose/Pose.fwd.hh>
#include <core/scoring/ScoreFunction.fwd.hh>
#include <core/scoring/methods/EnergyMethodOptions.fwd.hh>
#include <core/types.hh>

namespace protocols {
namespace stepwise {
namespace modeler {
namespace rna {
namespace phosphate {

void
remove_terminal_phosphates( core::pose::Pose & pose );

void
remove_terminal_phosphates( core::pose::Pose & pose, utility::vector1< core::Size > const & res_list  );

void
position_five_prime_phosphate( core::pose::Pose & pose, core::Size const res );

void
copy_over_phosphate_variants( core::pose::Pose & pose,
	core::pose::Pose const & reference_pose,
	utility::vector1< PhosphateMove > const & phosphate_move_list );
void
copy_over_phosphate_variants( core::pose::Pose & pose,
	core::pose::Pose const & reference_pose,
	utility::vector1< core::Size > const & res_list );

core::scoring::ScoreFunctionCOP
get_phosphate_scorefxn();

core::scoring::ScoreFunctionCOP
get_phosphate_scorefxn( core::scoring::methods::EnergyMethodOptions const & options );

} //phosphate
} //rna
} //modeler
} //stepwise
} //protocols

#endif
