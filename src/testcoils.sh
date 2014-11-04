cd ../share/examples
time ../../src/sanityck . 3
ec=$?
cd ../../src
exit $ec

