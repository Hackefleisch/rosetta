// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file TopologyBroker
/// @brief  top-class (Organizer) of the TopologyBroker mechanism
/// @details responsibilities:
/// @author Oliver Lange

// Unit Headers
#include <protocols/topology_broker/LoopFragmentClaimer.hh>

// Package Headers
#include <protocols/topology_broker/weights/AbinitioMoverWeight.hh>

// Project Headers
//#include <core/pose/Pose.hh>
#include <core/kinematics/MoveMap.hh>

#include <protocols/simple_moves/FragmentMover.hh>

// ObjexxFCL Headers

// Utility headers
//#include <utility/io/izstream.hh>
//#include <utility/io/ozstream.hh>
//#include <utility/io/util.hh>
#include <basic/Tracer.hh>



//#include <basic/options/option.hh>

//// C++ headers

// option key includes


static basic::Tracer tr( "protocols.topo_broker", basic::t_info );

namespace protocols {
namespace topology_broker {

using namespace core;

LoopFragmentClaimer::LoopFragmentClaimer( fragment::FragSetOP frags ) :
	FragmentClaimer( utility::pointer::make_shared< simple_moves::ClassicFragmentMover >( frags, utility::pointer::make_shared< kinematics::MoveMap >() ),
	"Loops", utility::pointer::make_shared< weights::ConstAbinitioMoverWeight >( 0.0 ) )
{
	runtime_assert( frags != nullptr );
}

LoopFragmentClaimer::LoopFragmentClaimer( fragment::FragSetOP frags, std::string label ) :
	FragmentClaimer( utility::pointer::make_shared< simple_moves::ClassicFragmentMover >( frags, utility::pointer::make_shared< kinematics::MoveMap >() ),
	"Loops", utility::pointer::make_shared< weights::ConstAbinitioMoverWeight >( 0.0 ) )
{
	runtime_assert( frags != nullptr );
	set_label( label );
}

fragment::FragSetCOP LoopFragmentClaimer::loop_frags( kinematics::MoveMap& movemap) const {
	//for now I allow only a single FragSet for loop-closing...
	// runtime_assert( frags.empty() );
	//to change this we need to find a way to consolidate the movemaps of two different Movers.
	// I guess correct behaviour would be to have a boolean AND for the maps.
	movemap=*mover().movemap();
	return mover().fragments();
}


} //topology_broker
} //protocols
