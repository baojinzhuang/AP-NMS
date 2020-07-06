#!/bin/sh

echo "正在执行的文件名：$0";
echo "服务器地址：$1";
echo "目标文件位置：$2";
echo "本机位置：$3";

tftp -4  $1 -c put $3 $2
if [ $? -ne 0 ]; then
    echo "put failed"
else
    echo "put succeed"
fi

