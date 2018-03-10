make
#clear
./comp test_case_1.lua
make target
./target
dot cfg.dot -Ocfg.svg -Tsvg
#./comp test_case_2.lua
#echo "" | ./comp test_case_3.lua
#echo "8" | ./comp test_case_3.lua
#echo "" | ./comp test_case_4.lua
#echo "6" | ./comp test_case_4.lua
#./comp test_case_5.lua
#./comp test_case_6.lua
#./comp test_case_7.lua
