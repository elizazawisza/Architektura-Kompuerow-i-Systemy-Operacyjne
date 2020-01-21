#!/bin/sh
wyslana1=0;
odebrana1=0;
i=0;
sredniaodebrana=0;
sredniawyslana=0;
tput clear
while :
do
	tput sc
	dni=`echo $(awk '{print $1}' /proc/uptime) /86400 | bc`
	godziny=`echo $(awk '{print $1}' /proc/uptime) /3600 | bc`
	minuty=`echo $(awk '{print $1}' /proc/uptime) /60 | bc`
	sekundy=`echo $(awk '{print $1}' /proc/uptime) /1 | bc`
	bateria=`cat /sys/class/power_supply/BAT0/capacity`
	obcmin=`cut -f1 -d' ' /proc/loadavg`
	obc5min=`cut -f2 -d' ' /proc/loadavg`
	obc15min=`cut -f3 -d' ' /proc/loadavg`
	wyslana2=`sed -n '4p' /proc/net/dev | awk '{print $10}'`
	odebrana2=`sed -n '4p' /proc/net/dev | awk '{print $2}'`
	let predkoscwyslana="($wyslana2-$wyslana1)"
	let predkoscodebrana=odebrana2-odebrana1
	let sredniaodebrana="($sredniaodebrana*$i+$predkoscodebrana)/($i+1)"
	let sredniawyslana="($sredniawyslana*$i+$predkoscwyslana)/($i+1)"

	echo "System jest już uruchominy: $dni dni, $godziny godzin, $minuty minut, $sekundy sekund"  
	echo "Stan baterii: $bateria %"

	echo "CPU: $obcmin [ostatnia minuta] $obc5min [ostatnie 5 min], $obc15min [ostatnie 15 minut]"

	echo "Wysyłanie: "

	if [ $predkoscwyslana -lt 8000 ]; 
	then
		echo "Aktualna prędkość: $(tput el) `echo "scale=2; $predkoscwyslana/8" |bc -l` B"
	elif [ $predkoscwyslana -gt 7999 -a $predkoscwyslana -lt 8000000 ];
	then 
		echo "Aktualna prędkość: $(tput el) `echo "scale=2; $predkoscwyslana/8000" |bc -l` kB"
	else
		echo "Aktualna prędkość: $(tput el) `echo "scale=2; $predkoscwyslana/8000000" |bc -l` MB"
	fi

	if [ $sredniawyslana -lt 8000 ]; 
	then
		echo "Średnia prędkość: $(tput el) `echo "scale=2; $sredniawyslana/8" |bc -l` B"
	elif [ $sredniawyslana -gt 7999 -a $sredniawyslana -lt 8000000 ];
	then 
		echo "Średnia prędkość: $(tput el) `echo "scale=2; $sredniawyslana/8000" |bc -l` kB"
	else
		echo "Średnia prędkość: $(tput el) `echo "scale=2; $sredniawyslana/8000000" |bc -l` MB"
	fi

	
	echo "Odbieranie:"
	if [ $predkoscodebrana -lt 8000 ]; 
	then
		echo "Aktualna prędkość: $(tput el) `echo "scale=2; $predkoscodebrana/8" |bc -l` B"
	elif [ $predkoscodebrana -gt 7999 -a $predkoscodebrana -lt 8000000 ];
	then 
		echo "Aktualna prędkość: $(tput el) `echo "scale=2; $predkoscodebrana/8000" |bc -l` kB"
	else
		echo "Aktualna prędkość: $(tput el) `echo "scale=2; $predkoscodebrana/8000000" |bc -l` MB"
	fi

	if [ $sredniaodebrana -lt 8000 ]; 
	then
		echo "Średnia prędkość: $(tput el) `echo "scale=2; $sredniaodebrana/8" |bc -l` B"
	elif [ $sredniaodebrana -gt 7999 -a $sredniaodebrana -lt 8000000 ];
	then 
		echo "Średnia prędkość: $(tput el) `echo "scale=2; $sredniaodebrana/8000" |bc -l` kB"
	else
		echo "Średnia prędkość: $(tput el) `echo "scale=2; $sredniaodebrana/8000000" |bc -l` MB"
	fi


	let wyslana1=wyslana2
	let odebrana1=odebrana2
	let i=i+1
	
	sleep 1
	tput rc 
	
	

done

