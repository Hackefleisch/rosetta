// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/frag_picker/quota/QuotaCollector.fwd.hh
/// @brief  Forward declaration for QuotaCollector
/// @author Dominik Gront (dgront@chem.uw.edu.pl)

#ifndef INCLUDED_protocols_frag_picker_quota_QuotaCollector_fwd_hh
#define INCLUDED_protocols_frag_picker_quota_QuotaCollector_fwd_hh

// utility headers
#include <utility/pointer/owning_ptr.hh>

namespace protocols {
namespace frag_picker {
namespace quota {

/// @brief forward declaration for QuotaCollector
class QuotaCollector;

typedef utility::pointer::shared_ptr<QuotaCollector> QuotaCollectorOP;
typedef utility::pointer::shared_ptr<QuotaCollector const>
	QuotaCollectorCOP;

} // quota
} // frag_picker
} // protocols

#endif /* INCLUDED_protocols_frag_picker_quota_QuotaCollector_FWD_HH */
