// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/docking/EllipsoidalRandomizationMover.fwd.hh
/// @brief forward declarations for EllipsoidalRandomizationMover (docking initial placement mover)
/// @author Nick Marze (nickmarze@gmail.com)

#ifndef INCLUDED_protocols_docking_EllipsoidalRandomizationMover_fwd_hh
#define INCLUDED_protocols_docking_EllipsoidalRandomizationMover_fwd_hh

// Utility headers
#include <utility/pointer/owning_ptr.hh>

namespace protocols {
namespace docking {
/// @brief this mover randomly places one docking partner about an ellipsoid representation of the other
class EllipsoidalRandomizationMover;
typedef utility::pointer::shared_ptr< EllipsoidalRandomizationMover > EllipsoidalRandomizationMoverOP;
typedef utility::pointer::shared_ptr< EllipsoidalRandomizationMover const > EllipsoidalRandomizationMoverCOP;

} //docking
} //protocols

#endif
