find -maxdepth 1 -type f -print0 | sed 's/\([^\x00]*\)\x00/\1\x00\L\1\x00/g' | xargs -0 --interactive -n2 mv -i
