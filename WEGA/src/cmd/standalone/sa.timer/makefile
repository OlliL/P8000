all:	sa.timer kern

sa.timer:
	cd standalone ; make all
instsa:
	cd standalone ; make install
	
kern:
	cd kernel ; make all

instkern:
	cd kernel ; make install
	
clean:	
	cd standalone ; make clean
	cd kernel ; make clean
