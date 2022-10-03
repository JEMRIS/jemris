JEMRIS MATLAB GUIs Standalone Executables

1. the standalone execuatable are only available for linux 

2. Install version 9.7 (R2019b) of the MATLAB Runtime in the default directory: /usr/local/MATLAB/MATLAB_Runtime

3. the exectuables are installed under /usr/local/share/jemris/standalone_guis/apps
   - change to your preferred jemris working directory, then run the GUIs with the commands
     >> /usr/local/share/jemris/standalone_guis/apps/run_JEMRIS_seq.sh /usr/local/MATLAB/MATLAB_Runtime/v97
     >> /usr/local/share/jemris/standalone_guis/apps/run_JEMRIS_sim.sh /usr/local/MATLAB/MATLAB_Runtime/v97
     >> /usr/local/share/jemris/standalone_guis/apps/run_JEMRIS_txrx.sh /usr/local/MATLAB/MATLAB_Runtime/v97

4. Shortcuts for Ubuntu desktop
   - copy the files /usr/local/share/jemris/standalone_guis/*.desktop to your Desktop folder
   - right-click each shortcut symbol on the Desktop and allow launching
   - double click will launch the GUIs (be patient, first start might be slow since MATLAB Runtime needs to be started)
   - all GUIs will run in the folder $HOME/jemris_workspace (will be created if not existent)
   - if the "start recon" button is not working in the JEMRIS_sim GUI: 
        - check if recon works from the command line (i.e. docker is installed and conda environment is active)
        - remove the section "# If not running interactively, don't do anything" in $HOME/.bash_profile 
        - this will ensure that the conda environment is activated for the GUI sessions

