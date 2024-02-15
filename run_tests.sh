#!/bin/bash

compare_files() {
    if ! cmp -s "$1" "$2"; then
        #echo ""
    #else
        echo "Il test $3 ha fallito!"
    fi
}

/usr/bin/gcc -Wall -Werror -g3 -fdiagnostics-color=always -g source.c -o a.out
if [ $? -ne 0 ]; then
    echo "Errore di compilazione"
    exit 1
else echo "Compilazione avvenuta con successo"
fi

for i in {1..112}
do
    inputFile="open_tests/open_$i.txt"
    expectedOutputFile="open_tests/open_$i.output.txt"
    echo "Testing $i..."
    start_time=$(date +%s.%N)
    ./a.out < $inputFile > output.txt
    end_time=$(date +%s.%N)
    elapsed_time=$(echo "$end_time - $start_time" | bc)
    compare_files output.txt $expectedOutputFile $i $elapsed_time
    rm output.txt
done

