LD_LIBRARY_PATH=../build find "$1" -type f -fprintf /dev/stderr "Processing: %p\n" -exec valgrind ../build/doctotext \{\} \; 2>&1 > /dev/null | grep "Processing\|ERROR SUMMARY"
