#!/bin/bash

set -e
script_runner=$(whoami)
exec_path="/usr/local/bin/"
exec_name="pfox"
exec_full_path="${exec_path}${exec_name}"
binary_download_url="https://github.com/diegomacario/PoorFox2/releases/download/1/pfox"

control_c()
{
    echo -en "\n\n*** Exiting ***\n\n"
        exit 1
}

trap control_c SIGINT

echo -e "\n"
echo "############################"
echo "#### Poor Fox Installer ####"
echo "############################"

# check if user is root
if [ $script_runner != "root" ] ; then
  echo -e "\nError: You must execute this script as a normal user with sudo privileges.\n"
  exit 1
fi

echo -e "\nThis script will simply download the binary for Poor Fox and place it under $exec_path\n"
curl -L $binary_download_url -o $exec_name && sudo mv $exec_name $exec_path && chmod +x $exec_full_path

echo -e "\nSuccessfully installed Poor Fox.\n"
