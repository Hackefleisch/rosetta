// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/simple_moves/DumpPdb.hh
/// @author Sarel Fleishman (sarelf@u.washington.edu), Jacob Corn (jecorn@u.washington.edu)

#ifndef INCLUDED_protocols_simple_moves_DumpPdb_hh
#define INCLUDED_protocols_simple_moves_DumpPdb_hh

#include <protocols/simple_moves/DumpPdb.fwd.hh>

#include <core/pose/Pose.fwd.hh>
#include <utility/tag/Tag.fwd.hh>
#include <protocols/moves/Mover.hh>
#include <core/scoring/ScoreFunction.fwd.hh>
#include <basic/datacache/DataMap.fwd.hh>



namespace protocols {
namespace simple_moves {

/// @brief what you think
/// this can now be assimilated into DumpPdbMover
class DumpPdb : public protocols::moves::Mover
{
public:
	DumpPdb();
	DumpPdb( std::string const & fname );
	~DumpPdb() override;
	void apply( core::pose::Pose & pose ) override;
	protocols::moves::MoverOP clone() const override {
		return( utility::pointer::make_shared< DumpPdb >( *this ) );
	}
	protocols::moves::MoverOP fresh_instance() const override { return utility::pointer::make_shared< DumpPdb >(); }
	void set_scorefxn( core::scoring::ScoreFunctionOP scorefxn);

	///@brief If set, will use the original pose name in the name. Useful if running xml in parallel.
	void set_use_pose_name( bool use_pose_name){use_pose_name_ = use_pose_name;}

	void tag_time(bool setting) { addtime_ = setting; }
	bool tag_time() const { return addtime_; }

	void parse_my_tag( utility::tag::TagCOP tag, basic::datacache::DataMap & ) override;

	/// @brief Set the filename for writing.
	/// @author Vikram K. Mulligan (vmullig@uw.edu).
	void set_filename( std::string const &filename_in ) { fname_ = filename_in; };

	std::string
	get_name() const override;

	static
	std::string
	mover_name();

	static
	void
	provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );

private:
	std::string fname_;
	/// @brief Dump a scored pdb?
	core::scoring::ScoreFunctionOP scorefxn_;
	/// @brief Add timing information to filename?
	bool addtime_;
	/// @brief Add timing information to filename?

	std::string dir_ = "";
	bool use_pose_name_ = false;
};


} // simple_moves
} // protocols


#endif /*INCLUDED_protocols_moves_DumpPdb_HH*/
