// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/task_operations/RetrieveStoredTaskOperation.hh
/// @brief  Retrieves a stored task from the pose's CacheableData. Must be used in
///         conjunction with the StoredTaskMover. Allows storage/retrieval of a task
///         so that particular sets of residues can be stably addressed throughout
///         the entirety of a RosettaScripts protocol.
/// @author Neil King (neilking@uw.edu)

#ifndef INCLUDED_protocols_task_operations_RetrieveStoredTaskOperation_hh
#define INCLUDED_protocols_task_operations_RetrieveStoredTaskOperation_hh

// Unit Headers
#include <protocols/task_operations/RetrieveStoredTaskOperation.fwd.hh>
#include <core/pack/task/operation/TaskOperation.hh>

// Project Headers
#include <core/pose/Pose.fwd.hh>
#include <core/pack/task/PackerTask.fwd.hh>

// Utility Headers
#include <utility/tag/XMLSchemaGeneration.fwd.hh>

// C++ Headers
#include <string>

namespace protocols {
namespace task_operations {

/// @details this class is a TaskOperation to prevent repacking of residues not near an interface.
class RetrieveStoredTaskOperation : public core::pack::task::operation::TaskOperation {
public:

	RetrieveStoredTaskOperation();
	core::pack::task::operation::TaskOperationOP clone() const override;
	~RetrieveStoredTaskOperation() override;

	void apply( core::pose::Pose const &, core::pack::task::PackerTask & ) const override;
	void parse_tag( TagCOP, DataMap & ) override;
	static void provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );
	static std::string keyname() { return "RetrieveStoredTask"; }

private:
	std::string task_name_;

};

} //namespace task_operations
} //namespace protocols

#endif // INCLUDED_protocols_task_operations_RetrieveStoredTaskOperation_HH
