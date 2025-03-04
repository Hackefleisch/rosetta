// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/membrane/MPLipidAccessibility.hh
/// @brief Mover that computes which residues are lipid accessible and puts that information into the B-factors: 50 is lipid accessible, 0 is lipid INaccessible
/// @author Julia Koehler Leman (julia.koehler1982@gmail.com)

#ifndef INCLUDED_protocols_membrane_MPLipidAccessibility_hh
#define INCLUDED_protocols_membrane_MPLipidAccessibility_hh

// Unit headers
#include <protocols/membrane/MPLipidAccessibility.fwd.hh>
#include <protocols/moves/Mover.hh>

// Protocol headers

// Core headers
#include <core/pose/Pose.fwd.hh>

// Basic/Utility headers
#include <basic/datacache/DataMap.fwd.hh>
#include <utility/tag/Tag.fwd.hh>

#include <utility/vector1.hh> // AUTO IWYU For vector1

namespace protocols {
namespace membrane {

///@brief Mover that computes which residues are lipid accessible and puts that information into the B-factors: 50 is lipid accessible, 0 is lipid INaccessible
class MPLipidAccessibility : public protocols::moves::Mover {

public:

	/////////////////////
	/// Constructors  ///
	/////////////////////

	/// @brief Default constructor
	MPLipidAccessibility();

	/// @brief Copy constructor (not needed unless you need deep copies)
	// MPLipidAccessibility( MPLipidAccessibility const & src );

	/// @brief Destructor (important for properly forward-declaring smart-pointer members)
	~MPLipidAccessibility() override;

	/////////////////////
	/// Mover Methods ///
	/////////////////////

public:

	/// @brief Apply the mover
	void
	apply( core::pose::Pose & pose ) override;

	void
	show( std::ostream & output = std::cout ) const override;

	/// @brief Get the name of the Mover

	///////////////////////////////
	/// Rosetta Scripts Support ///
	///////////////////////////////

	/// @brief parse XML tag (to use this Mover in Rosetta Scripts)
	void
	parse_my_tag(
		utility::tag::TagCOP tag,
		basic::datacache::DataMap & data
	) override;

	//MPLipidAccessibility & operator=( MPLipidAccessibility const & src );

	/// @brief required in the context of the parser/scripting scheme
	protocols::moves::MoverOP
	fresh_instance() const override;

	/// @brief required in the context of the parser/scripting scheme
	protocols::moves::MoverOP
	clone() const override;

	std::string
	get_name() const override;

	static
	std::string
	mover_name();

	static
	void
	provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );

	/////////////////////
	/// Get Methods   ///
	/////////////////////

	/// @brief get angle cutoff
	core::Real get_angle_cutoff() const;

	/// @brief get slice width
	core::Real get_slice_width() const;

	/// @Brief Set slice width via commandline
	void set_slice_width( core::Real width );

	/// @brief get shell radius
	core::Real get_shell_radius() const;

	/// @brief get dist cutoff
	core::Real get_dist_cutoff() const;

	/// @brief get tm_alpha
	bool get_tm_alpha() const;

	/// @brief Is this an alpha helical protein? (no >7 exception)
	bool is_alpha_helical() const;

private: // methods

	/// @brief set defaults
	void set_defaults();

	/// @brief Register Options from Command Line
	void register_options();

	/// @brief Initialize from commandline
	void init_from_cmd();

	/// @brief finalize setup
	void finalize_setup( Pose & pose );

	/// @brief check whether protein is in membrane
	bool protein_in_membrane( Pose & pose );

	/// @brief fill up slice arrays with protein data
	void fill_up_slices( Pose & pose );

	/// @brief compute slice COM
	void compute_slice_com();

private: // data

	/// @brief original data from first implementation
	core::Real angle_cutoff_;
	core::Real slice_width_;
	core::Real shell_radius_;
	core::Real dist_cutoff_;
	bool tm_alpha_;
	bool is_alpha_helical_;

	// define variables, the outer vector goes through the slices
	// inner vector goes through residues in each slice
	utility::vector1< utility::vector1< core::Size > > resi_;
	utility::vector1< utility::vector1< core::Vector > > ca_coord_, cb_coord_;
	utility::vector1< core::Vector > slice_com_;
	utility::vector1< core::Real > slice_zmin_;

	// Tell the compiler that we are not hiding the base
	// function with the parse_my_tag written above
	using protocols::moves::Mover::parse_my_tag;

	// Add a vector to store per atom lipid accessibility information
	utility::vector1< utility::vector1< core::Real > > per_atom_lipid_accessibility_;

};

} //protocols
} //membrane

#endif //INCLUDED_protocols_membrane_MPLipidAccessibility_hh
