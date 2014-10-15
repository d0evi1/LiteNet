#!/bin/bash

function main()
{
	ipcrm -M 0x20121101
	ipcrm -M 0x20121102
}


main
