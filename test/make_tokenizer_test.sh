#!/bin/bash
exe=tokenizer_test
inc=../

cmd=$1
if [[ -z "${cmd}" ]]; then
    # nothing passed means we'll do a build+run
    cmd=br
fi

# build
if [[ "$cmd" == "br" || "$cmd" == "b" ]]; then
  g++ ./tokenizer_test.cpp -o $exe -I $inc
fi

# run
if [[ "$cmd" == "br" || "$cmd" == "r" ]]; then
  ./$exe
fi
