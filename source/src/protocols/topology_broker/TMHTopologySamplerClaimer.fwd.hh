// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/topology_broker/TMHTopologySamplerClaimer.fwd.hh
/// @brief  protocols::topology_broker::TMHTopologySamplerClaimer forward declarations header
/// @author Stephanie H. DeLuca stephanie.h.deluca@vanderbilt.edu


#ifndef INCLUDED_protocols_topology_broker_TMHTopologySamplerClaimer_fwd_hh
#define INCLUDED_protocols_topology_broker_TMHTopologySamplerClaimer_fwd_hh


// Utility headers
#include <utility/pointer/access_ptr.hh>
#include <utility/pointer/owning_ptr.hh>


namespace protocols {
namespace topology_broker {

// Forward
class TMHTopologySamplerClaimer;

// Types
typedef  utility::pointer::shared_ptr< TMHTopologySamplerClaimer >  TMHTopologySamplerClaimerOP;
typedef  utility::pointer::shared_ptr< TMHTopologySamplerClaimer const >  TMHTopologySamplerClaimerCOP;

typedef  utility::pointer::weak_ptr< TMHTopologySamplerClaimer >  TMHTopologySamplerClaimerAP;
typedef  utility::pointer::weak_ptr< TMHTopologySamplerClaimer const >  TMHTopologySamplerClaimerCAP;


} // namespace protocols
} // namespace topology_broker

#endif
