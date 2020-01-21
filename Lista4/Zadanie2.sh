#!/bin/sh

{ printf "Name:     Status:   PID: \t Ilosc plikow: \n" 
for file in `ls /proc/*[0-9]/status`
do
if [ -e $file ]; then 
zmienna=`echo $file | cut -d"/" -f3`
printf "` sed -n '1p' $file | awk '{print$2}'`\t\t `sed -n '3p' $file | awk '{print$2}' `\t\t  `sed -n '11p' $file | awk '{print$2}'` \t\t `lsof -p $zmienna | wc -l` \n"
fi
done } | column -t
