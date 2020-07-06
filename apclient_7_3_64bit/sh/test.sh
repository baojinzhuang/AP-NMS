#!/bin/sh

sudo apt-get update
 
if [ $? -ne 0 ]; then
    echo "update failed"
else
    echo "update succeed"
fi

cd ~/Downdloads

if [ $? -ne 0 ]; then
    echo "cd failed"
else
    echo "cd succeed"
fi
