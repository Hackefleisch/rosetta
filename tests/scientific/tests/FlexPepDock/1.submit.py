#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# :noTabs=true:

# (c) Copyright Rosetta Commons Member Institutions.
# (c) This file is part of the Rosetta software suite and is made available under license.
# (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
# (c) For more information, see http://www.rosettacommons.org. Questions about this can be
# (c) addressed to University of Washington CoMotion, email: license@uw.edu.

## @file  _template_/1.submit.py
## @brief this script is part of <template> scientific test
## @author Sergey Lyskov

import os, sys, time
import benchmark

benchmark.load_variables()  # Python black magic: load all variables saved by previous script into globals
config = benchmark.config()

#==> EDIT HERE
testname    = "FlexPepDock"

debug       = config['debug']
rosetta_dir = config['rosetta_dir']
working_dir = config['working_dir']
hpc_driver  = benchmark.hpc_driver()
extension   = benchmark.calculate_extension()

#==> EDIT HERE
command_line = '''
-database {rosetta_dir}/database
-in:file:s {rosetta_dir}/tests/scientific/data/{testname}/{target}.pdb
-in:file:native {rosetta_dir}/tests/scientific/data/{testname}/{native}.pdb
-nstruct {nstruct}
-out:file:scorefile {prefix}/{target}.score
-lowres_preoptimize true
-pep_refine true
-flexPepDocking:receptor_chain A
-flexPepDocking:peptide_chain B
-ex1
-ex2aro
-use_input_sc
-multiple_processes_writing_to_one_directory
-no_color
'''.replace('\n', ' ').replace('  ', ' ')

#==> EDIT HERE
nstruct = 2 if debug else 200

#==> EDIT HERE
targets = '1CZY_bound 1CZY_ub 1CZY_ub_bb 2FOJ_bound 2FOJ_ub 2FOJ_ub_bb'.split()
targets = targets[:2] if debug else targets

#print(f'extension: {extension}')
#print(f'command_line: {command_line}')
#print(f'config: {benchmark.config()}')
#print(hpc_driver)

hpc_logs = f'{working_dir}/hpc-logs'
if not os.path.exists(hpc_logs): os.makedirs(hpc_logs)
hpc_job_ids = []
for target in targets:
    ### need to change command flags mid test
    new_cmd_line = command_line
    if target.endswith("ub_bb.pdb"):
        new_cmd_line += " -min_receptor_bb true"
    prefix = f'{working_dir}/output/{target}'
    native = target[:4]+"_bound"
    if not os.path.exists(prefix): os.makedirs(prefix)

    hpc_job_ids.append( hpc_driver.submit_serial_hpc_job(
        name=f'{testname}-{target}',

        #==> EDIT HERE
        executable = f'{rosetta_dir}/source/bin/FlexPepDocking.{extension}',
        arguments = new_cmd_line.format_map(vars()),
        working_dir = prefix,
        jobs_to_queue = min(nstruct, 50),
        log_dir = hpc_logs,
        time=24,
        block=False)
    )


# if not debug:
#     hpc_driver.wait_until_complete(hpc_job_ids, silent=True)
#     time.sleep(64)  # waiting for NFS caching
hpc_driver.wait_until_complete(hpc_job_ids, silent=True)



# Submitting PyRosetta job
# hpc_job_ids.append( hpc_driver.submit_serial_hpc_job(
    # name=f'{testname}-{PyRosetta-example-job}',

    # #==> EDIT HERE, substitute <MyPythonScript.py> and <script-arguments-if-any> with name of your PyRosetta Python script (located inside your test dir) and command line flags for it
    # executable = config["python_virtual_environment"]["python"],
    # arguments = '<MyPythonScript.py> <script-arguments-if-any>',
    # working_dir = prefix,
    # jobs_to_queue = min(nstruct, 50),
    # log_dir = hpc_logs,
    # time=24,
    # block=False)
# )


#==> EDIT HERE
benchmark.save_variables('debug targets nstruct working_dir testname')  # Python black magic: save all listed variable to json file for next script use (save all variables if called without argument)
