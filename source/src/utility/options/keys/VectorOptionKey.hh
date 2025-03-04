// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   utility/options/keys/VectorOptionKey.hh
/// @brief  Abstract automatic hidden index key for vector-valued options
/// @author Stuart G. Mentzer (Stuart_Mentzer@objexx.com)


#ifndef INCLUDED_utility_options_keys_VectorOptionKey_hh
#define INCLUDED_utility_options_keys_VectorOptionKey_hh


// Unit headers
#include <utility/options/keys/VectorOptionKey.fwd.hh>

// Package headers
#include <utility/options/keys/OptionKey.hh>


#ifdef    SERIALIZATION
// Cereal headers
#include <cereal/access.fwd.hpp>
#include <cereal/types/polymorphic.fwd.hpp>
#endif // SERIALIZATION

namespace utility {
namespace options {


/// @brief Abstract automatic hidden index key for vector-valued options
class VectorOptionKey :
	public OptionKey
{


private: // Types


	typedef  OptionKey  Super;


protected: // Creation


	/// @brief Default constructor
	inline
	VectorOptionKey()
	{}


	/// @brief Copy + identifier constructor
	inline
	VectorOptionKey(
		VectorOptionKey const & key,
		std::string const & id_a,
		std::string const & identifier_a = std::string(),
		std::string const & code_a = std::string()
	) :
		Super( key, id_a, identifier_a, code_a )
	{}


	/// @brief Key constructor
	inline
	explicit
	VectorOptionKey( Key const & key ) :
		Super( key )
	{}


	/// @brief Key + identifier constructor
	inline
	VectorOptionKey(
		Key const & key,
		std::string const & id_a,
		std::string const & identifier_a = std::string(),
		std::string const & code_a = std::string()
	) :
		Super( key, id_a, identifier_a, code_a )
	{}


	/// @brief Identifier constructor
	inline
	explicit
	VectorOptionKey(
		std::string const & id_a,
		std::string const & identifier_a = std::string(),
		std::string const & code_a = std::string()
	) :
		Super( id_a, identifier_a, code_a )
	{}


public: // Creation


	/// @brief Clone this
	VectorOptionKey *
	clone() const override = 0;


	/// @brief Destructor
	inline
	~VectorOptionKey() override {}


public: // Assignment


	/// @brief Key assignment
	inline
	VectorOptionKey &
	operator =( Key const & key )
	{
		assign_Key( key );
		return *this;
	}


public: // Properties


	/// @brief Scalar option key?
	bool
	scalar() const override
	{
		return false;
	}


	/// @brief Vector option key?
	bool
	vector() const override
	{
		return true;
	}


#ifdef    SERIALIZATION
	friend class cereal::access;
public:
	template< class Archive > void save( Archive & arc ) const;
	template< class Archive > void load( Archive & arc );
#endif // SERIALIZATION

}; // VectorOptionKey


} // namespace options
} // namespace utility


#ifdef    SERIALIZATION
CEREAL_FORCE_DYNAMIC_INIT( utility_options_keys_VectorOptionKey )
#endif // SERIALIZATION


#endif // INCLUDED_utility_options_keys_VectorOptionKey_HH
