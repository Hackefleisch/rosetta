// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   protocols/parser/PackerPaletteLoader.hh
/// @brief  Declartion of the XML parser's PackerPaletteLoader class for adding named PackerPalettes to the basic::datacache::DataMap
/// @author Vikram K. Mulligan (vmullig@uw.edu).

#ifndef INCLUDED_protocols_parser_PackerPaletteLoader_hh
#define INCLUDED_protocols_parser_PackerPaletteLoader_hh

// Package Headers
#include <protocols/parser/DataLoader.hh>

// Project Headers

// Utility Headers
#include <utility/tag/Tag.fwd.hh>
#include <utility/tag/XMLSchemaGeneration.fwd.hh>

#include <string>

namespace protocols {
namespace parser {

/// @brief A class for loading arbitrary data into the XML parser's basic::datacache::DataMap.
class PackerPaletteLoader : public DataLoader
{
public:
	PackerPaletteLoader();
	~PackerPaletteLoader() override;

	/// @brief The PackerPaletteLoader will create named PackerPalettes and load them into the basic::datacache::DataMap.
	///
	void load_data(
		utility::tag::TagCOP tag,
		basic::datacache::DataMap & data
	) const override;

	static std::string loader_name();
	static void provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );

};

} //namespace parser
} //namespace protocols

#endif
