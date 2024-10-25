// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   ScoreMover.cc
/// @brief  Applies a ScoreFunction to a Pose.  Also has CASP and loop modeling features.
/// @author Monica Berrondo

// unit headers
#include <protocols/simple_moves/ScoreMover.hh>
#include <protocols/simple_moves/ScoreMoverCreator.hh>

// type headers
#include <core/types.hh>

// project headers
#include <basic/options/option.hh>
#include <basic/options/keys/score.OptionKeys.gen.hh>
#include <basic/options/keys/corrections.OptionKeys.gen.hh>
#include <basic/options/keys/evaluation.OptionKeys.gen.hh>
#include <basic/options/keys/loops.OptionKeys.gen.hh>
#include <core/pose/Pose.hh>
#include <core/pose/extra_pose_info_util.hh>
#include <core/pose/datacache/CacheableDataType.hh>
#include <core/scoring/Energies.hh>
#include <core/scoring/rms_util.hh>
#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/ScoreFunctionFactory.hh>
#include <basic/Tracer.hh>

#include <basic/datacache/DiagnosticData.hh>
#include <basic/datacache/BasicDataCache.hh>

#include <core/io/raw_data/ScoreMap.hh>
#include <basic/datacache/DataMap.hh>
#include <protocols/loops/util.hh>
#include <protocols/loops/Loops.hh>

// utility headers
#include <utility>
#include <utility/tag/Tag.hh>

// ObjexxFCL headers

// C++ headers
#include <iostream>
#include <string>

#include <utility/excn/Exceptions.hh>
#include <utility/vector1.hh>
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/moves/mover_schemas.hh>


namespace protocols {
namespace simple_moves {

using namespace utility::tag;
using namespace core;
using namespace basic::options;
using namespace scoring;

static basic::Tracer TR( "protocols.simple_moves.ScoreMover" );




ScoreMover::ScoreMover() :
	moves::Mover( ScoreMover::mover_name() ),
	score_function_( get_score_function() ),
	verbose_(true),
	scorefile_("")
{}

ScoreMover::~ScoreMover() = default;

ScoreMover::ScoreMover(
	std::string const & weights, std::string const & patch /* = "" */
) :
	protocols::moves::Mover( ScoreMover::mover_name() ),
	score_function_(/* 0 */),
	verbose_(true),
	scorefile_("")
{
	// score function setup
	using namespace scoring;
	if ( patch == "" ) {
		score_function_ = ScoreFunctionFactory::create_score_function( weights );
	} else {
		score_function_ = ScoreFunctionFactory::create_score_function( weights, patch );
	}
}

ScoreMover::ScoreMover( ScoreFunctionOP score_function_in ) :
	protocols::moves::Mover( ScoreMover::mover_name() ),
	score_function_(std::move( score_function_in )),
	verbose_(true),
	scorefile_("")
{}

moves::MoverOP ScoreMover::clone() const {
	return utility::pointer::make_shared< ScoreMover >( *this );
}
moves::MoverOP ScoreMover::fresh_instance() const {
	return utility::pointer::make_shared< ScoreMover >();
}

void
ScoreMover::apply( Pose & pose ) {
	using namespace pose;
	//using datacache::CacheableDataType::SCORE_MAP;

	(*score_function_)(pose);

	if ( verbose_ ) {
		/// Now handled automatically.  score_function_->accumulate_residue_total_energies( pose );
		using namespace basic::datacache;
		core::io::raw_data::ScoreMap::add_energies_data_from_scored_pose( pose, score_map_ );
		pose.data().set(core::pose::datacache::CacheableDataType::SCORE_MAP, utility::pointer::make_shared< basic::datacache::DiagnosticData >(score_map_));
		pose.energies().show( TR );
		core::io::raw_data::ScoreMap::print( score_map_, TR );

		score_function_->show(TR, pose);
		TR << std::endl;
	}

	// More decoy quality data (rms, maxsub, GDTM)
	if ( get_native_pose() ) {
		Pose npose = *get_native_pose();
		if ( npose.size() == pose.size() ) {
			setPoseExtraScore( pose, "rms", core::scoring::native_CA_rmsd( *get_native_pose(), pose ) );
			setPoseExtraScore( pose, "allatom_rms", all_atom_rmsd( *get_native_pose(), pose ) );
			setPoseExtraScore( pose, "maxsub", CA_maxsub( *get_native_pose(), pose ) );
			setPoseExtraScore( pose, "maxsub2.0", CA_maxsub( *get_native_pose(), pose, 2.0 ) );
			Real gdtmm, m_1_1, m_2_2, m_3_3, m_4_3, m_7_4;
			gdtmm = CA_gdtmm( *get_native_pose(), pose, m_1_1, m_2_2, m_3_3, m_4_3, m_7_4 );
			setPoseExtraScore( pose, "gdtmm", gdtmm);
			setPoseExtraScore( pose, "gdtmm1_1", m_1_1);
			setPoseExtraScore( pose, "gdtmm2_2", m_2_2);
			setPoseExtraScore( pose, "gdtmm3_3", m_3_3);
			setPoseExtraScore( pose, "gdtmm4_3", m_4_3);
			setPoseExtraScore( pose, "gdtmm7_4", m_7_4);
		}
	}

	using namespace basic::options;
	using namespace basic::options::OptionKeys;
	if ( option[ OptionKeys::loops::loopscores].user() ) { //Option is string containing loops to score
		core::pose::Pose native_pose;
		if ( get_native_pose() ) {
			native_pose = *get_native_pose();
		} else {
			native_pose = pose;
		}
		protocols::loops::Loops my_loops( option[ OptionKeys::loops::loopscores]() );
		protocols::loops::addScoresForLoopParts( pose, my_loops, (*score_function_), native_pose, my_loops.size() );
	}

	if ( option[ OptionKeys::evaluation::score_exclude_res ].user() ) {
		utility::vector1<int> exclude_list = option[ OptionKeys::evaluation::score_exclude_res ];

		setPoseExtraScore( pose, "select_score", score_function_->get_sub_score_exclude_res( pose, exclude_list ) );
	}

}


void ScoreMover::parse_my_tag(
	TagCOP const tag,
	basic::datacache::DataMap & datamap
) {
	if ( tag->hasOption("scorefxn") ) {
		std::string const scorefxn_key( tag->getOption<std::string>("scorefxn") );
		if ( datamap.has( "scorefxns", scorefxn_key ) ) {
			score_function_ = datamap.get_ptr<ScoreFunction>( "scorefxns", scorefxn_key );
		} else {
			throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "ScoreFunction " + scorefxn_key + " not found in basic::datacache::DataMap.");
		}
	}

	if ( tag->hasOption("verbose") ) {
		verbose_ = tag->getOption<bool>("verbose");
	}
}

ScoreFunctionOP ScoreMover::score_function() const {
	return score_function_;
}

void ScoreMover::register_options() {
	using namespace basic::options::OptionKeys;
	option.add_relevant( score::weights          );
	option.add_relevant( score::patch            );
	option.add_relevant( corrections::score::dun10);
	option.add_relevant( score::empty            );
	option.add_relevant( score::fa_max_dis       );
	option.add_relevant( score::fa_Hatr          );
	option.add_relevant( score::no_smooth_etables);
	option.add_relevant( score::output_etables   );
	option.add_relevant( score::rms_target       );
	option.add_relevant( score::ramaneighbors    );
	option.add_relevant( score::optH_weights     );
	option.add_relevant( score::optH_patch       );
	option.add_relevant( basic::options::OptionKeys::loops::loopscores); //requires namespacing b/c protocols::loops also exists
	option.add_relevant( evaluation::score_exclude_res);
	//option.add_relevant( in::file::native        ); // implicit from use of if( get_native_pose())?

}

std::string ScoreMover::get_name() const {
	return mover_name();
}

std::string ScoreMover::mover_name() {
	return "ScoreMover";
}

void ScoreMover::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	attlist
		//Doesn't use parse_score_function
		+ XMLSchemaAttribute(
		"scorefxn", xs_string,
		"Score function to use when scoring this pose. Must be previously defined in the DataMap. Otherwise, we use the default scorefunction.")
		+ XMLSchemaAttribute(
		"verbose", xsct_rosetta_bool,
		"boolean controls a bunch of extra output - pose.energies.show() and something called a jd2:ScoreMap" );

	protocols::moves::xsd_type_definition_w_attributes(
		xsd, mover_name(),
		"This Mover applies the scorefunction to your pose",
		attlist );
}

std::string ScoreMoverCreator::keyname() const {
	return ScoreMover::mover_name();
}

protocols::moves::MoverOP
ScoreMoverCreator::create_mover() const {
	return utility::pointer::make_shared< ScoreMover >();
}

void ScoreMoverCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	ScoreMover::provide_xml_schema( xsd );
}


} // simple_moves
} // protocols
