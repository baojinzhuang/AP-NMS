#!/bin/sh

echo "正在执行的文件名：$0";
echo "服务器地址：$1";
echo "目标文件位置：$2";
echo "本机存入位置：$3";

tftp -4  $1 -c get $2 
if [ $? -ne 0 ]; then
    echo "update failed"
else
    echo "update succeed"
fi

sleep 5s
if [ $? -ne 0 ]; then
    echo "up failed"
else
    echo "up succeed"
fi
