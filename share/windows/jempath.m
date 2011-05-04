if (strcmp(computer,'PCWIN64'))
    addpath ([winqueryreg('HKEY_LOCAL_MACHINE', 'SOFTWARE\Wow6432Node\Research Centre Juelich\JEMRIS', 'Path'),'matlab']);
else
    addpath ([winqueryreg('HKEY_LOCAL_MACHINE', 'SOFTWARE\Research Centre Juelich\JEMRIS', 'Path'),'matlab']);
end
