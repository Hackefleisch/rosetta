// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file src/protocols/filters/Filter.hh
/// @brief header file for Filter base class
/// @details
///
///
///
/// @author Florian Richter, floric@u.washington.edu (feb 09 ), Sarel Fleishman sarelf@u.washington.edu

#ifndef INCLUDED_protocols_filters_Filter_hh
#define INCLUDED_protocols_filters_Filter_hh

// Unit Headers
#include <protocols/filters/Filter.fwd.hh>

// Package Headers
#include <basic/datacache/DataMap.fwd.hh>
#include <basic/citation_manager/CitationCollectionBase.fwd.hh>

// Project Headers
#include <core/types.hh>
#include <core/pose/Pose.fwd.hh>

// Utility headers
#include <utility/VirtualBase.hh>
#include <utility/vector1.hh>
#include <utility/tag/Tag.fwd.hh>
#include <utility/tag/XMLSchemaGeneration.fwd.hh> // DO NOT AUTO-REMOVE -- Not used in this header, but will be by practically every other Filter

// C++ headers
#include <string>

#ifdef WIN32
#include <utility/tag/Tag.hh>
#endif

namespace protocols {
namespace filters {

class Filter : public utility::VirtualBase {
public:
	Filter();
	Filter( std::string const & );
	~Filter() override;
	// allows reporting of filter values to a stream
	virtual void report( std::ostream &, core::pose::Pose const & ) const {}

	/// @brief used to report filter internals through a score or silent file
	// to determine that derived class has not overridden }
	virtual core::Real report_sm( core::pose::Pose const & ) const { return -99999; }

	virtual std::string get_type() const{ return type_; }
	std::string get_user_defined_name() const { return user_defined_name_; }
	void set_user_defined_name( std::string const & name ) { user_defined_name_ = name; };

	/// @brief used to clear internal variables if needed. Using fresh_instance is preferred since it's a pure virtual
	virtual void clear() {};


	/// @brief Called by FilterFactory when constructing new Filter. Takes care of the specific mover's parsing.
	virtual
	void parse_my_tag(
		utility::tag::TagCOP,
		basic::datacache::DataMap &
	);

	virtual FilterOP clone() const = 0;

	virtual FilterOP fresh_instance() const = 0;

	/// @brief Returns true if the given pose passes the filter, false otherwise.
	virtual bool apply( core::pose::Pose const & pose ) const = 0;

	virtual core::Real score( core::pose::Pose & pose);

	virtual
	std::string name() const { return "BaseFilter"; };

public: //Functions needed for the citation manager

	/// @brief Provide citations to the passed CitationCollectionList
	/// Subclasses should add the info for themselves and any other classes they use.
	/// @details The default implementation of this function does nothing.  It may be
	/// overriden by filters wishing to provide citation information.
	virtual void provide_citation_info(basic::citation_manager::CitationCollectionList & ) const;

private:
	std::string const type_;
	std::string user_defined_name_;
protected:
	std::string scorename_; // what part of "no protected data" is unclear?
};


/// @brief Wrapper-class that contains a vector1 of Filters
/// @brief apply function returns true if all member filters return true
class FilterCollection : public utility::VirtualBase {
public:

	~FilterCollection() override;

	/// @brief Returns true if the given pose passes all filters, false otherwise.
	bool apply( core::pose::Pose const & pose ) const;

	void report( std::ostream & out, core::pose::Pose const & pose ) const;

	FilterCOP
	get_filter( core::Size i ) { return filters_[i]; }

	void
	add_filter( FilterCOP filter_in ){
		filters_.push_back( filter_in ); }

	void
	remove_last_filter() {
		filters_.pop_back();
	}

	void
	clear(){
		filters_.clear(); }

	core::Size
	size() { return filters_.size(); }

private:

	utility::vector1< FilterCOP > filters_;

};

} // filters
} // protocols

#endif
