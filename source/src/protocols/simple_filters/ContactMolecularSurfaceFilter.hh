// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/simple_filters/ContactMolecularSurfaceFilter.hh
/// @brief  header file for ContactMolecularSurfaceFilter class
/// @author Longxing Cao <longxing@uw.edu>


#ifndef INCLUDED_protocols_simple_filters_ContactMolecularSurfaceFilter_hh
#define INCLUDED_protocols_simple_filters_ContactMolecularSurfaceFilter_hh

// Unit Headers
#include <protocols/simple_filters/ContactMolecularSurfaceFilter.fwd.hh>

// Package Headers
#include <protocols/filters/Filter.hh>

// Project Headers
#include <core/pose/Pose.fwd.hh>
#include <core/scoring/sc/ContactMolecularSurfaceCalculator.hh>
#include <core/select/residue_selector/ResidueSelector.fwd.hh>

// Utility headers

// Parser headers
#include <basic/datacache/DataMap.fwd.hh>
#include <protocols/filters/Filter.fwd.hh>
#include <utility/tag/Tag.fwd.hh>



//// C++ headers

namespace protocols {
namespace simple_filters {

class ContactMolecularSurfaceFilter : public protocols::filters::Filter {
public:
	typedef protocols::filters::Filter Super;
	typedef protocols::filters::Filter Filter;
	typedef protocols::filters::FilterOP FilterOP;
	typedef core::Real Real;
	typedef core::pose::Pose Pose;

	typedef utility::tag::TagCOP TagCOP;
	typedef basic::datacache::DataMap DataMap;
	typedef core::scoring::sc::ContactMolecularSurfaceCalculator ContactMolecularSurfaceCalculator;

public:// constructor/destructor
	// @brief default constructor
	ContactMolecularSurfaceFilter();

	// @brief constructor with arguments
	ContactMolecularSurfaceFilter( Real const & filtered_area, Real const & distance_weight,
		bool const quick, bool const verbose, bool const use_rosetta_radii=false);

	// @brief constructor with arguments and selectors
	ContactMolecularSurfaceFilter( Real const & filtered_area, Real const & distance_weight,
		bool const quick, bool const verbose,
		core::select::residue_selector::ResidueSelectorCOP const selector1,
		core::select::residue_selector::ResidueSelectorCOP const selector2,
		bool const use_rosetta_radii=false);

	~ContactMolecularSurfaceFilter() override= default;

public:// virtual constructor
	// @brief make clone
	filters::FilterOP clone() const override;

	// @brief make fresh instance
	filters::FilterOP fresh_instance() const override;

public:
	// accessors
	bool quick() const;
	bool verbose() const;
	bool use_rosetta_radii() const;
	bool apolar_target() const;
	Real filtered_area() const;
	Real distance_weight() const;
	Real near_squared_size() const;
	core::select::residue_selector::ResidueSelectorCOP selector1() const;
	core::select::residue_selector::ResidueSelectorCOP selector2() const;

	// mutators
	void quick( bool const quick );
	void verbose( bool const verbose );
	void use_rosetta_radii( bool const use_rosetta_radii );
	void apolar_target( bool const apolar_target );
	void filtered_area( Real const filtered_area );
	void distance_weight( Real const distance_weight );
	void near_squared_size( Real const near_squared_size );
	void selector1( core::select::residue_selector::ResidueSelectorCOP const selector );
	void selector2( core::select::residue_selector::ResidueSelectorCOP const selector );

public:// parser
	void parse_my_tag( TagCOP tag,
		basic::datacache::DataMap & data
	) override;

public:// virtual main operation
	// @brief returns true if the given pose passes the filter, false otherwise.
	// In this case, the test is whether the give pose is the topology we want.
	bool apply( Pose const & pose ) const override;

	/// @brief
	Real report_sm( Pose const & pose ) const override;

public:
	/// @brief calc contact molecular surface, returns results of the ContactMolecularSurfaceCalculator
	/// @param[in] pose Pose to be analyzed
	/// @exception EXCN_CalcInitFailed Thrown if calculator couldn't be initialized
	/// @exception EXCN_ResultsInvalid Thrown if computed results are invalid
	/// @returns ContactMolecularSurfaceCalculator::RESULTS object
	core::Real
	compute( Pose const & pose ) const;

	std::string
	name() const override;

	static
	std::string
	class_name();

	static
	void
	provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );


private:
	/// @brief Uses residue selectors to set up the ContactMolecularSurfaceCalculator
	/// @param[in]  pose Pose to be analyzed
	/// @param[out] scc Initialized, empty ContactMolecularSurfaceCalculator, to which pose residues are added
	void
	setup_from_selectors( Pose const & pose, ContactMolecularSurfaceCalculator & scc ) const;


private:
	Real filtered_area_;
	Real distance_weight_;
	Real near_squared_size_;
	bool apolar_target_;
	bool quick_ = false;
	bool verbose_ = false;
	bool use_rosetta_radii_ = false;
	core::select::residue_selector::ResidueSelectorCOP selector1_;
	core::select::residue_selector::ResidueSelectorCOP selector2_;

};


} // simple_filters
} // protocols

#endif
