#!/bin/bash
set -e

kernel_version="`modinfo -Fvermagic ./nnt_driver_ppc.ko | awk '{ print $1 }'`"
path_to_build="`pwd`"
path_to_build="$path_to_build/../build"
cd $path_to_build

mkdir -p /etc/mft/mlxfwreset/$kernel_version
/bin/cp -f ./nnt_driver_ppc.ko /etc/mft/mlxfwreset/$kernel_version/

cd -

