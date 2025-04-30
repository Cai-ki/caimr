SOURCE_DIR = .
BUILD_DIR = ${SOURCE_DIR}/build

build-all:
	rm -rf ${BUILD_DIR}
	cmake -G "Unix Makefiles" \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	-DCMAKE_C_COMPILER=gcc \
	-DCMAKE_CXX_COMPILER=g++ \
	-S ${SOURCE_DIR} -B ${BUILD_DIR}

	cmake --build ${BUILD_DIR}

testname = tcp_server_test

build-test:
	g++ -std=c++14 -g -I./include -L./build -Wl,-rpath,./build ./test/${testname}.cpp -lcaimr -lpthread -o ./test/${testname}

run-test:
	./test/${testname}

clang-format:
	clang-format --style=Google --dump-config > ${SOURCE_DIR}/.clang-format