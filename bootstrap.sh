sudo apt-get update
sudo apt-get install -y git g++ libboost-math-dev
sudo apt-get install -y libhdf5-dev libhdf5-serial-dev
sudo apt-get install -y libginac-dev libginac2 libginac2-dbg ginac-tools
sudo apt-get install -y libxerces-c3.1 libxerces-c-dev libsundials-cvode1 libsundials-serial-dev
sudo apt-get install -y mpich

cd ~
if [ -d "jemris_src" ]; then
  sudo rm -rf jemris_src
fi
mkdir ~/jemris_src
cd jemris_src
git clone /vagrant .

./configure
make && make install

jemris modlist 