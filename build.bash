#! /bin/bash

project_name="gofish"
target_debug="debug"
target_release="release"
target_lint="lint"

do_debug=0
do_release=0
do_lint=0
do_standard=1
should_build=0

function debug() {
    echo "doing debug stuff"
    do_standard=0
    do_release=0
    do_debug=1
    if [ $do_release -eq 1 ];
    then
        echo "WARNING: debug and release are mutually exclusive"
    fi
    should_build=1
}

function release() {
    echo "doing release stuff"
    do_standard=0
    do_debug=0
    do_release=1
    if [ $do_debug -eq 1 ];
    then
        echo "WARNING: release and debug are mutually exclusive"
    fi
    should_build=1
}

function lint() {
    echo "doing lint stuff"
    do_lint=1
    should_build=0
}

should_build=1

for var in "$@"
do
    case "$var" in
        $target_debug)
            debug
            ;;

        $target_release)
            release
            ;;

        $target_lint)
            lint
            ;;

        *)
            echo "unknown option $var"
            exit 1
    esac
done

cflags="-o ${project_name} -std=c89"
source_list="gofish.c Prompt.c Deck.c GameController.c Player.c"

if [ $should_build -eq 1 ];
then
	if [ $do_debug -eq 1 ];
	then
	    cflags="${cflags} -ggdb"
	fi
	
	if [ $do_release -eq 1 ];
	then
	    cflags="${cflags} -Weverything -O2"
	fi
	
	if [ $do_standard -eq 1 ];
	then
	    : # do nothing
	fi
	
	clang $cflags $source_list

else
	if [ $do_lint -eq 1 ];
	then
		splint -Dlint $source_list
	fi
fi

exit 0

