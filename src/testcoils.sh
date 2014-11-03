cd ../share/examples
time ../../src/sanityck . 3
ec=$?
cd ../../src
echo "EXIT CODE is" $ec
exit $ec
