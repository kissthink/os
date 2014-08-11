cd src
make clean install
mv os ../
cd ..
strip --strip-all os
