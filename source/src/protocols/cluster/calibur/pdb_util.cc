// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file external/calibur/pdb_util.hh
/// @author SC Li & YK Ng (kalngyk@gmail.com)
/// @author Andy Watkins (amw579@stanford.edu)

#define LONGEST_CHAIN 4000

#include <iostream>
#include <fstream>


#include <vector>

#include <protocols/cluster/calibur/pdb_util.hh>


#include <string.h> // AUTO IWYU For strtok

namespace protocols {
namespace cluster {
namespace calibur {

char aa[][4] = {"BCK","GLY","ALA","SER","CYS","VAL","THR","ILE",
"PRO","MET","ASP","ASN","LEU",
"LYS","GLU","GLN","ARG",
"HIS","PHE","TYR","TRP","CYX", "MSE"};

char slc[] = {'X','G','A','S','C','V','T','I',
'P','M','D','N','L','K','E','Q','R',
'H','F','Y','W','C', 'm'};



using StringVec = std::vector<std::string>;
using StringVecOP = utility::pointer::shared_ptr<StringVec>;


//- = - = - = - = - = - = - = - = - = - = - = - = - = - = - = - = - = -

int toInt( std::string const & aString ) {
	static char st[20];
	int start = 0;
	for ( char i : aString ) {
		if ( i != ' ' ) {
			st[start++] = i;
		}
	}
	st[start] = '\0';
	int rev = atoi(st);
	return rev;
}


core::Real toFloat( std::string const & aString ) {
	static char st[20];
	int start = 0;
	for ( char i : aString ) {
		if ( i != ' ' ) {
			st[start++] = i;
		}
	}
	st[start] = '\0';
	core::Real rev = atof(st);
	return rev;
}


void center_residues( std::vector<core::Real> & calpha_vector, int num_residue_ ) {
	core::Real cx = 0;
	core::Real cy = 0;
	core::Real cz = 0;

	int i3 = 0;
	for ( int i=0; i < num_residue_; i++ ) {
		cx += calpha_vector[i3];
		cy += calpha_vector[i3+1];
		cz += calpha_vector[i3+2];
		i3 += 3;
	}

	cx /= num_residue_;
	cy /= num_residue_;
	cz /= num_residue_;
	i3 = 0;
	for ( int i=0; i < num_residue_; i++ ) {
		calpha_vector[i3]   -= cx;
		calpha_vector[i3+1] -= cy;
		calpha_vector[i3+2] -= cz;
		i3 += 3;
	}
}


INPUT_FILE_TYPE
filetype( std::string const & filename )
{
	char buf[400];
	std::ifstream input(filename);
	std::string line;
	if ( !input ) {
		std::cerr << "Can't open input file \"" << filename.c_str() << "\"" << std::endl;
		exit(0);
	}

	input.getline(buf, 400);
	line = buf;
	if ( line.substr(0, 9)=="SEQUENCE:" ) {
		input.close();
		return SILENT_FILE;
	}

	input.seekg(0);
	input.getline(buf, 400);
	char *token = strtok(buf, " ");
	if ( token == nullptr ) {
		input.close();
		return UNKNOWN;
	}

	std::ifstream pdbfile(token); // check if token (name) is a file
	if ( pdbfile ) {
		pdbfile.close();
		input.close();
		return PDB_LIST;
	}
	pdbfile.close();

	input.close();

	return UNKNOWN;
}


unsigned int num_lines_in_file(std::string const & filename) {
	char buf[400];
	std::ifstream input(filename);
	if ( !input ) {
		std::cerr << "Can't open input file \"" << filename.c_str() << "\"" << std::endl;
		exit(0);
	}
	unsigned int count = 0;
	while ( !input.eof() )
			{
		input.getline(buf, 400);
		count++;
	}
	return count-1;
}

}
}
}

