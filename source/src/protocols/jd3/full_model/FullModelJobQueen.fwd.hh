// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/jd3/full_model/FullModelJobQueen.fwd.hh
/// @brief  class declaration for FullModelJobQueen
/// @author Andy Watkins (amw579@stanford.edu)

#ifndef INCLUDED_protocols_jd3_full_model_FullModelJobQueen_FWD_HH
#define INCLUDED_protocols_jd3_full_model_FullModelJobQueen_FWD_HH

//utility headers
#include <utility/pointer/owning_ptr.hh>

namespace protocols {
namespace jd3 {
namespace full_model {

class FullModelJobQueen;
class PreliminaryLarvalJob;

typedef utility::pointer::shared_ptr< FullModelJobQueen > FullModelJobQueenOP;
typedef utility::pointer::shared_ptr< FullModelJobQueen const > FullModelJobQueenCOP;

typedef utility::pointer::shared_ptr< PreliminaryLarvalJob > PreliminaryLarvalJobOP;
typedef utility::pointer::shared_ptr< PreliminaryLarvalJob const > PreliminaryLarvalJobCOP;

} // namespace full_model
} // namespace jd3
} // namespace protocols

#endif //INCLUDED_protocols_jd3_full_model_FullModelJobQueen_FWD_HH
