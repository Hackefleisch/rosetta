// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   src/protocols/ligand_docking/scoring_grid/ShapeGrid.hh
/// @author Sam DeLuca

#ifndef INCLUDED_protocols_qsar_scoring_grid_ShapeGrid_hh
#define INCLUDED_protocols_qsar_scoring_grid_ShapeGrid_hh

#include <protocols/qsar/scoring_grid/ShapeGrid.fwd.hh>
#include <protocols/qsar/scoring_grid/SingleGrid.hh>

#include <core/grid/CartGrid.fwd.hh>

#include <numeric/kdtree/KDPointList.hh>

#include <utility/vector1.hh>
#include <utility/tag/XMLSchemaGeneration.fwd.hh>

#include <boost/unordered/unordered_map.hpp>

namespace protocols {
namespace qsar {
namespace scoring_grid {

class ShapeGrid : public SingleGrid
{
public:

	ShapeGrid();
	~ShapeGrid() override;
	/// @brief Make a copy of the grid, respecting the subclassing.
	GridBaseOP clone() const override;
	void refresh(core::pose::Pose const & pose, core::Vector const & center, core::Size const & ) override;
	void refresh(core::pose::Pose const & pose, core::Vector const & center) override;
	void refresh(core::pose::Pose const & pose, core::Vector const & center, utility::vector1<core::Size> ) override;

	void parse_my_tag(utility::tag::TagCOP tag) override;

	/// @brief return the current score of an UltraLightResidue using the current grid
	core::Real score(core::conformation::UltraLightResidue const & residue, core::Real const max_score, qsarMapCOP qsar_map) const override;
	/// @brief return the current score of an atom using the current grid
	core::Real atom_score(core::conformation::UltraLightResidue const & residue, core::Size atomno, qsarMapCOP qsar_map) const override;

	core::Real score(core::conformation::Residue const & residue, core::Real const max_score, qsarMapCOP qsar_map) const override;
	/// @brief return the current score of an atom using the current grid
	core::Real atom_score(core::conformation::Residue const & residue, core::Size atomno, qsarMapCOP qsar_map) const override;

	/// @brief serialize the grid information
	utility::json_spirit::Value serialize() const override;
	/// @brief deserialize the grid information
	void deserialize(utility::json_spirit::mObject data) override;

	static std::string grid_name();
	static void provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );

	std::string hash_fingerprint() const override;

private:

	/// @brief get the appropriate score from the KBP tables
	core::Real get_score_from_angles(std::string const & name3,core::Real distance, core::Real theta, core::Real phi);

	/// @brief given a KDPointList and a set of query coordinates get the score to place in the grid
	core::Real get_point_score(numeric::kdtree::KDPointList const & nearest_residues,core::Vector const &  query_coords);


	/// @brief the KBP data is stored as a compressed json file in the database.  It is read during construction
	void load_kbp_data();

	/// @brief data store for the KBP data.  key is the name3 of the amino acid
	/// This is being done with an unordered map to allow for constant lookup but
	/// still allow for KBP data about NCAAs to be introduced eventually if needed
	typedef utility::pointer::shared_ptr< core::grid::CartGrid<core::Real> > KBPGridOP;
	typedef utility::pointer::shared_ptr< core::grid::CartGrid<core::Real> const > KBPGridCOP;
	boost::unordered_map<std::string, KBPGridCOP > kbp_data_;
	core::Real distance_bin_width_;
	core::Real theta_bin_width_;
	core::Real phi_bin_width_;

};

}
}
}

#endif
