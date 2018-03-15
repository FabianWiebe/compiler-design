#!/bin/bash
make
for i in `seq 5 5`;
do
	echo "test case $i:"
	./comp test_case_$i.lua
	gcc target.s -no-pie -o target -g
	echo "10" | ./target
	#echo "10" | lua5.3 test_case_$i.lua
	dot cfg.dot -Ocfg$i.svg -Tsvg
	echo ""
done
#./comp test_case_2.lua
#echo "" | ./comp test_case_3.lua
#echo "8" | ./comp test_case_3.lua
#echo "" | ./comp test_case_4.lua
#echo "6" | ./comp test_case_4.lua
#./comp test_case_5.lua
#./comp test_case_6.lua
#./comp test_case_7.lua
