source build-utils/build.sh
#mkdir cmake-build-debug/coverage
#lcov -c -i -d cmake-build-debug/src -o cmake-build-debug/coverage/base.info
#ctest --test-dir cmake-build-debug/test --rerun-failed --output-on-failure
cd ./cmake-build-debug/test || exit
./test-surviving-sarntal
cd ..
cmake --build . --target coverage
#mkdir coverage
#lcov --rc geninfo_unexecuted_blocks=1 --capture --directory src --output-file coverage/test.info
#lcov -a coverage/base.info -a coverage/test.info -o coverage/coverage.info
#lcov --remove coverage/coverage.info '/usr/*' --output-file coverage/coverage.info
#lcov --remove coverage/coverage.info 'cmake-build-debug/*' --output-file coverage/coverage.info
#genhtml coverage/coverage.info --output-directory coverage/report
cd ..