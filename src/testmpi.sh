echo
echo "Test Case 4: testing parallel execution"
echo "======================================="
mpiexec -np 3 ./pjemris ../share/examples/approved/mpi_simu.xml > mpirun.out
cat mpirun.out
echo
if test `grep Finished mpirun.out` = Finished ; then \
  echo Success; \
else \
  echo Error; \                                                                                                                                                    
fi
echo

