cd ../share/examples
time ../../src/sanityck . 1
ec=$?
cd ../../src
exit $ec

