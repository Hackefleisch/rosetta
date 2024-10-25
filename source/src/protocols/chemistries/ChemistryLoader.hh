// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   protocols/chemistries/ChemistryLoader.hh
/// @brief  Declartion of the XML parser's ChemistryLoader class for adding named Chemistrys to the basic::datacache::DataMap
/// @author Rocco Moretti (rmorettiase@gmail.com)

#ifndef INCLUDED_protocols_parser_ChemistryLoader_hh
#define INCLUDED_protocols_parser_ChemistryLoader_hh

// Package Headers
#include <protocols/parser/DataLoader.hh>

// Project Headers
#include <core/pose/Pose.fwd.hh>

// Utility Headers
#include <utility/tag/Tag.fwd.hh>
#include <utility/tag/XMLSchemaGeneration.fwd.hh>

#include <utility/vector1.hh>


namespace protocols {
namespace chemistries {

/// @brief A class for loading arbitrary data into the XML chemistries's basic::datacache::DataMap.
class ChemistryLoader : public parser::DataLoader
{
public:
	ChemistryLoader();
	~ChemistryLoader() override;

	/// @brief The ChemistryLoader will create named Chemistrys and load them into the basic::datacache::DataMap
	void load_data(
		utility::tag::TagCOP tag,
		basic::datacache::DataMap & data
	) const override;

	static std::string loader_name();
	static std::string chemistry_loader_ct_namer( std::string const & element_name );
	static void provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );

};

} //namespace chemistries
} //namespace protocols

#endif
