// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file core/scoring/mhc_epitope_energy/MHCEpitopePredictorPreLoaded.cc
/// @brief MHC epitope predictor with precomputed values loaded into a peptide->score map
/// @author Chris Bailey-Kellogg, cbk@cs.dartmouth.edu; Brahm Yachnin, brahm.yachnin@rutgers.edu

#include <core/types.hh>
#include <basic/Tracer.hh>
#include <utility/exit.hh>
#include <string>
#include <utility/string_util.hh>
#include <utility/sql_database/DatabaseSessionManager.hh>

#include <core/scoring/ScoringManager.hh>
#include <core/scoring/mhc_epitope_energy/MHCEpitopePredictorPreLoaded.hh>
#include <sys/stat.h> // For getting file size with stat().  If we move to C++17, should switch to std::filesystem::file_size() function

#include <basic/citation_manager/CitationManager.hh>
#include <basic/citation_manager/CitationCollection.hh>

#ifdef    SERIALIZATION
// Utility serialization headers
#include <utility/serialization/serialization.hh>

// Cereal headers
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/map.hpp>
#endif // SERIALIZATION

namespace core {
namespace scoring {
namespace mhc_epitope_energy {

static basic::Tracer TR("core.scoring.mhc_epitope_energy.MHCEpitopePredictorPreLoaded");

using namespace utility::sql_database;
using namespace cppdb;

/// @brief Check for the size of a file and print a warning if appropriate.
void MHCEpitopePredictorPreLoaded::check_file_size( std::string const &filename, core::Size const warn_threshold ) {
	core::Real filesize = 0; // Will be used to store the file's size, in bytes.

	// The following is using the stat() function to get the file's size.  If we switch to C++17, it should be switched to std::filesystem::file_size() to make it platform agnostic.
#ifdef WIN32
	struct _stat file_info; // stat object contains all file info.
	_stat( filename.c_str(), &file_info ); // Get filename's info and store in file_info buffer object.
#else
	struct stat file_info; // stat object contains all file info.
	stat( filename.c_str(), &file_info ); // Get filename's info and store in file_info buffer object.
#endif

	filesize = file_info.st_size; // Put the filesize (in bytes) into filesize.

	// End of C++17 replacement section.

	if ( filesize > warn_threshold ) {
		TR.Warning << "You are attempting to load " << filename << " into memory as a PreLoaded mhc_epitope database." << std::endl;
		TR.Warning << "This file is " << filesize / (1024*1024*1024) << "GB." << std::endl;
		TR.Warning << "Ensure that you have enough memory to support it, or use an External database stored on disk." << std::endl;
		TR.flush();
	}
}

/// @brief Initializes from an sqlite database
void MHCEpitopePredictorPreLoaded::load_database(std::string const &filename)
{
	filetype_ = LOAD_DATABASE;
	filename_ = filename;

	// Get the mhc map and peptide length from the database, returned as a std::pair.
	std::pair<std::map<std::string, core::Real>, core::Size> scores_length( core::scoring::ScoringManager::get_instance()->get_mhc_map_from_db(filename) );

	// Extract those values and store in the appropriate member variables.
	scores_ = scores_length.first;
	set_peptide_length( scores_length.second );
}

void MHCEpitopePredictorPreLoaded::load_csv(std::string const &filename)
{
	filetype_ = LOAD_CSV;
	filename_ = filename;

	// Read the entire file contents from the ScoringManager as a std::list of file lines.  Each line is an element in the list.
	std::list<std::string> csv_contents( core::scoring::ScoringManager::get_instance()->get_mhc_csv_contents(filename) );
	std::list<std::string>::iterator it = csv_contents.begin();

	// TODO: very simple csv parsing (but that's presumably all we need). if is there a robust library, wouldn't hurt to use it instead

	std::string curline(""); //Buffer for current line.
	bool got_header = false;
	while ( it != csv_contents.end() ) {
		curline = csv_contents.front();
		csv_contents.pop_front();
		it = csv_contents.begin(); // Make sure that we still have a valid iterator, pointing to the new first element.

		if ( curline.size() < 1 || curline[0]=='#' ) continue; //Ignore blank lines and comments
		// First real row should be a header; ignore it too
		if ( ! got_header ) {
			got_header = true;
			continue;
		}
		utility::vector1<std::string> cols = utility::string_split(curline, ',');
		if ( cols.size() < 2 ) {
			utility_exit_with_message("ERROR: bad row " + curline + " -- not enough columns");
		}
		std::string peptide = cols[1];
		if ( get_peptide_length() == 0 ) {
			// determine peptide length from first peptide
			set_peptide_length(peptide.size());
		} else if ( peptide.size() != get_peptide_length() ) {
			// check to make sure they're all of same length
			utility_exit_with_message("all peptides should be of the same length");
		}

		core::Real score = utility::string2Real(cols[2]);
		// TODO: check for / handle duplicates? maybe have an option to warn/error
		scores_[peptide] = score;
	}

	if ( get_peptide_length() == 0 ) {
		utility_exit_with_message("ERROR: no peptides in the file");
		// if epitope length remains 0, bad things happen in extracting and scoring peptides, so bail
	}

}

bool MHCEpitopePredictorPreLoaded::operator==(MHCEpitopePredictor const &other)
{
	MHCEpitopePredictorPreLoaded const *o = dynamic_cast<MHCEpitopePredictorPreLoaded const *>(&other);
	if ( !o ) return false;

	if ( o->filename_ != filename_ ) return false; // could be overly strict -- different filenames could have same data, but let's not go there
	if ( o->unseen_score_ != unseen_score_ ) return false;

	return true;
}

std::string MHCEpitopePredictorPreLoaded::report() const
{
	std::stringstream output("");

	// TODO: Any other metadata? If so, hold on to it in connect()
	output << "Map predictor from file '" << filename_ << " containing " << scores_.size() << " peptides; peptide length " << get_peptide_length() << "; unseen score " << unseen_score_;

	return output.str();
}

core::Real MHCEpitopePredictorPreLoaded::score(std::string const &pep)
{
	if ( pep.size() != get_peptide_length() ) {
		TR.Error << "Scoring peptide of size " << pep.size() << " with map expecting peptides of size " << get_peptide_length() << std::endl;
		utility_exit_with_message("MHCEpitopePredicotrMap is trying to score a peptide of the incorrect size!");
	}

	if ( scores_.count(pep)==0 ) return unseen_score_;
	return scores_[pep];
}

/// @brief Provide citations for IEDB.
/// @details If IEDB is being used, add the appropriate citation information
void MHCEpitopePredictorPreLoaded::provide_citation_info(basic::citation_manager::CitationCollectionList & citations ) const {
	if ( utility::endswith(filename_, "iedb_data.db" ) ) {
		using namespace basic::citation_manager;

		// Create a citation collection for this module:
		CitationCollectionOP collection(
			utility::pointer::make_shared< basic::citation_manager::CitationCollection >(
			"MHCEpitopePredictorPreLoaded",
			"MHCEpitopePredictor"
			)
		);

		// Add the relevant citation from the CitationManager to the collection
		collection->add_citation( CitationManager::get_instance()->get_citation_by_doi("10.1093/nar/gky1006" ) );

		// Add the collection to the CitationCollectionList
		citations.add( collection );
	}
}

}//ns mhc_epitope_energy
}//ns scoring
}//ns core

#ifdef    SERIALIZATION

template< class Archive >
void
core::scoring::mhc_epitope_energy::MHCEpitopePredictorPreLoaded::save( Archive & arc ) const {
	arc( cereal::base_class< core::scoring::mhc_epitope_energy::MHCEpitopePredictor >( this ) );
	arc( CEREAL_NVP( scores_ ) ); // std::map< std::string, core::Real >
	arc( CEREAL_NVP( filename_ ) ); // std::string
	arc( CEREAL_NVP( filetype_ ) ); // enum
	arc( CEREAL_NVP( unseen_score_ ) ); // core::Size
}

template< class Archive >
void
core::scoring::mhc_epitope_energy::MHCEpitopePredictorPreLoaded::load( Archive & arc ) {
	arc( cereal::base_class< core::scoring::mhc_epitope_energy::MHCEpitopePredictor >( this ) );
	arc( scores_ ); // std::map< std::string, core::Real >
	arc( filename_ ); // std::string
	arc( filetype_ ); // enum
	arc( unseen_score_ ); // core::Size
}

SAVE_AND_LOAD_SERIALIZABLE( core::scoring::mhc_epitope_energy::MHCEpitopePredictorPreLoaded );
CEREAL_REGISTER_TYPE( core::scoring::mhc_epitope_energy::MHCEpitopePredictorPreLoaded )

CEREAL_REGISTER_DYNAMIC_INIT( core_scoring_mhc_epitope_energy_MHCEpitopePredictorPreLoaded )
#endif // SERIALIZATION
