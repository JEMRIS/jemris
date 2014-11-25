add_subdirectory (matlab) 

install(DIRECTORY .
        DESTINATION share/jemris
        FILES_MATCHING
        PATTERN "*.xml"
        PATTERN "*.m"
        PATTERN "*.fig"
        PATTERN "*.mat"
        PATTERN "*.h5")
