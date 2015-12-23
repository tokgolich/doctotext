# Update VERSION file if needed.
branch=`svn info | grep '^URL:' | sed 's|^URL: .*/\(.*\)|\1|'`
if test $branch = "trunk"; then
	ver_main="0.0"
else
	ver_main=$branch
fi
if test `uname` = "Darwin"; then
	base_secs=`date -j -f "%Y %m %d %H %M %S" "2013 12 01 00 00 00" +%s`
else
	base_secs=`date -d 20131201 +%s`
fi
now_secs=`date +%s`
ver_build=`expr \( $now_secs - $base_secs \) / 60 / 60`
old_ver="?"
if test -f VERSION; then
	old_ver=`cat VERSION`
fi
if test $old_ver != $ver_main.$ver_build; then
	printf $ver_main.$ver_build > VERSION
fi
