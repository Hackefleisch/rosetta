// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   src/core/select/residue_selector/SelectorLogicParser.fwd.hh
/// @brief  Construct selectors as logical combinations of other residue selectors using AND, NOT, and OR
/// @author Andrew Leaver-Fay (aleaverfay@gmail.com)

#ifndef INCLUDED_core_select_residue_selector_SelectorLogicParser_FWD_HH
#define INCLUDED_core_select_residue_selector_SelectorLogicParser_FWD_HH

#include <utility/pointer/owning_ptr.hh>

namespace core {
namespace select {
namespace residue_selector {

class SelectorLogicParser;

typedef utility::pointer::shared_ptr< SelectorLogicParser > SelectorLogicParserOP;
typedef utility::pointer::shared_ptr< SelectorLogicParser const > SelectorLogicParserCOP;

}
}
}

#endif
