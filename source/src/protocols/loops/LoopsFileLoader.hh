// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/loops/LoopsFileLoader.hh
/// @brief
/// @author

#ifndef INCLUDED_protocols_loops_LoopsFileLoader_HH
#define INCLUDED_protocols_loops_LoopsFileLoader_HH

//unit headers
#include <protocols/loops/LoopsFileLoader.fwd.hh>

//project headers
#include <basic/resource_manager/ResourceLoader.hh>
#include <basic/resource_manager/types.hh>

//utility headers
#include <utility/tag/Tag.fwd.hh>
#include <utility/tag/XMLSchemaGeneration.fwd.hh>

//C++ headers
#include <iosfwd>

namespace protocols {
namespace loops {

/// @brief %LoopsFileLoader constructs a LoopsFileData instance from data provided by the %ResourceManager.
/// @details The %LoopsFileLoader is given an istream containing the information in a %LoopsFile from the
/// ResourceManager.  This information is then processed by an instance of LoopsFileIO to produce a LoopsFileDataOP to
/// return to the protocol.
class LoopsFileLoader : public basic::resource_manager::ResourceLoader
{
public:
	/// @brief Construct the %LoopsFileLoader.
	LoopsFileLoader();

	/// @brief Destructor.
	~LoopsFileLoader() override;

	/// @brief Return a LoopsFileDataOP constructed from the given input stream (istream).

	basic::resource_manager::ResourceCOP
	create_resource(
		basic::resource_manager::ResourceManager & resource_manager,
		utility::tag::TagCOP resource_tag,
		std::string const & input_id,
		std::istream & input_stream
	) const override;

	static
	std::string
	classname();

	static
	void
	provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );


};

} // namespace loops
} // namespace protocols

#endif //INCLUDED_protocols_loops_LoopsFileLoader_HH
