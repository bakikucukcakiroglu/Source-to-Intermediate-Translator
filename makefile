all:		mylang2ir

mylang2ir:	mylang2ir.o 
		g++ mylang2ir.o -o mylang2ir
		rm *.o

mylang2ir.o:	230_Project1.cpp
		gcc -c 230_Project1.cpp -o mylang2ir.o

