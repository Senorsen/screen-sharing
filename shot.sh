#!/bin/sh

while true
do
	scrot scr.png
    convert -quality 25 scr.png scr.tmp.jpg
    mv scr.tmp.jpg scr.jpg
	date
	sleep 0.02
done

