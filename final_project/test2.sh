#!/bin/bash
for i in `seq 20`; do
    # echo $((RANDOM))
    idx=$(((RANDOM % 99) * 512))
    ./ssd_fuse_dut.o /tmp/ssd/ssd_file w 512 $idx
done

