#!/bin/bash
./ssd_fuse_dut.o /tmp/ssd/ssd_file w 512 256
./ssd_fuse_dut.o /tmp/ssd/ssd_file w 1024 256
./ssd_fuse_dut.o /tmp/ssd/ssd_file w 1024 256
./ssd_fuse_dut.o /tmp/ssd/ssd_file r 2048 0