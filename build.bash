#! /bin/bash

project_name="gofish"
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

cflags="-o ${project_name}"
source_list="gofish.c Prompt.c Deck.c GameController.c"
if [ $do_lint -eq 1 ];
then
    echo "lint not yet supported"
    echo "and it probably won't because splint makes me sad"
fi

if [ $do_debug -eq 1 ];
then
    cflags="${cflags} -ggdb"
fi

if [ $do_release -eq 1 ];
then
    clang -Weverything -O2 gofish.c -o gofish
    cflags="${cflags} -Weverything -O2"
fi

if [ $do_standard -eq 1 ];
then
    : # do nothing
fi

clang $cflags $source_list
exit 0
