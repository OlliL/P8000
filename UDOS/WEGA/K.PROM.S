!******************************************************************************
*******************************************************************************
 
	U D O S - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1989
 
	pburn Modul: K.PROM.S
 
 
	Bearbeiter:	P. Hoge, R. Kuehle
 
	Datum:		18.4.1989
	Version:	2.1
 
*******************************************************************************
******************************************************************************!

 
PROM module
 
constant
	rd := 1
	wt := 2
	false := 0
 
external
	intppb procedure (byte byte)
	bcoff procedure			!turns off programmer!
	rbyte procedure (word) returns (byte)
	wbyte procedure (word byte)
	p2708 procedure (word)
 
	lchar procedure returns (byte)
	schar procedure (byte)
	ueb16 procedure
	sdlc procedure (byte byte byte) returns (byte byte)
	BUF array [%4000 byte]
	old byte		!0 		- board 2
				 ungleich 0 	- board 1!
 
global
 
	errflg byte
	Vpp	byte		!programmierspannung	1 - 25   V
							2 - 21   V
							3 - 12.5 V
							4 - free V!
	PM	byte		!program mode		0 - high speed
							1 - normal speed!
 
internal
	ptype byte
	begadr word
	nbytes word
	promadr word
	char, read_write byte
	k byte
	i, len word
	d, e byte
 
 
internal
 
readprom procedure
    entry
	intppb(rd ptype)
	if errflg<>false then return fi
	i := 0
	do
		ueb16
	   	BUF[i] := rbyte(promadr)
		if errflg<>false then exit fi
		len := len-1
		if len=0 then exit fi
	   	promadr := promadr+1
		i := i+1
	od
	bcoff
end readprom
 
writeprom procedure
    entry
	intppb(wt ptype)
	if errflg<>false then return fi
	promadr:=begadr
	len:=nbytes
	if ptype=0 then p2708(nbytes)
   	else
		i := 0
   		do
	 		wbyte(promadr,BUF[i])
 
			if errflg=3 then	schar (3)
						char:=lchar
						if char='n' then	exit
							    else	errflg:=0
						fi
			fi
			if errflg<>false then exit fi
			len := len-1
	   		if len=0 then exit fi
	   		promadr := promadr+1
			i := i+1
   		od
	fi
	bcoff
end writeprom
 
 
! anpassung schnittstelle 16-bit/8-bit p8000 fuer burn (wega)
  *********************************************************** !
 
global
prom procedure
    entry
	errflg:=false
 
	! read, write, erase, crc !
	read_write:=lchar
 
	! type prom !
	ptype:=lchar
 
	! beginn adresse prom !
	char:=lchar
	begadr:=word char		!high!
	begadr:=begadr * 256
	char:=lchar
	begadr:=begadr or word char	!low!
	promadr:=begadr
 
	! laenge prom !
	char:=lchar
	nbytes:=word char		!high!
	nbytes:=nbytes * 256
	char:=lchar
	nbytes:=nbytes or word char	!low!
	len:=nbytes
 
	!programmierspannung Vpp!
	Vpp:=lchar
 
	!program mode!
	PM:=lchar
 
	schar('q')			!acknowledge!
 
	! initialisierung EPROM-Programmer Board Abfrage !
	if read_write=5 then intppb (0 0) fi
 
	i:=0
 
	! write prom
	  ********* !
 
	if read_write=2 then
		do
			k:=64
			do
				BUF[i]:=lchar
				i:=i+1
				k:=k-1
				len:=len-1
				if len=0 then exit fi
				if k=0 then exit fi
			od
			schar('q')
			if len=0 then exit fi
		od
		writeprom
		if errflg<>3 then schar (errflg) fi
 
		! ruecklesen prom; feststellen programmierfehler;
		  uebergabe an wega bei Standard Speed Program Mode !
 
		if errflg=false then	if PM=1 then
						promadr:=begadr
						len:=nbytes
						i:=0
						intppb(rd ptype)
						do
							ueb16
							char:=rbyte(promadr)
							if errflg<>false then exit fi
							if char<>BUF[i] then errflg:=3 exit fi
							len := len-1
							if len=0 then exit fi
							promadr:=promadr+1
							i:=i+1
						od
						bcoff
						schar(errflg)
					fi
		fi
	fi
 
	! read prom
          ********* !
 
	if read_write=1 then
		readprom
		schar(errflg)
		if errflg=false then
			i := 0
			len:=nbytes
			do
				k:=64
				do
					schar(BUF[i])
					i:=i+1
					k:=k-1
					len:=len-1
					if len=0 then exit fi
					if k=0 then exit fi
				od
				char:=lchar
				if len=0 then exit fi
			od
		fi
	fi
 
	! erase prom
          ********** !
 
	if read_write=3 then
		intppb(rd ptype)
		do
			ueb16
			char:=rbyte(promadr)
			if errflg<>false then exit fi
			if char<>%FF then errflg:=3 exit fi
			len:=len-1
			if len=0 then exit fi
			promadr:=promadr+1
		od
		bcoff
		schar(errflg)
	fi
 
	! crc prom
	  ******** !
	if read_write=4 then
		d:=%ff
		e:=%ff
		k:=0
		intppb(rd ptype)
		do
			ueb16
			char:=rbyte(promadr)
			if errflg<>false then k:=errflg exit fi
			if char<>%FF then k:=3 fi
			d,e:=sdlc(char,d,e)
			len:=len-1
			if len=0 then exit fi
			promadr:=promadr+1
		od
		bcoff
		schar(k)
		if errflg=false then
			schar(d)
			schar(e)
		fi
	fi
 
	!prom programmer board
	 *********************!
 
	if read_write=5 then
		if old=0 then schar('2')
			 else schar('1')
		fi
	fi
 
end prom
 
end PROM
