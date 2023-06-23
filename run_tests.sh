#!/bin/bash

compare_files() {
    if ! cmp -s "$1" "$2"; then
        #echo ""
    #else
        echo "Il test $3 ha fallito!"
    fi
}

for i in {1..111}
do
    inputFile="open_tests/open_$i.txt"
    expectedOutputFile="open_tests/open_$i.output.txt"
    #echo "Testing $i..."
    ./a.out < $inputFile > output.txt
    compare_files output.txt $expectedOutputFile $i
    rm output.txt
    #echo "-----------------------------------"
done

