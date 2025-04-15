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

clang-format:
	clang-format --style=Google --dump-config > ${SOURCE_DIR}/.clang-format