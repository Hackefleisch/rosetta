// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/surface_docking/SurfaceVectorLoader.cc
/// @brief
/// @author Michael Pacella (mpacella88@gmail.com)

//unit headers
#include <protocols/surface_docking/SurfaceVectorLoader.hh>
#include <protocols/surface_docking/SurfaceVectorLoaderCreator.hh>

//package headers
#include <protocols/surface_docking/SurfaceParameters.hh>

// basic headers
#include <basic/resource_manager/loader_schemas.hh>

//utility headers
#include <utility/excn/Exceptions.hh>
#include <utility/string_util.hh>
#include <utility/vector1.hh>
#include <utility/tag/XMLSchemaGeneration.hh>
#include <utility/pointer/memory.hh>

// numeric headers
#include <numeric/xyzVector.hh>

//C++ headers
#include <istream>

//external headers
#include <boost/lexical_cast.hpp>

namespace protocols {
namespace surface_docking {

SurfaceVectorLoader::SurfaceVectorLoader() = default;
SurfaceVectorLoader::~SurfaceVectorLoader() = default;

/// @details Takes a locator id and istream, ensures that the correct number of points are present and performs error
/// checking that points are of the correct type before using the points to construct SurfaceParameters
/// @throws EXCN_Msg_Exception
utility::VirtualBaseCOP
SurfaceVectorLoader::create_resource(
	basic::resource_manager::ResourceManager &,
	utility::tag::TagCOP,
	std::string const & input_id,
	std::istream & istream
) const
{
	return create_surface_params( input_id, istream );
}

SurfaceParametersOP
SurfaceVectorLoader::create_surface_params(
	std::string const & input_id,
	std::istream & istream
)
{
	using boost::lexical_cast;
	using core::Real;
	using core::Size;
	using numeric::xyzVector;
	using std::getline;
	using std::string;
	using utility::excn::Exception;
	using utility::split;
	using utility::vector1;

	core::Size const number_of_points(3);
	core::Size const number_of_dimensions(3);

	core::Size lines_read(0);
	vector1< xyzVector<Real> > surf_coords(number_of_points);

	string line;
	while ( getline( istream, line ) ) {
		++lines_read;
		if ( lines_read > number_of_points ) {
			break;
		}

		vector1< string > point_coords ( split( line ) );
		if ( point_coords.size() != number_of_dimensions ) {
			std::ostringstream err;
			err << "The input coordinates must specify a point in three-dimensional space, but the point on line ";
			err << lines_read << " of " << input_id << " has " << point_coords.size() << " coordinates listed.";
			throw CREATE_EXCEPTION(Exception, err.str());
		}
		surf_coords[lines_read] = xyzVector<Real>(lexical_cast<Real>(point_coords[1]),lexical_cast<Real>(point_coords[2]),
			lexical_cast<Real>(point_coords[3]));
	}

	if ( lines_read > number_of_points ) {
		throw CREATE_EXCEPTION(Exception,  "SurfaceVectorLoader expected to be given exactly three points to " \
			"define the periodicity of the surface, but more than three points were provided in "  + input_id + ".");
	} else if ( lines_read < number_of_points ) {
		throw CREATE_EXCEPTION(Exception,  "SurfaceVectorLoader expected to be given exactly three points to " \
			"define the periodicity of the surface, but less than three points were provided in "  + input_id + ".");
	}

	return utility::pointer::make_shared< SurfaceParameters >( surf_coords[1], surf_coords[2], surf_coords[3] );
}

std::string
SurfaceVectorLoader::classname()
{
	return "SurfaceVector";
}


void
SurfaceVectorLoader::provide_xml_schema(
	utility::tag::XMLSchemaDefinition & xsd
)
{
	using namespace utility::tag;
	AttributeList attlist;
	basic::resource_manager::resource_loader_xsd_type_definition_w_attributes( xsd, classname(),
		"The SurfaceVectorLoader reads from the input stream identified by the Resource's 'input_id';"
		" This file should contain three points to define the periodicity of the Surface",
		attlist );
}


basic::resource_manager::ResourceLoaderOP SurfaceVectorLoaderCreator::create_resource_loader() const
{
	return utility::pointer::make_shared< SurfaceVectorLoader >();
}

std::string SurfaceVectorLoaderCreator::loader_type() const
{
	return SurfaceVectorLoader::classname();
}

void SurfaceVectorLoaderCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	return SurfaceVectorLoader::provide_xml_schema( xsd );
}

} // namespace surface_docking
} // namespace protocols
