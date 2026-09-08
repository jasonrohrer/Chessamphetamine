sed -i "s/unsigned long/unsigned int/g" *.h *.c
sed -i "s/ long / int /g" *.h *.c
sed -i "s/(long)/(int)/g" *.h *.c
#sed -i "s/UL/U/g" *.h *.c
sed -i "s/long mingin_/int mingin_/g" mingin.h
sed -i "s/int            result  = syscall/long           result  = syscall/" mingin.h
sed -i  "s/unsigned int            xBlackColor/unsigned long           xBlackColor/" mingin.h
