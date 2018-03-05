if (strcmp(computer,'PCWIN64'))
    addpath ([winqueryreg('HKEY_LOCAL_MACHINE', 'SOFTWARE\Wow6432Node\JEMRIS', 'Path'),'matlab']);
else
    addpath ([winqueryreg('HKEY_LOCAL_MACHINE', 'SOFTWARE\JEMRIS', 'Path'),'matlab']);
end
