// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file src/apps/pilot/nmarze/motif_dock.cc
/// @brief pilot app for testing motif docking score
/// @author Nick Marze (nickmarze@gmail.com)


#include <utility/excn/Exceptions.hh>
//#include <utility/exit.hh>

#include <devel/init.hh>

#include <basic/Tracer.hh>
#include <protocols/moves/Mover.hh>

#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/ScoreFunctionFactory.hh>

#include <protocols/jd2/JobDistributor.hh>
#include <protocols/jd2/Job.fwd.hh>
#include <protocols/jd2/internal_util.hh>

static basic::Tracer TR( "apps.pilot.nmarze.motif_dock" );


class MotifDock : public protocols::moves::Mover {
public:
	// default constructor
	MotifDock(){};
	// destructor
	virtual ~MotifDock(){};

	virtual void apply( core::pose::Pose & pose_in )
	{
		TR << "Applying MotifDock" << std::endl;

		using namespace core;
		using namespace protocols;
		using namespace core::pose;
		using namespace protocols::moves;

		protocols::jd2::JobOP job( protocols::jd2::JobDistributor::get_instance()->current_job() );
		scoring::ScoreFunctionOP motif_dock_score = scoring::ScoreFunctionFactory::create_score_function( "motif_dock_score" );
		(*motif_dock_score)(pose_in);

		TR << "Finished applying MotifDock" << std::endl;

		return;
	} // MotifDock::apply()

	std::string get_name() const { return "MotifDock"; }

	virtual
	protocols::moves::MoverOP
	fresh_instance() const {
		return utility::pointer::make_shared< MotifDock >();
	}

	virtual
	bool
	reinitialize_for_each_job() const { return false; }

	virtual
	bool
	reinitialize_for_new_input() const { return false; }

private:

};

typedef utility::pointer::shared_ptr< MotifDock > MotifDockOP;


/////////////////////////////////////////////////////////////////////////////////
int
main( int argc, char * argv [] )
{
	try {

		protocols::jd2::register_options();

		// initialize core
		devel::init(argc, argv);

		MotifDockOP motif_dock = utility::pointer::make_shared< MotifDock >();
		protocols::jd2::JobDistributor::get_instance()->go( motif_dock );

	} catch ( utility::excn::Exception const & e ) {
		e.display();
		return -1;
	}
	return 0;
}
