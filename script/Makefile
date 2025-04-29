test:
	go test -v .

benchname = .
testname = connect_test
benchtime = 1000000x

bench:
	go test -run -v -bench=${benchname} ./${testname}.go -benchtime=${benchtime}