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

#include <basic/options/util.hh>
#include <basic/options/option.hh>
#include <basic/options/keys/pose_metrics.OptionKeys.gen.hh>
#include <basic/options/option_macros.hh>


#include <core/conformation/Residue.hh>
#include <core/chemical/rna/RNA_Info.hh>

// Utility Headers
#include <utility/vector1.hh>
#include <utility/io/ozstream.hh>

// C++ Headers
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

//Auto Headers
#include <core/import_pose/import_pose.hh>
#include <core/pose/PDBInfo.hh>
#include <core/pose/init_id_map.hh>
#include <core/scoring/Energies.hh>
#include <core/scoring/EnergyGraph.hh>
#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/ScoreFunctionFactory.hh>
#include <core/scoring/sasa.hh>
#include <core/id/AtomID.hh>
#include <core/id/AtomID_Map.hh>
#include <core/scoring/TenANeighborGraph.hh>
#include <core/scoring/hbonds/HBondDatabase.hh>
#include <core/scoring/hbonds/HBondDatabase.fwd.hh>
#include <core/scoring/hbonds/HBondOptions.hh>
#include <core/scoring/hbonds/HBondSet.hh>
#include <core/scoring/hbonds/hbonds.hh>

#include <basic/Tracer.hh> // AUTO IWYU For Tracer

using namespace core;
using namespace core::pose::datacache;
using namespace core::pose::metrics;
using namespace core::scoring;
using namespace core::scoring::constraints;
using namespace core::id;
using namespace core::chemical;
using namespace core::chemical::rna;
using namespace basic::options;
using namespace basic::options::OptionKeys;
using namespace core::conformation;


OPT_KEY( String, input_rna )
OPT_KEY( String, input_protein )
OPT_KEY( Integer, rna_base_sasa_cutoff )

static basic::Tracer TR( "apps.pilot.ragul.rna_phr.get_rna_pharmacophore" );

// Returns TRUE if SASA for respective base is <= histogram cutoff
// cutoff value choosen based on the PDB scan of protein-RNA complxes
bool
is_buried_ring(core::conformation::Residue const & rsd, core::Real ring_sasa, core::Real sasa_cutoff){
	if ( rsd.name3() == " rA" ) return ring_sasa <= sasa_cutoff;//46.81 ;
	else if ( rsd.name3() == " rC" )  return ring_sasa <= sasa_cutoff;//31.09 ;
	else if ( rsd.name3() == " rG" )  return ring_sasa <= sasa_cutoff;//45.06 ;
	else if ( rsd.name3() == " rU" )  return ring_sasa <= sasa_cutoff;//52.66 ;
	else return false;
}

core::Real get_RNAring_sasa( core::conformation::Residue const & rsd, int rsdno,
	core::id::AtomID_Map<core::Real> & pose_atom_sasa ){
	if ( !rsd.is_RNA() ) {
		std::cout<<"Residue is not an RNA base. Cannot calculate RNA base SASA."<<std::endl;
		exit (1);
	}
	core::chemical::rna::RNA_Info const & rsd_type = rsd.RNA_info();
	core::Real curr_ring_sasa = 0;
	for ( Size rj = 1, rj_end = rsd.nheavyatoms(); rj <= rj_end; ++rj ) {
		if ( !rsd_type.is_RNA_base_atom(rj) ) continue;
		id::AtomID const aid( rj, rsdno);
		curr_ring_sasa += pose_atom_sasa[aid];
	}
	return(curr_ring_sasa);
}

int main( int argc, char * argv [] ){

	try {

		NEW_OPT( input_rna, "rna file name", "rna.pdb" );
		NEW_OPT( input_protein, "rna protein name", "protein.pdb" );
		NEW_OPT( rna_base_sasa_cutoff, "rna_base_sasa_cutoff", 25 );

		devel::init(argc, argv);

		std::string const input_rna_pose = option[ input_rna ];
		std::string const input_protein_pose = option[ input_protein ];
		int const sasa_cutoff = option[ rna_base_sasa_cutoff ];


		pose::Pose rna_pose, protein_pose;
		core::import_pose::pose_from_file( rna_pose, input_rna_pose , core::import_pose::PDB_file);
		core::import_pose::pose_from_file( protein_pose, input_protein_pose , core::import_pose::PDB_file);

		//Open outfile for RNA ring SASA in append mode
		std::ofstream complexrna_sasa_ofile;
		complexrna_sasa_ofile.open("complex_rna_ring_sasa.txt", std::ofstream::app);
		//Open pdb file for ring, donor & acceptor atoms
		utility::io::ozstream outPDB_stream;
		outPDB_stream.open("PHR.pdb", std::ios::out);

		for ( int ir = 1, ir_end = rna_pose.size(); ir <= ir_end; ir++ ) {
			core::conformation::Residue const & curr_rna_rsd = rna_pose.residue(ir);
			if ( !curr_rna_rsd.is_RNA() ) continue;
			int seqpos = curr_rna_rsd.seqpos();
			pose::Pose temp_protein_rnabase_pose = protein_pose;
			temp_protein_rnabase_pose.append_residue_by_jump(rna_pose.residue(ir), protein_pose.size(),"", "",  true);
			/*print pdb pose
			std::stringstream ss;
			ss << ir;
			std::string s(ss.str());
			std::string tmp = s + "_out.pdb";
			temp_protein_rnabase_pose.dump_pdb(tmp);
			*/

			//Set-up atomID for SASA calculations by atom
			utility::vector1< core::Real > complex_rsd_sasa( temp_protein_rnabase_pose.size(), 0.0 );
			core::id::AtomID_Map<core::Real> complex_atom_sasa;
			core::id::AtomID_Map<bool> complex_atom_subset;
			core::pose::initialize_atomid_map( complex_atom_sasa, temp_protein_rnabase_pose, 0.0 );
			core::pose::initialize_atomid_map( complex_atom_subset, temp_protein_rnabase_pose, true );
			//Calculate per-atom sasa
			core::Real probe_radius = basic::options::option[basic::options::OptionKeys::pose_metrics::sasa_calculator_probe_radius];
			core::scoring::calc_per_atom_sasa( temp_protein_rnabase_pose, complex_atom_sasa, complex_rsd_sasa, probe_radius, false, complex_atom_subset );
			//core::Real complex_rna_ring_sasa =  get_RNAring_sasa(curr_rna_rsd, ir, complex_atom_sasa);

			for ( int ic = 1, ic_end = temp_protein_rnabase_pose.size(); ic<=ic_end; ++ic ) {
				core::conformation::Residue const & curr_rsd = temp_protein_rnabase_pose.residue(ic);
				if ( !curr_rsd.is_RNA() ) continue;
				core::chemical::rna::RNA_Info const & curr_rsd_type = curr_rsd.RNA_info();
				core::Real curr_ring_sasa = 0;
				for ( Size jc = 1, jc_end = curr_rsd.nheavyatoms(); jc <= jc_end; ++jc ) {
					if ( !curr_rsd_type.is_RNA_base_atom(jc) ) continue;
					id::AtomID const aid( jc, ic);
					curr_ring_sasa += complex_atom_sasa[aid];
				}
				complexrna_sasa_ofile << seqpos << curr_rsd.name3() <<" "<< curr_ring_sasa<<"\n";

				// Filter RINGS based on base-sasa-cutoff
				if ( is_buried_ring(curr_rsd, curr_ring_sasa, sasa_cutoff) ) {
					outPDB_stream<<"REMARK"<<""<<seqpos<<""<<curr_rsd.name3()<<""<<curr_ring_sasa<<std::endl;
					for ( Size jc = 1, jc_end = curr_rsd.nheavyatoms(); jc <= jc_end; ++jc ) {
						if ( !curr_rsd_type.is_RNA_base_atom(jc) ) continue;
						//append rings to pdb file
						outPDB_stream
							<<std::setw(6)<<"ATOM  "
							<<std::setw(5)<<jc
							<<std::setw(5)<<curr_rsd.atom_name( jc )
							//<<std::setw(4)<<chemical::aa_from_oneletter_code(chemical::oneletter_code_from_aa(curr_rsd.aa()))
							<<std::setw(4)<<"RNG"
							<<" "
							<<std::setw(1)<<temp_protein_rnabase_pose.pdb_info()->chain(curr_rsd.seqpos())
							<<std::setw(4)<<seqpos
							<<"    "
							<<std::setw(8)<<std::fixed<<std::setprecision(3)<<curr_rsd.atom(jc).xyz()(1)<<std::setw(8)<<std::fixed<<std::setprecision(3)<<curr_rsd.atom(jc).xyz()(2)<<std::setw(8)<<std::fixed<<std::setprecision(3)<<curr_rsd.atom(jc).xyz()(3)<<std::endl;
					}
				}
			}

			//find Hbond interactions and include it to the pharmacophore list
			core::scoring::hbonds::HBondSet rna_hb_set;

			scoring::ScoreFunctionOP scorefxn = get_score_function();

			(*scorefxn)(temp_protein_rnabase_pose);

			assert( temp_protein_rnabase_pose.energies().residue_neighbors_updated() );
			core::scoring::EnergyGraph const & energy_graph( temp_protein_rnabase_pose.energies().energy_graph() );
			core::scoring::TenANeighborGraph const & tenA_neighbor_graph( temp_protein_rnabase_pose.energies().tenA_neighbor_graph() );
			core::scoring::hbonds::HBondDatabaseCOP hb_database( core::scoring::hbonds::HBondDatabase::get_database(core::scoring::hbonds::HBondOptions().params_database_tag()) );

			rna_hb_set.clear();

			for ( Size ic = 1, ic_end = temp_protein_rnabase_pose.size(); ic<=ic_end; ic++ ) {
				core::conformation::Residue const & curr_rsd = temp_protein_rnabase_pose.residue(ic);
				if ( !curr_rsd.is_RNA() ) continue;

				int const nnrna = tenA_neighbor_graph.get_node( ic )->num_neighbors_counting_self_static();

				for ( utility::graph::Graph::EdgeListConstIter
						nit = energy_graph.get_node(ic)->const_edge_list_begin(),
						nite = energy_graph.get_node(ic)->const_edge_list_end();
						nit != nite; ++nit ) {

					Size const pin( (*nit)->get_other_ind(ic) );

					Residue const& rn( temp_protein_rnabase_pose.residue( pin ) );
					int const nnn = tenA_neighbor_graph.get_node( pin )->num_neighbors_counting_self_static();

					if ( !rn.is_RNA() ) {

						identify_hbonds_1way( *hb_database, curr_rsd, rn, nnrna, nnn, false,
							false, false, false, false, rna_hb_set);

						identify_hbonds_1way( *hb_database, rn, curr_rsd, nnn, nnrna, false,
							false, false, false, false, rna_hb_set);
					}
				}
			}


			//TR << "\n\nHBOND SET:\n";
			for ( Size i=1; i<= Size(rna_hb_set.nhbonds()); ++i ) {

				core::scoring::hbonds::HBond const & hb( rna_hb_set.hbond(i) );
				core::conformation::Residue const & donor =temp_protein_rnabase_pose.residue( hb.don_res() );
				core::conformation::Residue const & accep =temp_protein_rnabase_pose.residue( hb.acc_res() );
				Size const donor_hatm_num = hb.don_hatm();
				Size const donor_base_atom_num = donor.atom_base( donor_hatm_num );
				Size const accep_atom_num = hb.acc_atm();

				TR << i << ":" <<
					chemical::oneletter_code_from_aa(donor.aa()) <<
					temp_protein_rnabase_pose.pdb_info()->number(donor.seqpos()) << temp_protein_rnabase_pose.pdb_info()->chain(donor.seqpos()) << ' ' <<
					'(' << donor.seqpos() << ')' <<
					donor.atom_name( donor_base_atom_num ) << " --- " <<
					chemical::oneletter_code_from_aa(accep.aa()) <<
					temp_protein_rnabase_pose.pdb_info()->number(accep.seqpos()) << temp_protein_rnabase_pose.pdb_info()->chain(accep.seqpos()) << ' ' <<
					'(' << accep.seqpos() << ')' <<
					accep.atom_name( accep_atom_num ) << "\n";

				//print PDB
				if ( donor.is_RNA() ) {
					outPDB_stream
						<<std::setw(6)<<"ATOM  "
						<<std::setw(5)<<i
						<<std::setw(5)<< donor.atom_name( donor_base_atom_num )
						//<<std::setw(4)<<chemical::aa_from_oneletter_code(chemical::oneletter_code_from_aa(donor.aa()))
						<<std::setw(4)<<"DNR"
						<<" "
						<<std::setw(1)<<temp_protein_rnabase_pose.pdb_info()->chain(donor.seqpos())
						<<std::setw(4)<<seqpos
						<<"    "
						<<std::setw(8)<<std::fixed<<std::setprecision(3)<<donor.atom(donor_base_atom_num).xyz()(1)<<std::setw(8)<<std::fixed<<std::setprecision(3)<<donor.atom(donor_base_atom_num).xyz()(2)<<std::setw(8)<<std::fixed<<std::setprecision(3)<<donor.atom(donor_base_atom_num).xyz()(3)<<std::endl;
				}
				if ( accep.is_RNA() ) {
					outPDB_stream
						<<std::setw(6)<<"ATOM  "
						<<std::setw(5)<<i
						<<std::setw(5)<<accep.atom_name( accep_atom_num )
						//<<std::setw(4)<<chemical::aa_from_oneletter_code(chemical::oneletter_code_from_aa(accep.aa()))
						<<std::setw(4)<<"ACP"
						<<" "
						<<std::setw(1)<<temp_protein_rnabase_pose.pdb_info()->chain(accep.seqpos())
						<<std::setw(4)<<seqpos
						<<"    "
						<<std::setw(8)<<std::fixed<<std::setprecision(3)<<accep.atom(accep_atom_num).xyz()(1)<<std::setw(8)<<std::fixed<<std::setprecision(3)<<accep.atom(accep_atom_num).xyz()(2)<<std::setw(8)<<std::fixed<<std::setprecision(3)<<accep.atom(accep_atom_num).xyz()(3)<<std::endl;
				}
			}

			outPDB_stream.clear();
			complexrna_sasa_ofile.clear();

		}
		outPDB_stream.close();
		complexrna_sasa_ofile.close();

	} catch (utility::excn::Exception const & e ) {
		e.display();
		return -1;
	}

	return 0;
}
