#!/bin/sh

url=`curl -s  --request GET   --url 'https://api.thecatapi.com/v1/images/search?format=json'   --header 'Content-Type: application/json'   --header 'x-api-key: f98e42af-2e7b-4fed-8f89-0f2cb5385b74' | jq -r '.[0].url'`
echo $url
obrazek=`wget $url -O obrazek.jpg`
kot=`img2txt -f utf8 obrazek.jpg > obrazek.txt`
tekst=`curl -s 'http://api.icndb.com/jokes/random' | jq -r '.value.joke' >> obrazek.txt`
clear
wynik=`cat obrazek.txt`
echo "$wynik"
