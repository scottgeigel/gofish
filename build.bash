#! /bin/bash

target_debug="debug"
target_release="release"
target_lint="lint"

do_debug=0
do_release=0
do_lint=0
do_standard=1

function debug() {
    echo "doing debug stuff"
    do_standard=0
    do_release=0
    do_debug=1
    if [ $do_release -eq 1 ];
    then
        echo "WARNING: debug and release are mutually exclusive"
    fi
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
}

function lint() {
    echo "doing lint stuff"
    do_lint=1
}

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

if [ $do_lint -eq 1 ];
then
    echo "lint not yet supported"
fi

if [ $do_debug -eq 1 ];
then
    clang -ggdb gofish.c -o gofish
fi

if [ $do_release -eq 1 ];
then
    clang -Weverything -O2 gofish.c -o gofish
fi

if [ $do_standard -eq 1 ];
then
    clang gofish.c -o gofish
fi
exit 0
