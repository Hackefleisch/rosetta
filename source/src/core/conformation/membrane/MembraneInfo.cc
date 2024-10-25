// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file     core/conformation/membrane/MembraneInfo.cc
/// @brief    Data describing the relationship between protein(s) and a membrane environment
///
/// @details  MembraneInfo is a container object that describes membrane-protein relationships
///             1. Coordinates of the membrane
///             2. A pointer to MEM which describes relative orientation
///             3. Topology of the transmembrane spans
///             4. Thickness of the membrane
///             5. Interface transition steepness
///
/// @note     This object is a member of Conformation and should only be accessed using
///           pose.conformation().membrane_info(). Do not access MEM outside of the framework!
///
/// @author   Rebecca Alford (ralford3@jhu.edu)

// Unit Headers
#include <core/conformation/membrane/MembraneInfo.hh>

// Project Headers
#include <core/conformation/membrane/ImplicitLipidInfo.hh>
#include <core/conformation/membrane/SpanningTopology.hh>
#include <core/conformation/membrane/MembraneParams.hh>
#include <core/conformation/membrane/MembraneGeometry.hh>
#include <core/conformation/membrane/membrane_geometry/Bicelle.hh>
#include <core/conformation/membrane/membrane_geometry/Slab.hh>
#include <core/conformation/membrane/membrane_geometry/Vesicle.hh>
#include <core/conformation/membrane/membrane_geometry/DoubleVesicle.hh>


// Package Headers
#include <core/conformation/Conformation.hh>

#include <core/kinematics/FoldTree.hh>


#include <core/types.hh>


// Utility Headers
#include <basic/Tracer.hh>
#include <utility/vector1.hh>

// C++ Headers
#include <string>

static basic::Tracer TR( "core.conformation.membrane.MembraneInfo" );

#ifdef SERIALIZATION
// Utility serialization headers
#include <utility/vector1.srlz.hh>
#include <utility/serialization/serialization.hh>


// Cereal headers
#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>
#endif // SERIALIZATION


namespace core {
namespace conformation {
namespace membrane {

/// @brief Create a default version of MembraneInfo (DONT USE)
MembraneInfo::MembraneInfo() :
	thickness_( 0 ),
	steepness_( 0 ),
	membrane_core_( 0 ),
	membrane_rsd_num_( 0 ),
	membrane_jump_( 0 ),
	is_helical_( true ),
	spanning_topology_( nullptr ),
	implicit_lipids_( nullptr )
{}

/// @brief Create MembraneInfo from initialized data
MembraneInfo::MembraneInfo(
	core::Size membrane_pos,
	core::SSize membrane_jump,
	core::Size membrane_core,
	core::Real thickness,
	core::Real steepness,
	SpanningTopologyOP topology
) :
	thickness_( thickness ),
	steepness_( steepness ),
	membrane_core_( membrane_core ),
	membrane_rsd_num_( membrane_pos ),
	membrane_jump_( membrane_jump ),
	is_helical_( true ),
	spanning_topology_( topology ),
	implicit_lipids_( nullptr ),
	per_atom_lipid_accessibility_(),
	membrane_geometry_( utility::pointer::make_shared< membrane_geometry::Slab >( thickness, steepness ) )
{}

/// @brief Create MembraneInfo from initialized data
MembraneInfo::MembraneInfo(
	core::Size membrane_pos,
	core::SSize membrane_jump,
	core::Real steepness,
	SpanningTopologyOP topology,
	std::string lipid_composition_name,
	core::Real lipid_composition_temp
) :
	steepness_( steepness ),
	membrane_core_( 15 ),
	membrane_rsd_num_( membrane_pos ),
	membrane_jump_( membrane_jump ),
	is_helical_( true ),
	spanning_topology_( topology ),
	per_atom_lipid_accessibility_()
{
	implicit_lipids_ = ImplicitLipidInfoOP( new ImplicitLipidInfo( lipid_composition_name, lipid_composition_temp ) );
	thickness_ = implicit_lipids_->water_thickness();
	membrane_geometry_ = MembraneGeometryOP( new membrane_geometry::Slab( thickness, steepness ) );

}


/// @brief Create MembraneInfo from initialized data
MembraneInfo::MembraneInfo(
	core::Size membrane_pos,
	core::SSize membrane_jump,
	core::Real steepness,
	SpanningTopologyOP topology,
	std::string lipid_composition_name,
	core::Real lipid_composition_temp,
	MP_GEOMETRY_TRANSITION mp_geometry,
	Conformation const & conf
) :
	steepness_( steepness ),
	membrane_core_( 15 ),
	membrane_rsd_num_( membrane_pos ),
	membrane_jump_( membrane_jump ),
	is_helical_( true ),
	spanning_topology_( topology ),
	per_atom_lipid_accessibility_()
{
	implicit_lipids_ = ImplicitLipidInfoOP( new ImplicitLipidInfo( lipid_composition_name, lipid_composition_temp ) );
	thickness_ = implicit_lipids_->water_thickness();
	switch ( mp_geometry )
			{
			case MP_GEOMETRY_TRANSITION::SLAB :
				membrane_geometry_ = utility::pointer::make_shared< membrane_geometry::Slab >( steepness, thickness_ );
				TR << "SLAB geometry" << std::endl;
				break;
				//MP_GEOMETRY_TRANSITION::MICELLE == BICELLE == NANODISC so you only need one case for any of these
			case MP_GEOMETRY_TRANSITION::BICELLE :
				membrane_geometry_ = utility::pointer::make_shared< membrane_geometry::Bicelle >( steepness, thickness_, conf, membrane_pos );
				TR << "BICELLE geometry" << std::endl;
				break;
			case MP_GEOMETRY_TRANSITION::VESICLE :
				membrane_geometry_ = utility::pointer::make_shared< membrane_geometry::Vesicle >( steepness, thickness_ );
				TR << "VESICLE geometry" << std::endl;
				break;
			case MP_GEOMETRY_TRANSITION::DOUBLE_VESICLE :
				membrane_geometry_ = utility::pointer::make_shared< membrane_geometry::DoubleVesicle >( steepness, thickness_ );
				TR << "DOUBLE_VESICLE geometry" << std::endl;
				break;
			default :
				membrane_geometry_ = utility::pointer::make_shared< membrane_geometry::Slab >( steepness, thickness_ );
				TR << "SLAB geometry" << std::endl;
				break;
			}

}

MembraneInfo::MembraneInfo(
	core::Size membrane_pos,
	core::SSize membrane_jump,
	core::Size membrane_core,
	core::Real steepness,
	SpanningTopologyOP topology,
	std::string lipid_composition_name,
	core::Real lipid_composition_temp,
	MP_GEOMETRY_TRANSITION mp_geometry,
	Conformation const & conf
) :
	steepness_( steepness ),
	membrane_core_( membrane_core ),
	membrane_rsd_num_( membrane_pos ),
	membrane_jump_( membrane_jump ),
	is_helical_( true ),
	spanning_topology_( topology ),
	per_atom_lipid_accessibility_()
{
	implicit_lipids_ = ImplicitLipidInfoOP( new ImplicitLipidInfo( lipid_composition_name, lipid_composition_temp ) );
	thickness_ = implicit_lipids_->water_thickness();
	switch ( mp_geometry )
			{
			case MP_GEOMETRY_TRANSITION::SLAB :
				membrane_geometry_ = utility::pointer::make_shared< membrane_geometry::Slab >( steepness, thickness_ );
				TR << "SLAB geometry" << std::endl;
				break;
			case MP_GEOMETRY_TRANSITION::BICELLE :
				membrane_geometry_ = utility::pointer::make_shared< membrane_geometry::Bicelle >( steepness, thickness_, conf, membrane_pos );
				TR << "BICELLE geometry" << std::endl;
				break;
			case MP_GEOMETRY_TRANSITION::VESICLE :
				membrane_geometry_ = utility::pointer::make_shared< membrane_geometry::Vesicle >( steepness, thickness_ );
				TR << "VESICLE geometry" << std::endl;
				break;
			case MP_GEOMETRY_TRANSITION::DOUBLE_VESICLE :
				membrane_geometry_ = utility::pointer::make_shared< membrane_geometry::DoubleVesicle >( steepness, thickness_ );
				TR << "DOUBLE_VESICLE geometry" << std::endl;
				break;
			default :
				membrane_geometry_ = utility::pointer::make_shared< membrane_geometry::Slab >( steepness, thickness_ );
				TR << "SLAB geometry" << std::endl;
				break;
			}

}


//Create MembraneInfo from initialized data with membrane geometry transition enum
MembraneInfo::MembraneInfo(
	core::Size membrane_pos,
	core::SSize membrane_jump,
	core::Size membrane_core,
	core::Real thickness,
	core::Real steepness,
	SpanningTopologyOP topology,
	MP_GEOMETRY_TRANSITION mp_geometry,
	Conformation const & conf
) :
	thickness_( thickness ),
	steepness_( steepness ),
	membrane_core_( membrane_core ),
	membrane_rsd_num_( membrane_pos ),
	membrane_jump_( membrane_jump ),
	is_helical_( true ),
	spanning_topology_( topology ),
	per_atom_lipid_accessibility_()
{
	switch ( mp_geometry )
			{
			case MP_GEOMETRY_TRANSITION::SLAB :
				membrane_geometry_ = utility::pointer::make_shared< membrane_geometry::Slab >( steepness, thickness );
				TR << "SLAB geometry" << std::endl;
				break;
				//MP_GEOMETRY_TRANSITION::MICELLE == BICELLE == NANODISC so you only need one case for any of these
			case MP_GEOMETRY_TRANSITION::BICELLE :
				membrane_geometry_ = utility::pointer::make_shared< membrane_geometry::Bicelle >( steepness, thickness, conf, membrane_pos );
				TR << "BICELLE geometry" << std::endl;
				break;
			case MP_GEOMETRY_TRANSITION::VESICLE :
				membrane_geometry_ = utility::pointer::make_shared< membrane_geometry::Vesicle >( steepness, thickness );
				TR << "VESICLE geometry" << std::endl;
				break;
			case MP_GEOMETRY_TRANSITION::DOUBLE_VESICLE :
				membrane_geometry_ = utility::pointer::make_shared< membrane_geometry::DoubleVesicle >( steepness, thickness );
				TR << "DOUBLE_VESICLE geometry" << std::endl;
				break;
			default :
				membrane_geometry_ = utility::pointer::make_shared< membrane_geometry::Slab >( steepness, thickness );
				TR << "SLAB geometry" << std::endl;
				break;
			}
}

//Create MembraneInfo from initialized data with membrane geometry
MembraneInfo::MembraneInfo(
	core::Size membrane_pos,
	core::SSize membrane_jump,
	core::Size membrane_core,
	core::Real thickness,
	core::Real steepness,
	SpanningTopologyOP topology,
	MembraneGeometryCOP membrane_geometry
) :
	thickness_( thickness ),
	steepness_( steepness ),
	membrane_core_( membrane_core ),
	membrane_rsd_num_( membrane_pos ),
	membrane_jump_( membrane_jump ),
	spanning_topology_( topology ),
	per_atom_lipid_accessibility_(),
	membrane_geometry_( membrane_geometry )
{}

/// @brief Create a deep copy of all data in this object.
MembraneInfo::MembraneInfo( MembraneInfo const & src ) :
	utility::VirtualBase(),
	thickness_( src.thickness_ ),
	steepness_( src.steepness_ ),
	membrane_rsd_num_( src.membrane_rsd_num_ ),
	membrane_jump_( src.membrane_jump_ ),
	is_helical_( src.is_helical_ ),
	spanning_topology_( src.spanning_topology_ ),
	implicit_lipids_( src.implicit_lipids_ ),
	per_atom_lipid_accessibility_( src.per_atom_lipid_accessibility_ ),
	membrane_geometry_(src.membrane_geometry_ )
{}

/// @brief create a deep copy of all data in this object upon assignment
MembraneInfo &
MembraneInfo::operator=( MembraneInfo const & src ) {

	// Abort self-assignment.
	if ( this == &src ) {
		return *this;
	}

	// Make a deep copy of everything
	this->thickness_ = src.thickness_;
	this->steepness_ = src.steepness_;
	this->membrane_rsd_num_ = src.membrane_rsd_num_;
	this->membrane_jump_ = src.membrane_jump_;
	this->is_helical_ = src.is_helical_;
	this->spanning_topology_ = src.spanning_topology_;
	this->implicit_lipids_ = src.implicit_lipids_;
	this->per_atom_lipid_accessibility_ = src.per_atom_lipid_accessibility_;
	this->membrane_geometry_ = src.membrane_geometry_;


	return *this;
}

/// @brief Destructor
MembraneInfo::~MembraneInfo() {}

/// @brief Generate a string representation of information represented by ths MembraneInfo
void
MembraneInfo::show() const {
	show( std::cout );
}

void
MembraneInfo::show( std::ostream & output ) const {

	// Show generic membrane info
	output << "MembraneInfo: Information about this Membrane Protein" << std::endl;
	output << "Membrane Residue Num: " << membrane_rsd_num_ << std::endl;
	output << "Membrane Fold Tree Jump: " << membrane_jump_ << std::endl;
	output << "Membrane Thickness: " << thickness_ << std::endl;
	output << "Membrane Steepness: " << steepness_ << std::endl;
	output << "Membrane Spanning Topology " << std::endl;

	// SHow spanning topology object
	spanning_topology_->show( output );

	if ( implicit_lipids_ != nullptr ) {
		output << "Implicit Lipid Information" << std::endl;
		implicit_lipids_->show( output );
	}
}

// Status variables

// Chemical Information about this Membrane

/// @brief Effective thickness of the membrane (default = 15)
core::Real
MembraneInfo::membrane_thickness() const {
	return thickness_;
}

/// @brief Elazar parameter - membrane core (currently defaults to 15)
core::Real
MembraneInfo::membrane_core() const {
	return membrane_core_;
}

/// @brief Steepness of hydrophobic -> hydrophillic transition (default = 15)
core::Real
MembraneInfo::membrane_steepness() const {
	return steepness_;
}

// membrane position & orientation

/// @brief Membrane center
Vector
MembraneInfo::membrane_center( Conformation const & conf ) const  {
	return conf.residue( membrane_rsd_num() ).xyz( membrane::center );
}

/// @brief Membrane normal
Vector
MembraneInfo::membrane_normal( Conformation const & conf ) const {

	Vector normal_tracked = conf.residue( membrane_rsd_num() ).xyz( membrane::normal );
	Vector normal = normal_tracked - membrane_center( conf );

	return normal.normalize();
}

/// @brief Is residue in the membrane?
//based on c-alpha xyz coordinate
bool
MembraneInfo::in_membrane( Conformation const & conf, core::Size resnum ) const {

	bool in_mem( false );

	core::Real f_thk; //transition function without the pore

	//transition function value is based on c-alpha xyz coordinate
	if ( !membrane_geometry_->has_pore() ) {
		f_thk = membrane_geometry_->f_transition( conf, resnum, 2 );
	} else {
		//if there is a pore, we need to calucalte the f_cavity function to solve for
		//the transition function without the pore.
		//f_hydration = f_thk + f_cavity - f_thk*f_cavity
		//f_thk = ( f_hydration - f_cavity ) / ( 1 - f_cavity )
		core::Real f_trans_w_pore = membrane_geometry_->f_transition( conf, resnum, 2 );
		core::Vector const & xyz = membrane_geometry_->corrected_xyz( conf, resnum, 2 );
		core::Real f_cavity = membrane_geometry_->f_cavity( xyz );
		f_thk = ( f_trans_w_pore - f_cavity ) / (1 - f_cavity);
	}

	//since this is based on the transition function without the pore
	//residues facing an aqueous pore, inside a membrane protein will
	//result in in_membrane returning true
	if ( f_thk <= 0.5 ) {
		in_mem = true;
	}


	return in_mem;
} // in membrane?


/// @brief Is atom in the membrane?
bool
MembraneInfo::in_membrane_atom( Conformation const & conf, core::Size resnum, core::ShortSize atomnum ) const {

	bool in_mem( false );

	core::Real f_thk; //transition function without the pore

	if ( !membrane_geometry_->has_pore() ) {
		f_thk = membrane_geometry_->f_transition( conf, resnum, atomnum );
	} else {
		//if there is a pore, we need to calucalte the f_cavity function to solve for
		//the transition function without the pore.
		//f_hydration = f_thk + f_cavity - f_thk*f_cavity
		//f_thk = ( f_hydration - f_cavity ) / ( 1 - f_cavity )
		core::Real f_trans_w_pore = membrane_geometry_->f_transition( conf, resnum, atomnum );
		core::Vector const & xyz = membrane_geometry_->corrected_xyz( conf, resnum, atomnum );
		core::Real f_cavity = membrane_geometry_->f_cavity( xyz );
		f_thk = ( f_trans_w_pore - f_cavity ) / (1 - f_cavity);
	}

	//since this is based on the transition function without the pore
	//residues facing an aqueous pore, inside a membrane protein will
	//result in in_membrane returning true
	if ( f_thk <= 0.5 ) {
		in_mem = true;
	}

	return in_mem;
}


bool
MembraneInfo::use_franklin() const {
	bool use_franklin_transition ( false );

	if ( implicit_lipids_ != nullptr ) {
		use_franklin_transition = true;
	}

	return use_franklin_transition;
}

/// @brief Compute residue position relative to membrane normal
Real
MembraneInfo::residue_z_position( Conformation const & conf, core::Size resnum ) const {

	// Compute z_position
	Vector const & xyz( conf.residue( resnum ).atom( "CA" ).xyz() );

	// membrane normal is normalized to 15, that's why dividing it here by 15
	core::Vector normalized_to_1( membrane_normal( conf ) );
	normalized_to_1.normalize();
	core::Real result = dot( xyz - membrane_center( conf ), normalized_to_1 );
	return result;
}

/// @brief Compute atom position relative to membrane normal
Real
MembraneInfo::atom_z_position( Conformation const & conf, core::Size resnum, core::Size atomnum ) const {

	// Compute z_position
	Vector const & xyz( conf.residue( resnum ).atom( atomnum ).xyz() );

	// membrane normal is normalized to 15, that's why dividing it here by 15
	core::Vector normalized_to_1( membrane_normal( conf ) );
	normalized_to_1.normalize();
	core::Real result = dot( xyz - membrane_center( conf ), normalized_to_1 );
	return result;
}

/// @brief Sequence position of the membrane residue
core::Size
MembraneInfo::membrane_rsd_num() const {
	return membrane_rsd_num_;
}

/// @brief Indeitifier for the membrane jump
core::SSize
MembraneInfo::membrane_jump() const { return membrane_jump_; }

/// @brief Allow a protocol to set a new jump number for the membrane jump
void
MembraneInfo::set_membrane_jump( core::SSize jumpnum ) {

	TR << "Setting a new membrane jump number in MembraneInfo to " << jumpnum << "." << std::endl;
	TR << "Use with caution!" << std::endl;
	membrane_jump_ = jumpnum;
}

/// @brief Somewhat weak check that a membrane foldtree is valid
bool
MembraneInfo::check_membrane_fold_tree( core::kinematics::FoldTree const & ft_in ) const {

	// Check regular fold tree
	if ( ! ft_in.check_fold_tree() ) {
		return false;
	}

	// Check memrbane residue is a jump point
	if ( ! ft_in.is_jump_point( membrane_rsd_num_ ) ) {
		return false;
	}

	// Otherwise, looks reasonable!
	return true;
}


/// @brief Is the protein alpha helical or beta barrel
bool
MembraneInfo::is_helical() const {
	return is_helical_;
}

void
MembraneInfo::is_helical( bool const is_helical ) {
	is_helical_ = is_helical;

	if ( implicit_lipids_ != nullptr ) {
		implicit_lipids_->is_helical( is_helical );
	}
}


// topology of TM spans and lipophilicity

/// @brief Transmembrane spaning topology
/// @details Return a SpanningTopology object, which includes a
/// list of Span objects, describing the start and end sequence
/// positions of each transmembrane span
SpanningTopologyOP
MembraneInfo::spanning_topology() const {
	return spanning_topology_;
}

/// @brief Get implicit lipid information
ImplicitLipidInfoOP
MembraneInfo::implicit_lipids() const {
	return implicit_lipids_;
}

/// @brief Get const membrane_geometry information
MembraneGeometryCOP
MembraneInfo::membrane_geometry() const {
	return membrane_geometry_;
}


/// @brief Per-atom accessibility to membrane lipids (0 = not exposed, 15 = exposed)
core::Real
MembraneInfo::per_atom_lipid_accessibility( core::Size rsd, core::Size atom ) const {
	return per_atom_lipid_accessibility_[ rsd ][ atom ];
}

/// @brief Per-atom accessibility to membrane lipids
void
MembraneInfo::set_per_atom_lipid_accessibility(
	utility::vector1< utility::vector1< core::Real > > const & v ) {
	per_atom_lipid_accessibility_ = v;
}

void
MembraneInfo::set_membrane_geometry( MembraneGeometryCOP mem_geo ) {
	membrane_geometry_ = mem_geo;
}

/// @brief Show MembraneInfo method for pyrosetta
std::ostream & operator << ( std::ostream & os, MembraneInfo const & mem_info )
{

	// Grab a const version of spanning topology
	os << "Membrane residue located at position " << mem_info.membrane_rsd_num();
	os << "Membrane Number of transmembrane spans: " << mem_info.spanning_topology()->nspans() << std::endl;

	return os;
}

} // membrane
} // conformation
} // core

#ifdef    SERIALIZATION

/// @brief Automatically generated serialization method
template< class Archive >
void
core::conformation::membrane::MembraneInfo::save( Archive & arc ) const {
	arc( CEREAL_NVP( thickness_ ) ); // core::Real
	arc( CEREAL_NVP( steepness_ ) ); // core::Real
	arc( CEREAL_NVP( membrane_core_ ) ); // core::Real
	arc( CEREAL_NVP( membrane_rsd_num_ ) ); // core::Size
	arc( CEREAL_NVP( membrane_jump_ ) ); // core::SSize
	arc( CEREAL_NVP( is_helical_ ) ); // bool
	arc( CEREAL_NVP( spanning_topology_ ) ); // SpanningTopologyOP
	arc( CEREAL_NVP( implicit_lipids_ ) ); // ImplicitLipidInfoOP
	arc( CEREAL_NVP( per_atom_lipid_accessibility_ ) ); // utility::vector1< utility::vector1< core::Real >
	arc( CEREAL_NVP( membrane_geometry_ ) ); //MembraneGeometryCOP
}

/// @brief Automatically generated deserialization method
template< class Archive >
void
core::conformation::membrane::MembraneInfo::load( Archive & arc ) {
	arc( thickness_ ); // core::Real
	arc( steepness_ ); // core::Real
	arc( membrane_core_ ); // core::Real
	arc( membrane_rsd_num_ ); // core::Size
	arc( membrane_jump_ ); // core::SSize
	arc( is_helical_ ); // bool
	arc( spanning_topology_ ); // SpanningTopologyOP
	arc( implicit_lipids_ ); // ImplicitLipidInfoOP
	arc( per_atom_lipid_accessibility_ ); // utility::vector1< utility::vector1< core::Real >
	arc( membrane_geometry_ ); // MembraneGeometryCOP

}
SAVE_AND_LOAD_SERIALIZABLE( core::conformation::membrane::MembraneInfo );
CEREAL_REGISTER_TYPE( core::conformation::membrane::MembraneInfo )

CEREAL_REGISTER_DYNAMIC_INIT( core_conformation_membrane_MembraneInfo )
#endif // SERIALIZATION

