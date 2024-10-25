// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   ClosureProblem.cc
/// @brief  Source file for ClosureProblem.
/// @author Kale Kundert (kale.kundert@ucsf.edu)

// Unit headers
#include <protocols/kinematic_closure/types.hh>
#include <protocols/kinematic_closure/internal.hh>
#include <protocols/kinematic_closure/ClosureProblem.hh>
#include <protocols/kinematic_closure/ClosureSolution.hh>
#include <protocols/kinematic_closure/pivot_pickers/PivotPicker.hh>

// Core headers
#include <core/pose/Pose.hh>
#include <core/kinematics/FoldTree.hh>
#include <core/conformation/Conformation.hh>
#include <core/conformation/Residue.hh>
#include <core/id/AtomID.hh>
#include <core/id/TorsionID.hh>
#include <core/util/kinematics_util.hh>

// Protocols headers
#include <protocols/loops/Loop.hh>
#include <protocols/loops/loops_main.hh>
#include <protocols/simple_moves/MutateResidue.hh>

// Numeric headers
#include <numeric/conversions.hh>
#include <numeric/kinematic_closure/closure.hh>
#include <numeric/kinematic_closure/vector.hh>
#include <numeric/xyz.functions.hh>
#include <numeric/xyzVector.hh>

// Utility headers
#include <utility/exit.hh>
#include <utility/vector1.hh>
#include <utility/tools/make_vector1.hh>
#include <iostream>

using namespace std;

namespace protocols {
namespace kinematic_closure {

using core::id::AtomID;
using numeric::kinematic_closure::operator <<;
using numeric::conversions::radians;
using numeric::conversions::from_radians;

ClosureProblem::ClosureProblem() { // {{{1
	frame_called_ = false;
}

ClosureProblem::~ClosureProblem() = default; // {{{1
// }}}1

void ClosureProblem::frame( // {{{1
	Pose const & pose, Loop const & loop,
	pivot_pickers::PivotPickerOP pivot_picker) {

	pivots_ = pivot_picker->pick(pose, loop);
	pivots_.set_cut(pivots_.cut() ? pivots_.cut() : pivots_.midpoint());

	frame_called_ = true;

	// Extract the unperturbed DOFs from the given pose.

	extract_cartesian_coordinates(pose,
		unperturbed_xyzs_);

	extract_internal_coordinates(
		unperturbed_xyzs_,
		unperturbed_lengths_,
		unperturbed_angles_,
		unperturbed_torsions_);

	// Copy the unperturbed DOFS into the matrices that will eventually be
	// modified by the perturbers.

	perturbed_torsions_ = ParameterList(unperturbed_torsions_);
	perturbed_angles_ = ParameterList(unperturbed_angles_);
	perturbed_lengths_ = ParameterList(unperturbed_lengths_);

	// Store the sequence of the residues between pivots (including pivots)

	unperturbed_sequence_.clear();
	for ( core::Size i=pivots_.start(); i<=pivots_.stop(); ++i ) {
		unperturbed_sequence_.push_back(pose.residue(i).name3());
	}
	perturbed_sequence_ = unperturbed_sequence_;

}

SolutionList ClosureProblem::solve() const { // {{{1
	int num_solutions = 0;
	IndexList order = utility::tools::make_vector1(1, 2, 3);

	ParameterMatrix solution_torsions;
	ParameterMatrix solution_angles;
	ParameterMatrix solution_lengths;

	runtime_assert(frame_called_);

	// Solve closure problem.

	numeric::kinematic_closure::radians::bridge_objects(
		unperturbed_xyzs_,
		perturbed_torsions_,
		perturbed_angles_,
		perturbed_lengths_,
		pivot_atoms(), order,
		solution_torsions,
		solution_angles,
		solution_lengths,
		num_solutions);

	// Cast the results into an easy-to-use format.

	SolutionList solutions(num_solutions);

	for ( core::Size index = 1; index <= (core::Size) num_solutions; index++ ) {
		solutions[index] = protocols::kinematic_closure::ClosureSolutionOP( new ClosureSolution(
			this, index,
			solution_torsions[index],
			solution_angles[index],
			solution_lengths[index]) );
	}

	return solutions;
}

// {{{1

/// @details This method is typically used to restore the pose if no solutions
/// to the closure problem could be found.  It should only be called after
/// frame() and solve().

void ClosureProblem::restore(Pose & pose) const {
	runtime_assert(frame_called_);
	apply_internal_coordinates(
		unperturbed_lengths_, unperturbed_angles_, unperturbed_torsions_, pose);
	mutate_residues(pose, unperturbed_sequence_);
}
// }}}1

void ClosureProblem::perturb_phi( // {{{1
	core::Size residue, Real value, AngleUnit unit) {

	core::Size loop_residue = residue - first_residue() + 1;
	perturbed_torsions_[3 * loop_residue + 1] = radians(value, unit);
}

void ClosureProblem::perturb_psi( // {{{1
	core::Size residue, Real value, AngleUnit unit) {

	core::Size loop_residue = residue - first_residue() + 1;
	perturbed_torsions_[3 * loop_residue + 2] = radians(value, unit);
}

void ClosureProblem::perturb_omega( // {{{1
	core::Size residue, Real value, AngleUnit unit) {

	core::Size loop_residue = residue - first_residue() + 1;
	perturbed_torsions_[3 * loop_residue + 0] = radians(value, unit);
}

void ClosureProblem::perturb_n_ca_c( // {{{1
	core::Size residue, Real value, AngleUnit unit) {

	core::Size loop_residue = residue - first_residue() + 1;
	perturbed_angles_[3 * loop_residue + 2] = radians(value, unit);
}

void ClosureProblem::perturb_ca_c_n( // {{{1
	core::Size residue, Real value, AngleUnit unit) {

	core::Size loop_residue = residue - first_residue() + 1;
	perturbed_angles_[3 * loop_residue + 0] = radians(value, unit);
}

void ClosureProblem::perturb_c_n_ca( // {{{1
	core::Size residue, Real value, AngleUnit unit) {

	core::Size loop_residue = residue - first_residue() + 1;
	perturbed_angles_[3 * loop_residue + 1] = radians(value, unit);
}

void ClosureProblem::perturb_n_ca(core::Size residue, Real value) { // {{{1
	core::Size loop_residue = residue - first_residue() + 1;
	perturbed_lengths_[3 * loop_residue + 1] = value;
}

void ClosureProblem::perturb_ca_c(core::Size residue, Real value) { // {{{1
	core::Size loop_residue = residue - first_residue() + 1;
	perturbed_lengths_[3 * loop_residue + 2] = value;
}

void ClosureProblem::perturb_c_n(core::Size residue, Real value) { // {{{1
	core::Size loop_residue = residue - first_residue() + 1;
	perturbed_lengths_[3 * loop_residue + 0] = value;
}

ParameterList & ClosureProblem::perturb_torsions() { // {{{1
	return perturbed_torsions_;
}

ParameterList & ClosureProblem::perturb_angles() { // {{{1
	return perturbed_angles_;
}

ParameterList & ClosureProblem::perturb_lengths() { // {{{1
	return perturbed_lengths_;
}

ClosureProblem::Memento::Memento(ClosureProblemOP problem) // {{{1
: problem_(problem),
	perturbed_torsions_(problem->perturbed_torsions_),
	perturbed_angles_(problem->perturbed_angles_),
	perturbed_lengths_(problem->perturbed_lengths_) {

	runtime_assert(problem->frame_called_);
}

void ClosureProblem::Memento::restore() const { // {{{1
	problem_->perturbed_torsions_ = perturbed_torsions_;
	problem_->perturbed_angles_ = perturbed_angles_;
	problem_->perturbed_lengths_ = perturbed_lengths_;
}
// }}}1

Size ClosureProblem::first_residue() const { // {{{1
	runtime_assert(frame_called_);
	return pivots_.start();
}

Size ClosureProblem::cut_residue() const { // {{{1
	runtime_assert(frame_called_);
	return pivots_.cut();
}

Size ClosureProblem::last_residue() const { // {{{1
	runtime_assert(frame_called_);
	return pivots_.stop();
}

Size ClosureProblem::num_residues() const { // {{{1
	runtime_assert(frame_called_);
	return pivots_.length();
}

Size ClosureProblem::num_atoms() const { // {{{1
	return 3 * (num_residues() + 2);
}

Real ClosureProblem::phi(core::Size residue, AngleUnit unit) const { // {{{1
	core::Size loop_residue = residue - first_residue() + 1;
	return from_radians(perturbed_torsions_[3 * loop_residue + 1], unit);
}

Real ClosureProblem::psi(core::Size residue, AngleUnit unit) const { // {{{1
	core::Size loop_residue = residue - first_residue() + 1;
	return from_radians(perturbed_torsions_[3 * loop_residue + 2], unit);
}

Real ClosureProblem::omega(core::Size residue, AngleUnit unit) const { // {{{1
	core::Size loop_residue = residue - first_residue() + 1;
	return from_radians(perturbed_torsions_[3 * loop_residue + 0], unit);
}

Real ClosureProblem::n_ca_c(core::Size residue, AngleUnit unit) const { // {{{1
	core::Size loop_residue = residue - first_residue() + 1;
	return from_radians(perturbed_angles_[3 * loop_residue + 2], unit);
}

Real ClosureProblem::ca_c_n(core::Size residue, AngleUnit unit) const { // {{{1
	core::Size loop_residue = residue - first_residue() + 1;
	return from_radians(perturbed_angles_[3 * loop_residue + 0], unit);
}

Real ClosureProblem::c_n_ca(core::Size residue, AngleUnit unit) const { // {{{1
	core::Size loop_residue = residue - first_residue() + 1;
	return from_radians(perturbed_angles_[3 * loop_residue + 1], unit);
}

Real ClosureProblem::n_ca(core::Size residue) const { // {{{1
	core::Size loop_residue = residue - first_residue() + 1;
	return perturbed_lengths_[3 * loop_residue + 1];
}

Real ClosureProblem::ca_c(core::Size residue) const { // {{{1
	core::Size loop_residue = residue - first_residue() + 1;
	return perturbed_lengths_[3 * loop_residue + 2];
}

Real ClosureProblem::c_n(core::Size residue) const { // {{{1
	core::Size loop_residue = residue - first_residue() + 1;
	return perturbed_lengths_[3 * loop_residue + 0];
}

bool ClosureProblem::is_pivot_residue(core::Size residue) const { // {{{1
	return residue == first_residue() ||
		residue == cut_residue() ||
		residue == last_residue();
}

bool ClosureProblem::is_nonpivot_residue(core::Size residue) const { // {{{1
	return ! is_pivot_residue(residue);
}

Loop ClosureProblem::pivot_loop() const { // {{{1
	return pivots_;
}

IndexList ClosureProblem::residues() const { // {{{1
	IndexList residues;

	for ( core::Size i = first_residue(); i <= last_residue(); i++ ) {
		residues.push_back(i);
	}

	return residues;
}

IndexList ClosureProblem::pivot_residues() const { // {{{1
	IndexList pivot_residues(3);

	pivot_residues[1] = first_residue();
	pivot_residues[2] = cut_residue();
	pivot_residues[3] = last_residue();

	return pivot_residues;
}

IndexList ClosureProblem::nonpivot_residues() const { // {{{1
	IndexList nonpivot_residues;

	for ( core::Size i = first_residue() + 1; i < last_residue(); i++ ) {
		if ( i == cut_residue() ) continue;
		nonpivot_residues.push_back(i);
	}

	return nonpivot_residues;
}

IndexList ClosureProblem::pivot_atoms() const { // {{{1
	IndexList pivot_atoms(3);

	core::Size cut_offset = cut_residue() - first_residue();
	core::Size end_offset = last_residue() - first_residue();

	pivot_atoms[1] = 5;
	pivot_atoms[2] = 5 + (3 * cut_offset);
	pivot_atoms[3] = 5 + (3 * end_offset);

	return pivot_atoms;
}

void ClosureProblem::perturbed_sequence(utility::vector1<std::string> const &values){
	perturbed_sequence_ = values;
}

utility::vector1<std::string> ClosureProblem::perturbed_sequence() const{
	return perturbed_sequence_;
}

utility::vector1<std::string> ClosureProblem::unperturbed_sequence() const{
	return unperturbed_sequence_;
}

bool ClosureProblem::sequence_mutated() const{
	bool mutation_made = false;
	for ( core::Size i=1; i<=unperturbed_sequence_.size(); i++ ) {
		mutation_made = mutation_made || perturbed_sequence_[i] != unperturbed_sequence_[i];
	}
	return mutation_made;
}

// }}}1

// {{{1
/// @details This method does not change anything about the problem object
/// itself.  All changes are made to the given `atom_xyzs` pseudo-matrix.

void ClosureProblem::extract_cartesian_coordinates (
	Pose const & pose, CoordinateList & atom_xyzs) const {

	atom_xyzs.resize(num_atoms());

	// The closure algorithm requires coordinates for the two residues which
	// frame the region being closed.  If the region coincides with the end of a
	// chain, this is becomes a nontrivial problem.  For that reason, this step
	// has been delegated out to the two functions called below.

	frame_lower_pivot(pose, atom_xyzs);
	frame_upper_pivot(pose, atom_xyzs);

	// The remaining code simply copies Cartesian coordinates from the given pose
	// into the given matrix structure.  The copying starts with the 4th position
	// in the matrix, to prevent overwriting the work done above.

	core::Size index = 4;

	for ( core::Size i = first_residue(); i <= last_residue(); i++ ) {
		core::conformation::Residue const &residue = pose.residue(i);

		// This inner loop assumes that atoms 1, 2, & 3 are the backbone atoms.
		// This will break if the residue in question is, for example, a metal ion.
		// Metal ions only have one atom, so attempts to access the second and
		// third ones will crash the program.

		for ( core::Size j = 1; j <= 3; j++ ) {
			atom_xyzs[index][1] = residue.xyz(j).x();
			atom_xyzs[index][2] = residue.xyz(j).y();
			atom_xyzs[index][3] = residue.xyz(j).z();
			index++;
		}
	}
}

void ClosureProblem::extract_internal_coordinates( // {{{1
	CoordinateList const & atom_xyzs,
	ParameterList & bond_lengths,
	ParameterList & bond_angles,
	ParameterList & torsion_angles) const {

	using numeric::xyzVector;
	using numeric::angle_radians;
	using numeric::dihedral_radians;

	bond_lengths = ParameterList(num_atoms(), 0);
	bond_angles = ParameterList(num_atoms(), 0);
	torsion_angles = ParameterList(num_atoms(), 0);

	core::Size num_bond_lengths = num_atoms() - 1;
	core::Size num_bond_angles = num_atoms() - 2;
	core::Size num_torsion_angles = num_atoms() - 3;

	xyzVector<Real> vecs[4];

	// Bond Lengths

	for ( core::Size index = 1; index <= num_bond_lengths; index++ ) {
		vecs[0] << atom_xyzs[index + 0];
		vecs[1] << atom_xyzs[index + 1];

		bond_lengths[index] = vecs[0].distance(vecs[1]);
	}

	// Bond Angles

	for ( core::Size index = 1; index <= num_bond_angles; index++ ) {
		vecs[0] << atom_xyzs[index + 0];
		vecs[1] << atom_xyzs[index + 1];
		vecs[2] << atom_xyzs[index + 2];

		bond_angles[(index % num_atoms()) + 1] =
			angle_radians(vecs[0], vecs[1], vecs[2]);
	}

	// Torsion Angles

	for ( core::Size index = 1; index <= num_torsion_angles; index++ ) {
		vecs[0] << atom_xyzs[index + 0];
		vecs[1] << atom_xyzs[index + 1];
		vecs[2] << atom_xyzs[index + 2];
		vecs[3] << atom_xyzs[index + 3];

		torsion_angles[(index % num_atoms()) + 1] =
			dihedral_radians(vecs[0], vecs[1], vecs[2], vecs[3]);
	}
}

void ClosureProblem::apply_internal_coordinates( // {{{1
	ParameterList const & bond_lengths,
	ParameterList const & bond_angles,
	ParameterList const & torsion_angles,
	Pose & pose) const {

	// This function is complicated by the fact that the inputs (bond_lengths,
	// bond_angles, and torsion_angles) contain values which need to be ignored.
	// The bridge_objects() method accepts internal and cartesian coordinates for
	// every backbone atom within one residue of the loop.  Only the cartesian
	// coordinates are used for atoms outside the loop, and only the internal
	// coordinates are used for atoms inside the loop.  Internal coordinates for
	// atoms outside the loop are ignored, but are still copied into the matrices
	// that get returned and subsequently passed to this method.  The punchline
	// is that the internal coordinates for atoms outside the loop passed to this
	// function may not agree with the cartesian coordinates that were actually
	// used to solve the closure problem, and need to be ignored.
	//
	// This problem is triggered by the IdealizePerturber, which sets an omega
	// and some bond angles for atoms before the loop.  All these inputs are
	// ignored by bridge_objects(), but passed through to this method.  Applying
	// them results in a chain break.
	//
	// In my view, this is really a problem with bridge_objects() and the fact
	// that it requires redundant data.  However, bridge_objects() is a hairy
	// function that I'm already refactoring in another branch, so it's not worth
	// meddling with here.  Instead, I carefully chose indices for all the loops
	// in this function that avoid the bogus dofs.  I could have also made
	// changes to perturb_phi() and its brethren so that the bogus dofs couldn't
	// be set in the first place, but this seemed like a more robust approach.

	using core::conformation::Conformation;
	using core::id::AtomID;

	Conformation & conformation = pose.conformation();
	AtomID ids[4];

	// Set the bond lengths in the solution pose.  Note that in bond_lengths,
	// index x refers to the distance between atoms x and x+1.

	for ( core::Size index = 5; index <= num_atoms() - 3; index++ ) {
		ids[0] = id_from_index(index + 0);
		ids[1] = id_from_index(index + 1);

		conformation.set_bond_length(
			ids[0], ids[1], bond_lengths[index]);
	}

	// Set the bond angles in the solution pose.  Note that in bond_angles, index
	// x refers to the angle between atoms x-1 and x+1.

	for ( core::Size index = 5; index <= num_atoms() - 3; index++ ) {
		ids[0] = id_from_index(index - 1);
		ids[1] = id_from_index(index + 0);
		ids[2] = id_from_index(index + 1);

		conformation.set_bond_angle(
			ids[0], ids[1], ids[2], bond_angles[index]);
	}

	// Set the torsion angles in the solution pose.  Note that in torsion_angles,
	// index x refers to the angle between atoms x-1 and x+2.

	using numeric::conversions::degrees;
	for ( core::Size index = 4, cur_res = first_residue(); cur_res <= last_residue(); ++cur_res ) {
		pose.set_phi(cur_res, degrees(torsion_angles[index++]));
		pose.set_psi(cur_res, degrees(torsion_angles[index++]));
		if ( cur_res < last_residue() ) pose.set_omega(cur_res, degrees(torsion_angles[index++]));
	}

	pose.update_residue_neighbors();
}

void ClosureProblem::frame_lower_pivot( // {{{1
	Pose const & pose, CoordinateList & atom_xyzs) const {

	using core::conformation::Residue;
	using core::conformation::ResidueOP;

	ResidueOP frame_residue;
	Residue pivot_residue = pose.residue(first_residue());

	// The closure algorithm requires coordinates for the two residues framing
	// the region being sampled.  This method finds those coordinates for the
	// residue framing the lower (N-terminal) side of the region.  If the lower
	// pivot is also the N-terminus, this residue doesn't exist and the needed
	// coordinates are from a "pseudo-residue" that is artificially build with
	// ideal coordinates.

	if ( pivot_residue.is_lower_terminus() ) {
		core::conformation::ConformationOP buffer( new core::conformation::Conformation() );

		// Make a copy of the N-terminal residue.
		Residue pivot_residue_copy = core::conformation::Residue(
			pivot_residue.type(), pivot_residue,
			pose.conformation(), false);

		// Create a pseudo-residue of the same type.
		Residue pseudo_residue = Residue(
			pivot_residue.type(), true /* dummy argument to pick constructor. */);

		// Store the N-terminus in a temporary conformation.
		buffer->append_residue_by_bond(
			pivot_residue_copy);

		// Attach the pseudo-residue before the N-terminus in the copy.
		buffer->safely_prepend_polymer_residue_before_seqpos(
			pseudo_residue, 1, true);

		// Set the junction omega angle to its ideal value.
		buffer->set_torsion(
			core::id::TorsionID(1, core::id::BB, 3),
			IdealParameters::omega_dihedral);

		// Get a pointer to the pseudo-residue.
		frame_residue = buffer->residue(1).clone();
	} else {
		// If the lower pivot isn't the N-terminus, then simply identify the frame
		// residue as the one right below it.  This is the standard case.
		frame_residue = pose.residue(first_residue() - 1).clone();
	}

	// Append the coordinates of the frame residue into the list of atoms for the
	// kinematic closure algorithm to consider.  The coordinates are added in the
	// order: N, CA, C.

	for ( core::Size i = 1; i <= 3; i++ ) {
		atom_xyzs[i][1] = frame_residue->xyz(i).x();
		atom_xyzs[i][2] = frame_residue->xyz(i).y();
		atom_xyzs[i][3] = frame_residue->xyz(i).z();
	}
}

void ClosureProblem::frame_upper_pivot( // {{{1
	Pose const & pose, CoordinateList & atom_xyzs) const {

	using core::conformation::Residue;
	using core::conformation::ResidueOP;

	ResidueOP frame_residue;
	Residue pivot_residue = pose.residue(last_residue());

	// The closure algorithm requires coordinates for the two residues framing
	// the region being sampled.  This method finds those coordinates for the
	// residue framing the upper (C-terminal) side of the region.  This residue
	// won't exist if the lower pivot is the C-terminus.  In this case, the
	// needed coordinates are extracted from a "pseudo-residue" that is
	// artificially build with ideal coordinates.

	if ( pivot_residue.is_upper_terminus() ) {
		core::conformation::ConformationOP buffer( new core::conformation::Conformation() );

		// Make a copy of the N-terminal residue.
		Residue pivot_residue_copy = core::conformation::Residue(
			pivot_residue.type(), pivot_residue,
			pose.conformation(), false);

		// Create a pseudo-residue of the same type.
		Residue pseudo_residue = core::conformation::Residue(
			pivot_residue.type(), true /* dummy argument to pick constructor. */);

		// Store the N-terminus in a temporary conformation.
		buffer->append_residue_by_bond(
			pivot_residue_copy);

		// Attach the pseudo-residue before the N-terminus in the copy.
		buffer->safely_append_polymer_residue_after_seqpos(
			pseudo_residue, 1, true);

		// Set the junction omega angle to its ideal value.
		buffer->set_torsion(
			core::id::TorsionID(2, core::id::BB, 3),
			IdealParameters::omega_dihedral);

		// Get a pointer to the pseudo-residue.
		frame_residue = buffer->residue(2).clone();
	} else {
		// If the upper pivot isn't the C-terminus, then simply identify the frame
		// residue as the one right above it.  This is the standard case.
		frame_residue = pose.residue(last_residue() + 1).clone();
	}

	// Append the coordinates of the frame residue into the list of atoms for the
	// kinematic closure algorithm to consider.  The coordinates are added in the
	// order: N, CA, C.

	core::Size offset = atom_xyzs.size() - 3;

	for ( core::Size i = 1; i <= 3; i++ ) {
		atom_xyzs[i + offset][1] = frame_residue->xyz(i).x();
		atom_xyzs[i + offset][2] = frame_residue->xyz(i).y();
		atom_xyzs[i + offset][3] = frame_residue->xyz(i).z();
	}
}

bool ClosureProblem::ids_span_cut(AtomID left, AtomID right) const { // {{{1
	return (left.rsd() == cut_residue()) && (right.rsd() == cut_residue() + 1);
}

// {{{1
/// @details More specifically, the first atom in the loop is taken to be the
/// backbone nitrogen of the residue preceding the first pivot.  Thus the index
/// of the first pivot is 5 (pivots must be alpha carbons).

AtomID ClosureProblem::id_from_index(core::Size index) const {
	return AtomID(
		((index - 1) % 3) + 1,
		((index - 1) / 3) + first_residue() - 1);
}
// }}}1

void ClosureProblem::mutate_residues(core::pose::Pose &pose, utility::vector1<std::string> sequence) const{
	runtime_assert(frame_called_);

	// If there is no mutation, return

	bool mutation_made = false;
	for ( core::Size i=1; i<=pivots_.stop() - pivots_.start() + 1; ++i ) {
		core::Size seqpos = pivots_.start() + i - 1;

		if ( pose.residue(seqpos).name3() != sequence[i] ) {
			mutation_made = true;
		}
	}

	if ( !mutation_made ) { return;}

	protocols::simple_moves::MutateResidue mutater;

	// Change the fold tree such that the residue at the cutpoint won't cause problems
	core::util::remove_cutpoint_variants(pose);
	core::kinematics::FoldTree original_ft(pose.fold_tree());
	core::kinematics::FoldTree ft(pose.size());
	pose.fold_tree(ft);

	for ( core::Size i=1; i<=pivots_.stop() - pivots_.start() + 1; ++i ) {
		core::Size seqpos = pivots_.start() + i - 1;

		// Apply mutation at positions where AA is changed
		if ( pose.residue(i).name3() != sequence[i] ) {
			mutater.set_res_name(sequence[i]);
			mutater.set_target(seqpos);
			mutater.apply(pose);
		}
	}

	// Restore the fold tree
	pose.fold_tree(original_ft);
	protocols::loops::add_cutpoint_variants(pose);
}

} // end namespace kinematic_closure
} // end namespace protocols
