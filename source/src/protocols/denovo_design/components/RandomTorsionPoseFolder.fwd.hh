// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/denovo_design/components/RandomTorsionPoseFolder.fwd.hh
/// @brief Folds a pose using random phi/psi torsions
/// @author Tom Linsky (tlinsky@uw.edu)


#ifndef INCLUDED_protocols_denovo_design_components_RandomTorsionPoseFolder_fwd_hh
#define INCLUDED_protocols_denovo_design_components_RandomTorsionPoseFolder_fwd_hh

// Utility headers
#include <utility/pointer/owning_ptr.hh>



// Forward
namespace protocols {
namespace denovo_design {
namespace components {

class RandomTorsionPoseFolder;

typedef utility::pointer::shared_ptr< RandomTorsionPoseFolder > RandomTorsionPoseFolderOP;
typedef utility::pointer::shared_ptr< RandomTorsionPoseFolder const > RandomTorsionPoseFolderCOP;



} //protocols
} //denovo_design
} //components


#endif //INCLUDED_protocols_denovo_design_components_RandomTorsionPoseFolder_fwd_hh





