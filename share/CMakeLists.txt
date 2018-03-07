configure_file (
        "${PROJECT_SOURCE_DIR}/share/matlab/JEMRIS_sim.m.in"
        "${PROJECT_BINARY_DIR}/share/matlab/JEMRIS_sim.m"
        @ONLY
)
configure_file (
        "${PROJECT_SOURCE_DIR}/share/matlab/JEMRIS_seq.m.in"
        "${PROJECT_BINARY_DIR}/share/matlab/JEMRIS_seq.m"
        @ONLY
)

configure_file (
        "${PROJECT_SOURCE_DIR}/share/matlab/JEMRIS_txrx.m.in"
        "${PROJECT_BINARY_DIR}/share/matlab/JEMRIS_txrx.m"
        @ONLY
)

install(DIRECTORY "${PROJECT_BINARY_DIR}/share/matlab"
        DESTINATION share/jemris
        FILES_MATCHING
        PATTERN "*.xml"
        PATTERN "*.m"
        PATTERN "*.fig"
        PATTERN "*.mat"
        PATTERN "*.h5")

install(DIRECTORY "matlab"
        DESTINATION share/jemris
        FILES_MATCHING
        PATTERN "*.xml"
        PATTERN "*.m"
        PATTERN "*.fig"
        PATTERN "*.mat"
        PATTERN "*.h5")

install(DIRECTORY "examples"
        DESTINATION share/jemris
        FILES_MATCHING
        PATTERN "*.xml"
        PATTERN "approved" EXCLUDE)

INSTALL(FILES examples/ext_gx.h5 examples/ext_rf.h5 examples/sens.h5
  DESTINATION share/jemris/examples)

install(DIRECTORY "angio_simu"
        DESTINATION share/jemris
	PATTERN "Makefile*" EXCLUDE)

