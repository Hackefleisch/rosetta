// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.
/// @brief
/// @author jk

// Project Headers
#include <devel/init.hh>
#include <core/pose/Pose.hh>
#include <basic/MetricValue.hh>

#include <basic/options/util.hh>
#include <basic/options/option.hh>
#include <basic/options/keys/pose_metrics.OptionKeys.gen.hh>
#include <basic/options/option_macros.hh>

#include <core/pose/metrics/CalculatorFactory.hh>
#include <core/pose/metrics/simple_calculators/SasaCalculatorLegacy.hh>
#include <protocols/simple_pose_metric_calculators/NumberHBondsCalculator.hh>
#include <protocols/pose_metric_calculators/PackstatCalculator.hh>
#include <protocols/simple_pose_metric_calculators/BuriedUnsatisfiedPolarsCalculator.hh>

#include <core/conformation/Residue.hh>
#include <core/scoring/sasa.hh>

// Utility Headers
#include <utility/vector1.hh>

// C++ Headers
#include <iostream>
#include <string>

//Auto Headers
#include <core/import_pose/import_pose.hh>
#include <core/scoring/ScoreFunction.fwd.hh>
#include <core/scoring/ScoreFunctionFactory.hh>

using namespace core;
using namespace core::pose::datacache;
using namespace core::pose::metrics;
using namespace core::scoring;
using namespace core::scoring::constraints;
using namespace core::id;
using namespace basic::options;
using namespace basic::options::OptionKeys;
using namespace conformation;

OPT_KEY( Real, cst_force_constant )
OPT_KEY( Boolean, print_init )
OPT_KEY( Boolean, print_unbound )
OPT_KEY( Boolean, print_complex )
OPT_KEY( String, input_bound_pdb )
OPT_KEY( String, input_unbound_pdb )
OPT_KEY( String, input_ligand_pdb )

static basic::Tracer TR( "apps.pilot.ragul_ligand_sasa.main" );


int main( int argc, char * argv [] ){
	try{

		NEW_OPT( cst_force_constant, "coordinate constraint force constant", 0.5 );
		NEW_OPT( print_init, "print the initial complex for debugging", true );
		NEW_OPT( print_unbound, "print the mimized protein for debugging", true );
		NEW_OPT( print_complex, "print the minimized complex", true );
		NEW_OPT( input_bound_pdb, "bound protein-ligand file name", "bound.pdb" );
		NEW_OPT( input_unbound_pdb, "unbound protein file name", "unbound.pdb" );
		NEW_OPT( input_ligand_pdb, "unbound ligand file name", "ligand.pdb" );

		devel::init(argc, argv);

		std::string const bound_protein = option[ input_bound_pdb ];
		std::string const unbound_protein = option[ input_unbound_pdb ];
		std::string const input_ligand = option[ input_ligand_pdb ];

		//setup scorefxn
		scoring::ScoreFunctionOP scorefxn(get_score_function());
		scoring::ScoreFunctionOP repack_scorefxn(get_score_function());

		//Register calculators
		std::string sasa_calc_name = "sasa";
		std::string hbond_calc_name = "hbond";
		std::string packstat_calc_name = "packstat";
		std::string burunsat_calc_name = "burunsat";
		core::pose::metrics::PoseMetricCalculatorOP sasa_calculator( new core::pose::metrics::simple_calculators::SasaCalculatorLegacy );
		core::pose::metrics::CalculatorFactory::Instance().register_calculator( sasa_calc_name, sasa_calculator );

		core::pose::metrics::PoseMetricCalculatorOP hb_calc( new protocols::simple_pose_metric_calculators::NumberHBondsCalculator() );
		core::pose::metrics::CalculatorFactory::Instance().register_calculator( hbond_calc_name, hb_calc );

		core::pose::metrics::PoseMetricCalculatorOP packstat_calc( new protocols::pose_metric_calculators::PackstatCalculator() );
		core::pose::metrics::CalculatorFactory::Instance().register_calculator( packstat_calc_name, packstat_calc );

		core::pose::metrics::PoseMetricCalculatorOP burunsat_calc( new protocols::simple_pose_metric_calculators::BuriedUnsatisfiedPolarsCalculator(sasa_calc_name, hbond_calc_name) );
		core::pose::metrics::CalculatorFactory::Instance().register_calculator( burunsat_calc_name, burunsat_calc );

		pose::Pose bound_protein_pose, unbound_protein_pose, ligand_pose;
		core::import_pose::pose_from_file( bound_protein_pose, bound_protein , core::import_pose::PDB_file);
		core::import_pose::pose_from_file( unbound_protein_pose, unbound_protein , core::import_pose::PDB_file);
		core::import_pose::pose_from_file( ligand_pose, input_ligand , core::import_pose::PDB_file);
		basic::MetricValue<Real> total_sasa_mval;
		core::Real ligand_pose_sasa = 0.0, bound_pose_sasa = 0.0, unbound_pose_sasa = 0.0, Total_pose_exposed_SASA = 0.0;
		ligand_pose.metric(sasa_calc_name,"total_sasa",total_sasa_mval);
		ligand_pose_sasa = total_sasa_mval.value();
		bound_protein_pose.metric(sasa_calc_name,"total_sasa",total_sasa_mval);
		bound_pose_sasa = total_sasa_mval.value();
		unbound_protein_pose.metric(sasa_calc_name,"total_sasa",total_sasa_mval);
		unbound_pose_sasa = total_sasa_mval.value();
		Total_pose_exposed_SASA = 1 - ((((unbound_pose_sasa + ligand_pose_sasa) - bound_pose_sasa)/2)/ligand_pose_sasa);

		//hydrophobic and polar SASA

		utility::vector1< core::Real > complex_rsd_sasa( bound_protein_pose.size(), 0.0 );
		utility::vector1< core::Real > separated_rsd_sasa( unbound_protein_pose.size(), 0.0 );
		utility::vector1< core::Real > complex_rsd_hsasa( bound_protein_pose.size(), 0.0 ); // hydrophobic SASA only
		utility::vector1< core::Real > separated_rsd_hsasa( unbound_protein_pose.size(), 0.0 ); // hydrophobic SASA only

		core::Real probe_radius = basic::options::option[basic::options::OptionKeys::pose_metrics::sasa_calculator_probe_radius];

		core::scoring::calc_per_res_hydrophobic_sasa( bound_protein_pose, complex_rsd_sasa, complex_rsd_hsasa, probe_radius, false );
		core::scoring::calc_per_res_hydrophobic_sasa( unbound_protein_pose, separated_rsd_sasa, separated_rsd_hsasa, probe_radius, false );

		core::Size lig_res_num = 0;
		for ( int j = 1, resnum = bound_protein_pose.size(); j <= resnum; ++j ) {
			if ( !bound_protein_pose.residue(j).is_protein() ) {
				lig_res_num = j;
				break;
			}
		}
		if ( lig_res_num == 0 ) {
			std::cout<<"No ligand given in bound protein PDB structure.  Cannot identify interface."<<std::endl;
			exit (1);
		}

		core::Real complex_sasa = 0.0;
		core::Real complex_polar_sasa = 0.0;
		core::Real complex_hydrophobic_sasa = 0.0;
		core::Real separated_sasa = 0.0;
		core::Real separated_polar_sasa = 0.0;
		core::Real separated_hydrophobic_sasa = 0.0;

		for ( core::uint j = 1, resnum = bound_protein_pose.size(); j <= resnum; ++j ) {
			if ( j == lig_res_num ) continue;
			complex_sasa += complex_rsd_sasa[j];
			complex_hydrophobic_sasa += complex_rsd_hsasa[j];
			separated_sasa += separated_rsd_sasa[j];
			separated_hydrophobic_sasa += separated_rsd_hsasa[j];
		}
		complex_polar_sasa = complex_sasa - complex_hydrophobic_sasa;
		separated_polar_sasa = separated_sasa - separated_hydrophobic_sasa;

		core::Real interface_sasa = std::abs(complex_sasa - separated_sasa);
		core::Real interface_hydrophobic_sasa = (std::abs(complex_hydrophobic_sasa - separated_hydrophobic_sasa))/interface_sasa;
		core::Real interface_polar_sasa = (std::abs(complex_polar_sasa - separated_polar_sasa))/interface_sasa;

		std::cout << "Scores:"<<bound_protein<<"\t"<<  Total_pose_exposed_SASA <<"\t"<< interface_hydrophobic_sasa <<"\t"<< interface_polar_sasa <<std::endl;

	} catch (utility::excn::Exception const & e ) {
		e.display();
		return -1;
	}
	return 0;

}
