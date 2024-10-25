// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/jd2/MPIWorkPoolJobDistributor.hh
/// @brief  header for MPIWorkPoolJobDistributor - intended for continuous resamplig jobs  that spawn new jobs based on a pool/archive of
///         structures
/// @author Oliver Lange olange@u.washington.edu

#ifndef INCLUDED_protocols_jd2_archive_NormalizedEvaluatedArchive_hh
#define INCLUDED_protocols_jd2_archive_NormalizedEvaluatedArchive_hh

// Unit headers
#include <protocols/jd2/archive/ArchiveManager.fwd.hh>
#include <protocols/jd2/archive/EvaluatedArchive.hh>
#include <protocols/jd2/archive/VarianceStatisticsArchive.fwd.hh>
//#include <protocols/jd2/archive/NormalizedEvaluatedArchive.fwd.hh>

// Package headers
#include <core/io/silent/silent.fwd.hh>


// Utility headers
#include <core/types.hh>

// C++ headers
#include <string>

#include <utility/vector1.hh>


namespace protocols {
namespace jd2 {
namespace archive {
//class ArchiveManager;


/// @brief Tags used to tag messeges sent by MPI functions used to decide whether a slave is requesting a new job id or
///flagging as job as being a bad input

/// @details This job distributor is meant for running jobs where the machine you are using has a large number of
///processors, the number of jobs is much greater than the number of processors, or the runtimes of the individual jobs
///could vary greatly. It dedicates the head node (whichever processor gets processor rank #0) to handling job requests
///from the slave nodes (all nonzero ranks). Unlike the MPIWorkPartitionJobDistributor, this JD will not work at all
///without MPI and the implementations of all but the interface functions have been put inside of ifdef directives.
///Generally each function has a master and slave version, and the interface functions call one or the other depending
///on processor rank.

class NormalizedEvaluatedArchive : public EvaluatedArchive {
	typedef EvaluatedArchive Parent;
public:

	/// @brief Constructor  and Destructor
	NormalizedEvaluatedArchive( ArchiveManagerAP ptr );
	NormalizedEvaluatedArchive();
	~NormalizedEvaluatedArchive() override;

	/// @brief Archive specific options
	static void register_options();

	void init_from_options();

	void initialize() override;

	/// @brief add decoy to Archive
	/// @detail evaluate decoy and call add_evaluated_structure
	bool add_evaluated_structure(
		core::io::silent::SilentStructOP from_batch,
		core::io::silent::SilentStructOP alternative_decoy,
		Batch const&
	) override;

	/// @brief overloaded that we can sort the pool after reading
	bool restore_from_file() override;

	/// @brief determine variation of scores (those that are non-zeros in select_weights_ )
	bool determine_score_variations() const;

	///overloaded to save / restore the variance_archive_
	void save_to_file( std::string suffix = "" ) override;

	/// @brief recompute all score-values of all decoys and re-order the archive by (new) select_score
	void rescore() override;

	WeightMap const& score_variations() const override;

	core::Real score_variation( std::string const& col ) const override;


protected:

	void invalidate_score_variations() override {
		score_variations_are_clean_ = false;
	}

private:
	bool is_start_zero_score( std::string const& ) const;

	mutable WeightMap score_variations_;
	mutable bool score_variations_are_clean_;
	VarianceStatisticsArchiveOP variance_archive_;

	//option settings
	core::Size min_decoys_for_statistics_;
	core::Size nstruct_for_statistics_;
	bool keep_adding_to_statistics_;
	bool use_variance_archive_;
	core::Real insertion_prob_;
	bool activated_;
	bool lower_quartile_;
	//for these scores we compute the variance as 0...Q3 instead of Q1..Q3
	utility::vector1< std::string > positive_scores_;
	/// @brief keep track whether our options have been registered at start up
	static bool options_registered_;
};


}//archive
}//jd2
}//protocols


#endif //INCLUDED_protocols_jd2_Archive_HH
