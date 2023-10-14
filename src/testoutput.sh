cd ../share/examples
time ../../src/sanityck . 4
ec=$?
cd ../../src
exit $ec

