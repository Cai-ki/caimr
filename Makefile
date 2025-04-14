source_dir = .
build_dir = ${source_dir}/build

build-all:
	rm -rf ./build
	cmake -G "Unix Makefiles" \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	-DCMAKE_C_COMPILER=gcc \
	-DCMAKE_CXX_COMPILER=g++ \
	-S ${source_dir} -B ${build_dir}

	cmake --build ./build 

clang-format:
	clang-format --style=Google --dump-config > ./.clang-format