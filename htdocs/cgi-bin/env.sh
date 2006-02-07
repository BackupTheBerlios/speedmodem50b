#!/bin/sh
echo content-type: text/plain

$( env | sort )
