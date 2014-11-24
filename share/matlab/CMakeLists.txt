set (prefix ${CMAKE_INSTALL_PREFIX})
set (MPIRUN ${MPIEXEC})
set (MPIRUN_PROC_OPT ${MPIEXEC_NUMPROC_FLAG})
configure_file ( "${CMAKE_CURRENT_SOURCE_DIR}/JEMRIS_seq.m.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/JEMRIS_seq.m")
configure_file ( "${CMAKE_CURRENT_SOURCE_DIR}/JEMRIS_sim.m.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/JEMRIS_sim.m")
configure_file ( "${CMAKE_CURRENT_SOURCE_DIR}/JEMRIS_txrx.m.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/JEMRIS_txrx.m")
configure_file ( "${CMAKE_CURRENT_SOURCE_DIR}/pbs_script.sh.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/pbs_script.sh")


