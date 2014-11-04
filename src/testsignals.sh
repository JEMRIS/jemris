cd ../share/examples
time ../../src/sanityck . 2
ec=$?
cd ../../src
exit $ec

