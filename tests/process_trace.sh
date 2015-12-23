if test $# -ne 2; then
	echo >&2;
	echo "Usage: process_trace.sh <executable> <trace_file>" >&2;
	echo >&2;
	exit 1;
fi
if ! test -f $1; then
	echo >&2;
	echo "Error: Executable file $1 not found." >&2;
	echo >&2;
	exit 2;
fi
if ! test -f $2; then
	echo >&2;
	echo "Error: Trace file $2 not found." >&2;
	echo >&2;
	exit 3;
fi
if file $1 | grep "PE32+" > /dev/null; then
	ADDR2LINE=x86_64-w64-mingw32-addr2line
elif file $1 | grep "PE32" > /dev/null; then
	ADDR2LINE=i686-w64-mingw32-addr2line 
else
	ADDR2LINE=addr2line
fi
if ! $ADDR2LINE -v > /dev/null 2>&1; then
	echo >&2;
	echo "Error: $ADDR2LINE command not found." >&2;
	echo >&2;
	exit 4;
fi
cat $2 | while read level type func caller
do
	for l in `seq $level`; do
		echo -n -e "\t"
	done
	if test $type = 'C'; then
		echo -n "CALL "
	fi
	if test $type = 'R'; then
		echo -n "EXIT "
	fi
	$ADDR2LINE -f -C -p -e $1 $func
done
