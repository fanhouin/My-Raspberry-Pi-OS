#!/bin/bash

SSD_FILE="/tmp/ssd/ssd_file"
GOLDEN="/tmp/ssd_file_golden"
TEMP="/tmp/temp"
rm -rf ${GOLDEN} ${TEMP}
touch ${GOLDEN}
truncate -s 0 ${SSD_FILE}
truncate -s 0 ${GOLDEN}

rand(){
    min=$1
    max=$(($2-$min))
    num=$(cat /dev/urandom | head -n 10 | cksum | awk -F ' ' '{print $1}')
    echo $(($num%$max))
}

case "$1" in
    ## for gc
    "test1")
        cat /dev/urandom | tr -dc '[:alpha:][:digit:]' | head -c 11264 > ${TEMP}
        for i in $(seq 0 9)
        do
            dd if=${TEMP} iflag=skip_bytes skip=$(($i*1024)) of=${GOLDEN} oflag=seek_bytes seek=$(($i*5120)) bs=1024 count=1 conv=notrunc 2> /dev/null
            dd if=${TEMP} iflag=skip_bytes skip=$(($i*1024)) of=${SSD_FILE} oflag=seek_bytes seek=$(($i*5120)) bs=1024 count=1 conv=notrunc 2> /dev/null
        done

        cat /dev/urandom | tr -dc '[:alpha:][:digit:]' | head -c 11264 > ${TEMP}
        for i in $(seq 0 9)
        do
            dd if=${TEMP} iflag=skip_bytes skip=$(($i*1024)) of=${GOLDEN} oflag=seek_bytes seek=$(($i*5120 + 1024)) bs=1024 count=1 conv=notrunc 2> /dev/null
            dd if=${TEMP} iflag=skip_bytes skip=$(($i*1024)) of=${SSD_FILE} oflag=seek_bytes seek=$(($i*5120 + 1024)) bs=1024 count=1 conv=notrunc 2> /dev/null
        done
        
        
        dd if=${TEMP} iflag=skip_bytes skip=10240 of=${GOLDEN} oflag=seek_bytes seek=0 bs=1024 count=1 conv=notrunc 2> /dev/null
        dd if=${TEMP} iflag=skip_bytes skip=10240 of=${SSD_FILE} oflag=seek_bytes seek=0 bs=1024 count=1 conv=notrunc 2> /dev/null

        cat /dev/urandom | tr -dc '[:alpha:][:digit:]' | head -c 51200 | tee ${SSD_FILE} > ${GOLDEN} 2> /dev/null
        cat /dev/urandom | tr -dc '[:alpha:][:digit:]' | head -c 11264 > ${TEMP}
        for i in $(seq 0 9)
        do
            dd if=${TEMP} iflag=skip_bytes skip=$(($i*1024)) of=${GOLDEN} oflag=seek_bytes seek=$(($i*5120)) bs=1024 count=1 conv=notrunc 2> /dev/null
            dd if=${TEMP} iflag=skip_bytes skip=$(($i*1024)) of=${SSD_FILE} oflag=seek_bytes seek=$(($i*5120)) bs=1024 count=1 conv=notrunc 2> /dev/null
        done

        cat /dev/urandom | tr -dc '[:alpha:][:digit:]' | head -c 11264 > ${TEMP}
        for i in $(seq 0 9)
        do
            dd if=${TEMP} iflag=skip_bytes skip=$(($i*1024)) of=${GOLDEN} oflag=seek_bytes seek=$(($i*5120 + 1024)) bs=1024 count=1 conv=notrunc 2> /dev/null
            dd if=${TEMP} iflag=skip_bytes skip=$(($i*1024)) of=${SSD_FILE} oflag=seek_bytes seek=$(($i*5120 + 1024)) bs=1024 count=1 conv=notrunc 2> /dev/null
        done
        
        
        dd if=${TEMP} iflag=skip_bytes skip=10240 of=${GOLDEN} oflag=seek_bytes seek=0 bs=1024 count=1 conv=notrunc 2> /dev/null
        dd if=${TEMP} iflag=skip_bytes skip=10240 of=${SSD_FILE} oflag=seek_bytes seek=0 bs=1024 count=1 conv=notrunc 2> /dev/null

        ;;
    
    # for unalign write
    "test2") 
        cat /dev/urandom | tr -dc '[:alpha:][:digit:]' | head -c 11264 > ${TEMP}
        dd if=${TEMP} iflag=skip_bytes skip=123 of=${GOLDEN} oflag=seek_bytes seek=123 bs=1024 count=1 conv=notrunc 2> /dev/null
        dd if=${TEMP} iflag=skip_bytes skip=123 of=${SSD_FILE} oflag=seek_bytes seek=123 bs=1024 count=1 conv=notrunc 2> /dev/null

        dd if=${TEMP} iflag=skip_bytes skip=3000 of=${GOLDEN} oflag=seek_bytes seek=3000 bs=72 count=20 conv=notrunc 2> /dev/null
        dd if=${TEMP} iflag=skip_bytes skip=3000 of=${SSD_FILE} oflag=seek_bytes seek=3000 bs=72 count=20 conv=notrunc 2> /dev/null   
        
        dd if=${TEMP} iflag=skip_bytes skip=777 of=${GOLDEN} oflag=seek_bytes seek=3100 bs=484 count=1 conv=notrunc 2> /dev/null
        dd if=${TEMP} iflag=skip_bytes skip=777 of=${SSD_FILE} oflag=seek_bytes seek=3100 bs=484 count=1 conv=notrunc 2> /dev/null  

        dd if=${TEMP} iflag=skip_bytes skip=30 of=${GOLDEN} oflag=seek_bytes seek=512 bs=1078 count=1 conv=notrunc 2> /dev/null
        dd if=${TEMP} iflag=skip_bytes skip=30 of=${SSD_FILE} oflag=seek_bytes seek=512 bs=1078 count=1 conv=notrunc 2> /dev/null

        dd if=${TEMP} iflag=skip_bytes skip=98 of=${GOLDEN} oflag=seek_bytes seek=1033 bs=1 count=499 conv=notrunc 2> /dev/null
        dd if=${TEMP} iflag=skip_bytes skip=98 of=${SSD_FILE} oflag=seek_bytes seek=1033 bs=1 count=499 conv=notrunc 2> /dev/null
        
        dd if=${TEMP} iflag=skip_bytes skip=999 of=${GOLDEN} oflag=seek_bytes seek=2987 bs=3 count=222 conv=notrunc 2> /dev/null
        dd if=${TEMP} iflag=skip_bytes skip=999 of=${SSD_FILE} oflag=seek_bytes seek=2987 bs=3 count=222 conv=notrunc 2> /dev/null
        ;;

    ## for GC valid_count[0,1] = 0
    "test3")
        cat /dev/urandom | tr -dc '[:alpha:][:digit:]' | head -c 11264 > ${TEMP}
        for i in $(seq 0 9)
        do
            dd if=${TEMP} iflag=skip_bytes skip=$(($i*1024)) of=${GOLDEN} oflag=seek_bytes seek=$(($i*5120)) bs=1024 count=1 conv=notrunc 2> /dev/null
            dd if=${TEMP} iflag=skip_bytes skip=$(($i*1024)) of=${SSD_FILE} oflag=seek_bytes seek=$(($i*5120)) bs=1024 count=1 conv=notrunc 2> /dev/null
        done
        cat /dev/urandom | tr -dc '[:alpha:][:digit:]' | head -c 11264 > ${TEMP}
        for i in $(seq 0 9)
        do
            dd if=${TEMP} iflag=skip_bytes skip=$(($i*1024)) of=${GOLDEN} oflag=seek_bytes seek=$(($i*5120)) bs=1024 count=1 conv=notrunc 2> /dev/null
            dd if=${TEMP} iflag=skip_bytes skip=$(($i*1024)) of=${SSD_FILE} oflag=seek_bytes seek=$(($i*5120)) bs=1024 count=1 conv=notrunc 2> /dev/null
        done
        
        dd if=${TEMP} iflag=skip_bytes skip=10240 of=${GOLDEN} oflag=seek_bytes seek=0 bs=1024 count=1 conv=notrunc 2> /dev/null
        dd if=${TEMP} iflag=skip_bytes skip=10240 of=${SSD_FILE} oflag=seek_bytes seek=0 bs=1024 count=1 conv=notrunc 2> /dev/null

        cat /dev/urandom | tr -dc '[:alpha:][:digit:]' | head -c 51200 | tee ${SSD_FILE} > ${GOLDEN} 2> /dev/null
        ;;


    *)
        printf "Usage: sh test.sh test_pattern\n"
        printf "\n"
        printf "test_pattern\n"
        return 
        ;;
esac

# check
diff ${GOLDEN} ${SSD_FILE}
if [ $? -eq 0 ]
then
    echo "success!"
else
    echo "fail!"
fi

echo "WA:"
./ssd_fuse_dut /tmp/ssd/ssd_file W
#rm -rf ${TEMP}