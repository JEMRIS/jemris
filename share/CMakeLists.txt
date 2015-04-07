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

