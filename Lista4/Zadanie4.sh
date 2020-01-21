#!/bin/sh

strona=$1
czas=$2
i=1
w3m -dump $strona > strona_przed.txt
sleep $czas

while true; 
do
	w3m -dump $strona > strona_po.txt
	diff -c strona_przed.txt strona_po.txt > zmiana$i.txt
	zmiana=`diff strona_przed.txt strona_po.txt`
	if [ -n "$zmiana" ];
	then 
		echo "Na stronie pojawiły się zmiany"
			git add zmiana$i.txt
			git commit -m "Zmiana numer $i"
			git push -f https://github.com/elizazawisza/AKiSO_Zadanie4.git
	else 
		echo "Nie ma żadnych zmian"
	fi
	zamianastron=`cp strona_po.txt strona_przed.txt`
	rm zmiana$i.txt
	let i=i+1
	sleep $czas
done
