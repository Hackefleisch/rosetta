// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/scoring/methods/HolesEnergy.cc
/// @brief  Packing Score
/// @author Will Sheffler


//Unit headers
#include <core/energy_methods/HolesEnergy.hh>
#include <core/energy_methods/HolesEnergyCreator.hh>

#include <core/scoring/packing/PoseBalls.hh>

//Package headers

//#include <core/scoring/ScoringManager.hh>
#include <core/pose/Pose.hh>
#include <core/pose/datacache/CacheableDataType.hh>
#include <basic/datacache/CacheableData.fwd.hh>
#include <basic/datacache/BasicDataCache.hh>
#include <basic/database/open.hh>
#include <core/id/CacheableAtomID_MapVector.hh>
#include <basic/options/option.hh>

#include <core/scoring/packing/compute_holes_score.hh>


#include <basic/options/keys/holes.OptionKeys.gen.hh>

//numeric headers
#include <numeric/xyzVector.hh>

//utility headers

//C++ headers

#include <core/scoring/EnergyMap.hh>

//Auto Headers
#include <core/pose/init_id_map.hh>

namespace core {
namespace energy_methods {


/// @details This must return a fresh instance of the HolesEnergy class,
/// never an instance already in use
core::scoring::methods::EnergyMethodOP
HolesEnergyCreator::create_energy_method(
	core::scoring::methods::EnergyMethodOptions const &
) const {
	return utility::pointer::make_shared< HolesEnergy >();
}

core::scoring::ScoreTypes
HolesEnergyCreator::score_types_for_method() const {
	using namespace core::scoring;
	ScoreTypes sts;
	sts.push_back( holes );
	sts.push_back( holes_decoy );
	sts.push_back( holes_resl );
	sts.push_back( holes_min );
	sts.push_back( holes_min_mean );
	return sts;
}


//////////////////////////////////////////////////////
//@brief
//////////////////////////////////////////////////////
HolesEnergy::HolesEnergy() : parent( utility::pointer::make_shared< HolesEnergyCreator >() )
{
	decoy_params_.read_data_file(basic::database::full_name("scoring/rosettaholes/decoy25.params"));
	resl_params_ .read_data_file(basic::database::full_name("scoring/rosettaholes/resl.params"));
	std::string p = basic::options::option[ basic::options::OptionKeys::holes::minimize ]();
	min_params_  .read_data_file(basic::database::full_name("scoring/rosettaholes/"+p+".params"));
}

//////////////////////////////////////////////////////
//@brief
//////////////////////////////////////////////////////
void
HolesEnergy::finalize_total_energy(
	pose::Pose & pose,
	core::scoring::ScoreFunction const &,
	core::scoring::EnergyMap & totals
) const
{
	core::scoring::packing::PoseBalls pb(pose);

	Real resl_score  = compute_holes_score(pose,pb, resl_params_,false).score;
	Real decoy_score = compute_holes_score(pose,pb,decoy_params_,true ).score;
	Real min_score   = compute_holes_score(pose,pb,  min_params_,true ).score;

	totals[ core::scoring::holes_decoy ] = decoy_score;
	totals[ core::scoring::holes_resl  ] = resl_score;
	totals[ core::scoring::holes_min   ] = min_score;

	decoy_score /= pb.nballs();
	resl_score  /= pb.nballs();
	Real composite_score = 1.0 - (1.0 / (1.0 + exp( -3.768941 * decoy_score - 0.5842765 ) ));
	composite_score = resl_score + 3*composite_score;

	totals[ core::scoring::holes ] = composite_score;
	totals[ core::scoring::holes_decoy ] = decoy_score;
	totals[ core::scoring::holes_resl ] = resl_score;
	totals[ core::scoring::holes_min ] = min_score;
	totals[ core::scoring::holes_min_mean ] = min_score / pb.nballs();
}

void
HolesEnergy::setup_for_derivatives(
	pose::Pose & pose,
	core::scoring::ScoreFunction const &
) const {
	using namespace basic::datacache;
	using namespace id;
	using namespace numeric;
	using basic::datacache::DataCache_CacheableData;

	if ( !pose.data().has( core::pose::datacache::CacheableDataType::HOLES_POSE_INFO ) ) {
		pose.data().set( core::pose::datacache::CacheableDataType::HOLES_POSE_INFO, utility::pointer::make_shared< CacheableAtomID_MapVector >() );
	}
	CacheableDataOP dat( pose.data().get_ptr( core::pose::datacache::CacheableDataType::HOLES_POSE_INFO ) );
	CacheableAtomID_MapVectorOP cachemap = utility::pointer::dynamic_pointer_cast<CacheableAtomID_MapVector>( dat );
	AtomID_Map<xyzVector<Real> > & derivs(cachemap->map());
	core::pose::initialize_atomid_map_heavy_only(derivs,pose);

	/*Real score = */compute_holes_deriv( pose, min_params_, derivs );//.score;
}

void
HolesEnergy::eval_atom_derivative(
	id::AtomID const & aid,
	pose::Pose const & pose,
	kinematics::DomainMap const &,
	core::scoring::ScoreFunction const &,
	core::scoring::EnergyMap const & weights,
	Vector & F1,
	Vector & F2
) const {
	//using namespace core::pose::datacache::CacheableDataType;
	using namespace basic::datacache;
	using namespace id;
	using namespace numeric;
	CacheableDataCOP dat( pose.data().get_const_ptr( core::pose::datacache::CacheableDataType::HOLES_POSE_INFO ) );
	CacheableAtomID_MapVectorCOP cachemap = utility::pointer::dynamic_pointer_cast<CacheableAtomID_MapVector const>( dat );
	AtomID_Map<xyzVector<Real> > const & derivs(cachemap->map());

	if ( aid.rsd() > derivs.size() || aid.atomno() > derivs.n_atom(aid.rsd()) ) {
		return;
	}

	numeric::xyzVector<core::Real> atom_x = pose.xyz(aid);
	numeric::xyzVector<core::Real> const f2( derivs[aid] );
	numeric::xyzVector<core::Real> const atom_y = atom_x - f2;   // a "fake" atom in the direcion of the gradient
	numeric::xyzVector<core::Real> const f1( atom_x.cross( atom_y ) );

	F1 += weights[ core::scoring::holes_min ] * f1;
	F2 += weights[ core::scoring::holes_min ] * f2;
}

core::Size
HolesEnergy::version() const
{
	return 1; // Initial versioning
}


} // scoring
} // core
