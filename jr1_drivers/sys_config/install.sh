#!/bin/bash
# install the system config scripts
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
sudo cp ${DIR}/*.rules /etc/udev/rules.d
