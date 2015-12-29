;----------------------------------------------------------------------
;  ***           PROGRAMMIERUNG DER WDC-KARTE                   ***
;  ***              Hard-Disk (Winchester)                      ***
;----------------------------------------------------------------------
; CPU Z80A, CTC, 8K PROM, 6K RAM, Host-Schnittstelle, Disk-Schnittstelle
; Programmierung fuer die Arbeit mit bis zu drei Laufwerken
;----------------------------------------------------------------------

	begin	0

	jr	entry
string	text	'WDC_4.21'

sc_len	equ	512	;Laenge des Datenfeldes eines Sektors (Byte)
 
; I/O-Adressen
CTC	equ	70h	;Grundadresse des CTC
DSKEA	equ	08h	;Disk-Endadresse (8282)
CNTST	equ	18h	;Steuer- und Statusport (OUT-Port, 8212)
BM_T	equ	28h	;Port fuer die Ausgabe des Taktbitmuster fuer Marken
BM_D	equ	38h	;Port fuer die Ausgabe des Datenbitmusters fuer Marken
DSKC1	equ	48h	;Disk-Schnittstellenport 1 (8282)
DSKC2	equ	58h	;Disk-Schnittstellenport 2 (8212)
IMPAUS	equ	68h	;Port zum Ausschalten der DSS
IMPATV	equ	78h	;Impuls /ATV (Anzeige Takt vorwaerts)
ST_PRT	equ	88h	;Status-Port (IN-Port, DL541)

;-----------------------------------------------------------------------------
; PROM-Adressen
;-----------------------------------------------------------------------------
crc_0	equ	1ff8h	;CRC-Wert EPROM 0
crc_0n	equ	1ffah	;CRC-Wert EPROM 0 negiert
crc_1	equ	1ffch	;CRC-Wert EPROM 1
crc_1n	equ	1ffeh	;CRC-Wert EPROM 1 negiert

;-----------------------------------------------------------------------------
; RAM-Adressen
;-----------------------------------------------------------------------------
; Adressen aus dem Lese-Schreibbereich
ram	equ	2000h	;Anfangsadresse RAM
dt	equ	2ff5h	;Anfangsadresse fuer die Datenbereiche
 
; Stackbereich
stk_br	equ	3080h

; Startadressentabelle fuer ISR (CTC-Kanaele 0, 1, 2, 3)
isr_mk	equ	30a0h
isr_dk	equ	30a2h
isr_hs	equ	30a4h
isr_ix	equ	30a6h
 
; Kommandoeingabebereich
cmd_br	equ	30bfh

; Fehlerausgabe
err_br	equ	30c7h

;--------  Parametertabelle (vom Laufwerk)  ------------------------------
p_str	equ	30d0h	;8 Byte, Firmware-Version (aus string)
p_blk	equ	30d8h	;13 Byte, Beschreibung Laufwerkstyp
			;1 Byte frei
			;1 Byte frei
cyls	equ	30e7h	;2 Byte, Zylinderanzahl
hds	equ	30e9h	;1 Byte, Kopfanzahl
secs	equ	30eah	;1 Byte, Sektoranzahl
pre_cy	equ	30ebh	;2 Byte, Zylindernummer der Praekompensation
rp_mod	equ	30edh	;1 Byte, Ramp-Mode-Einschaltung
			;6 Byte frei
ztk_40	equ	30f4h	;1 Byte, Zeitkonstante Formatieren 40 MHz
ztk_41	equ	30f5h	;1 Byte, Zeitkonstante formatieren 41,4 Mhz
zmn_40	equ	30f6h	;1 Byte, u. Grenze fuer Zeitm. IX-Impulse 40 MHz
zmx_40	equ	30f7h	;1 Byte, o. Grenze fuer Zeitm. IX-Impulse 40 MHz
zmn_41	equ	30f8h	;1 Byte, u. Grenze fuer Zeitm. IX-Impulse 41,4 MHz
zmx_41	equ	30f9h	;1 Byte, o. Grenze fuer Zeitm. IX-Impulse 41,4 MHz
;--------  Ende der Parametertabelle  ------------------------------------

bl_dr0	equ	3115h	;2 Byte, Anzahl der Bloecke auf einem LW (Low)
bl_dr1	equ	3117h	;2 Byte, Anzahl der Bloecke auf einem LW (High)
bl_zyl	equ	3119h	;2 Byte, Anzahl der Bloecke auf einem Zyl (Low)
bl_zyh	equ	311bh	;2 Byte, Anzahl der Bloecke auf einem Zyl (High)
			;1 Byte frei
			;1 Byte frei
sav_fb	equ	311fh	;1 Byte, Rettungsbyte fuer FB
l_inc	equ	3120h	;1 Byte, Spurinkrementierungen bei Mehrblockzugr.
c_s_ix	equ	3121h	;1 Byte, Zaehler fuer erlaubte falsche Sektor-
			;nummern nach Index
hst_bl	equ	3122h	;1 Byte, Blockzaehler fuer HS-Treiber
hst_by	equ	3123h	;1 Byte, Besetztanzeiger fuer HS-Treiber 
ident	equ	3124h	;7 Byte, Kennfeld FE CY CY HD SC CRC CRC
cnt_st	equ	312bh	;1 Byte, Ausgabebyte fuer CNTST
dsk_c1	equ	312ch	;1 Byte, Ausgabebyte fuer DSKC1
dsk_c2	equ	312dh	;1 Byte, Ausgabebyte fuer DSKC2
ch_cyl	equ	312eh	;1 Byte, Zylinderwechsel waehrend eines Zugriffs
err_in	equ	312fh	;1 Byte, Fehler waehrend der Initialisierung
dt_len	equ	3130h	;2 Byte, Datenlaenge (Zwischenspeicher)
rd_mod	equ	3132h	;1 Byte, Modus beim Lesen
k_flg	equ	3133h	;1 Byte, Steuerung der Sektorsuche
sc_akt	equ	3134h	;1 Byte, angeforderte Sektornummer (beim Schreiben)
			;2 Byte frei
s_rdy	equ	3137h	;1 Byte, 'Soft'-Ready-Byte (PAR- & BTT-Zustand)
d_rdy	equ	3138h	;1 Byte, Ready-Byte fuer Laufwerke
ctc_st	equ	3139h	;1 Byte, CTC-Programmierwort (Kanal2)
sc_cnt	equ	313ah	;1 Byte, Sektorzaehler

; Steuerbytes fuer die Disk-Schnittstelle
cl_crc	equ	317ah	;1 Byte
st_ina	equ	317bh	;1 Byte
st_akt	equ	317ch	;1 Byte
st_min	equ	317dh	;1 Byte
st_vb	equ	317eh	;1 Byte
st_dak	equ	317fh	;1 Byte
st_wa	equ	3180h	;1 Byte
st_wm	equ	3181h	;1 Byte
st_wc	equ	3182h	;1 Byte
d_akt	equ	3183h	;1 Byte
d_inak	equ	3184h	;1 Byte
men_ia	equ	3185h	;1 Byte

; Disk-Adresszaehler
dsk_ad	equ	3186h	;2 Byte
dsk_az	equ	3188h	;2 Byte

; Durchlaufzaehler fuer Formatieren
dl_f	equ	3192h	;1 Byte
dl_r	equ	3193h	;1 Byte
 
; Tabelle der Zylinderpositionen der Laufwerke
cy_tab	equ	3194h	;6 Byte

; Fehlercode und Fehlerzaehlerinitialisierung
err_cd	equ	31a0h	;1 Byte, Fehlercode
c_zt	equ	31a1h	;1 Byte
c_cd	equ	31a2h	;1 Byte
c_sc	equ	31a3h	;1 Byte
c_ci	equ	31a4h	;1 Byte
c_cj	equ	31a5h	;1 Byte
c_fb	equ	31a6h	;1 Byte
c_ki	equ	31a7h	;1 Byte

; Fehlerzaehler
; Keine Marken erkannt (Zeitueberwachung)
sum_zt	equ	31aah
mx_zt	equ	31ach
max_zt	equ	31adh
ct_zt	equ	31aeh
; CRC-Fehler im Identifier(1)
sum_ck	equ	31afh
mx_ck	equ	31b1h
max_ck	equ	31b2h
ct_ck	equ	31b3h
; CRC-Fehler im Identifier(2)
sum_cj	equ	31b4h
mx_cj	equ	31b6h
max_cj	equ	31b7h
ct_cj	equ	31b8h
; Falsche Sektornummern (nach Distanzinterrupt)
sum_sc	equ	31b9h
mx_sc	equ	31bbh
max_sc	equ	31bch
ct_sc	equ	31bdh
; Kein richtiges Kennfeld
sum_ki	equ	31beh
mx_ki	equ	31c0h
max_ki	equ	31c1h
ct_ki	equ	31c2h
; Falsches Datenfeldkennzeichen nach richtigem Kennfeld
sum_fb	equ	31c3h
mx_fb	equ	31c5h
max_fb	equ	31c6h
ct_fb	equ	31c7h
; CRC-Fehler im Datenfeld
sum_cd	equ	31c8h
mx_cd	equ	31cah
max_cd	equ	31cbh
ct_cd	equ	31cch
; Schreibwiederholungen bei 'Read after Write'
sum_ra	equ	31cdh
mx_ra	equ	31cfh
max_ra	equ	31d0h
ct_ra	equ	31d1h
; Falsche Sektornummer beim Schreiben
sum_wi	equ	31d2h
mx_wi	equ	31d4h
max_wi	equ	31d5h
ct_wi	equ	31d6h
; 'Fault Write'-Fehler
sum_fw	equ	31d7h
mx_fw	equ	31d9h
max_fw	equ	31dah
ct_fw	equ	31dbh

; Zugriffszaehler
z_cnt	equ	31dch	;10 Byte fuer Zugriffsanzahl,beginnend mit 'lowest'

; Fehlerzaehler fuer das Testlesen
t_cnt	equ	31f0h
t_kf	equ	31f1h
f_0f	equ	31f2h
f_ebit	equ	31f3h
f_ausr	equ	31f4h
f_unkl	equ	31f5h
f_ande	equ	31f6h
f_eb	equ	31f7h
 
; Blockverwaltung
p_kf	equ	31f8h	;2 Byte, Zeiger im 'kf-Feld'
p_dt	equ	31fah	;2 Byte, Zeiger im 'dt-Feld'
kf_br	equ	31fch	;2 Byte, aktuelle Anfangsadresse von kf
dt_br	equ	31feh	;2 Byte, aktuelle Anfangsadresse von dt

; Defektspurtabellen der Laufwerke
df_end	equ	3200h	;2 Byte, Adresse des aktuellen Werts von dfendx
df_tab	equ	3202h	;2 Byte, Adresse des aktuellen Werts von dftabx
dfend0	equ	3204h	;2 Byte, Ende-Adresse der BTT fuer LW0
dftab0	equ	3206h	;120(78h) Byte, BTT des LW0 (40*3 Byte Laenge)
	;	327eh	;3 Byte, Bereich fuer moegliches Endekennzeichen
dfend1	equ	3281h	;2 Byte, Ende-Adresse der BTT fuer LW0
dftab1	equ	3283h	;120(78h) Byte, BTT des LW1 (40*3 Byte Laenge)
	;	32fbh	;3 Byte, Bereich fuer moegliches Endekennzeichen
dfend2	equ	32feh	;2 Byte, Ende-Adresse der BTT fuer LW0
dftab2	equ	3300h	;120(78h) Byte, BTT des LW2 (40*3 Byte Laenge)
	;	3378h	;3 Byte, Bereich fuer moegliches Endekennzeichen

; Hilfsgroessen fuer die Zeitkonstantenbestimmung beim Formatieren
ztk	equ	3380h	;1 Byte, Zeitkonstante
ztk_f	equ	3381h	;8 Byte, Feld fuer Hilfsgroessen

; Rettungsbereich
s_prae	equ	3590h	;10h Byte fuer Praeambelbereich (Schreiben)
s_post	equ	35a0h	;10h Byte fuer Postambelbereich (Schreiben)
sav_20	equ	35b0h	;20h Byte 36b0...36cf
fs_tab	equ	35e0h	;20h Byte (32), Sektorfolge fuer Formatieren

; 3600h bis 378fh Bereich fuer Sektorkennfelder
kf_top	equ	3600h
kf	equ	3603h

; Bereiche fuer das Umladen von Zugriffszaehler und Fehlerzellen
s_zcnt	equ	37c0h	;10 Byte, Zugriffszaehler
s_errz	equ	37cah	;50 Byte, Fehlerzellen
 eject
;--------------------------------------------------------
; Systemanlauf und Initialisierungen
;--------------------------------------------------------
entry
	ld	sp,stk_br+1

; Test der CRC-Werte fuer beide EPROM's
	ld	hl,(crc_0)	;CRC-Wert fuer 1.EPROM
	ld	de,(crc_0n)	;negierter CRC-Wert
	add	hl,de
	inc	hl
	ld	a,h
	or	l
	jp	nz,err_pr
	ld	hl,(crc_1)	;CRC-Wert fuer 2.EPROM
	ld	de,(crc_1n)	;negierter CRC-Wert
	add	hl,de
	inc	hl
	ld	a,h
	or	l
	jp	nz,err_pr
	ld	hl,0		;Anfangsadresse 1.EPROM
	ld	bc,1000h	;4K Byte
	call	crc_br		;CRC-Berechnung
	ld	hl,(crc_0)	;abgelegter CRC-Wert fuer 1.EPROM
	ld	a,d
	ld	d,e
	ld	e,a
	xor	a
	sbc	hl,de
	jp	nz,err_pr
	ld	hl,1000h	;Anfangsadresse 2. EPROM
	ld	bc,0ff8h	;4K Byte - 8
	call	crc_br		;CRC-Berechnung
	ld	hl,(crc_1)	;abgelegter CRC-Wert fuer 2. EPROM
	ld	a,d
	ld	d,e
	ld	e,a
	xor	a
	sbc	hl,de
	jp	z,crc_ok
err_pr	
	ld	a,06h		;Status 6
	out	(CNTST),a	;Statusausgabe
	halt

; Umladen Zugriffszaehler
crc_ok	ld	de,s_zcnt
	ld	hl,z_cnt
	ld	bc,10
	ldir

; Umladen Fehlerzellen
	ld	hl,ct_fw
	ld	bc,sum_zt
	xor	a
	sbc	hl,bc
	inc	hl
	push	hl
	pop	bc
	ld	de,s_errz
	ld	hl,sum_zt
	ldir

; Loeschen Gesamt-RAM
	ld	hl,2000h
	ld	de,2001h
	ld	(hl),081h
	ld	bc,017bfh
	ldir

; Interrupt-Initialisierung
	ld	hl,isr_mk
	ld	a,h
	ld	i,a		;High-Teil Interruptvektor
	im	2
	ei
	ld	a,3		;Reset fuer die CTC-Kanaele
	out	(CTC),a
	out	(CTC+1),a
	out	(CTC+2),a
	out	(CTC+3),a
	ld	a,l		;Low-Teil Interruptvektor
	out	(CTC),a

; ISR-Tabelle initialisieren
	ld	hl,isr_m1
	ld	(isr_mk),hl
	ld	hl,isr_al
	ld	(isr_dk),hl
	ld	hl,isr_h0
	ld	(isr_hs),hl
	ld	hl,isr_al
	ld	(isr_ix),hl
 
; Loeschen Zugriffszaehler
	ld	b,10
	ld	hl,z_cnt
cl_zc	ld	(hl),0
	inc	hl
	djnz	cl_zc

; Loeschen Fehlercode und Fehlerzellen
	xor	a
	ld	(err_in),a
	ld	(err_cd),a
	ld	(s_rdy),a
	ld	hl,ct_fw	;Ende der Fehlerzellen
	ld	bc,sum_zt	;Beginn der Fehlerzellen
	sbc	hl,bc
	ld	b,l
	inc	b
	ld	hl,sum_zt
	xor	a
cl_sum	ld	(hl),a
	inc	hl
	djnz	cl_sum

; Loeschen Zeitkonstante und Hilfsfeld (Formatierungshilfsmittel)
	ld	b,9
	xor	a
	ld	hl,ztk
cl_ztk	ld	(hl),a
	inc	hl
	djnz	cl_ztk

; Loeschen Besetztzeiger und Blockzaehler des HS-Treibers
	ld	(hst_by),a
	ld	(hst_bl),a

; Initialisieren 'cnt_st' fuer CNTST (nach Reset)
	ld	a,0		;Status '0' nach Reset (8212 hat CLR-Eingang)
	ld	(cnt_st),a
 
; Initialisieren 'dsk_c2' fuer DSKC2
	ld	a,92h		;FR & Harddisk & Select0 & 'DEN=inaktiv'
				; & 'MEN=inaktiv' & 'CRCEN=inaktiv'
				; & 'WG=inaktiv'
	out	(DSKC2),a
	ld	a,12h		;FR ruecksetzen
	ld	(dsk_c2),a
	out	(DSKC2),a
 
; Initialisieren 'dsk_c1' fuer DSKC1
	ld	a,0		;HS0 & 'SD=inaktiv' (nach aussen)
				; & 'STEP=inaktiv' & Motor aus & 'HL=inaktiv'
	ld	(dsk_c1),a
	out	(DSKC1),a
 
; Loeschen der Positionstabelle
	ld	hl,-1
	ld	(cy_tab),hl
	ld	(cy_tab+2),hl
	ld	(cy_tab+4),hl

;--------------------------------------------------------
; Bestimmung der DSKC2-Steuerbytes (DEN, MEN, CRCEN, WG)
;--------------------------------------------------------
	ld	a,(dsk_c2)
	and	0f0h
	ld	b,a		;alle Steuerbits = 0
; Steuerbytes fuer das Lesen ('WG=inaktiv')
	or	7		;'DEN=aktiv' & 'MEN=inaktiv' & 'CRCEN=aktiv'
	ld	(cl_crc),a	;Steuerbits zum CRC-Loeschen auf 'ff'
	ld	a,b
	or	2		;alles inaktiv
	ld	(st_ina),a	;Steuerbits zum Abschalten der Schnittstelle
	ld	a,b
	or	5		;alles aktiv
	ld	(st_akt),a	;Steuerbits zur Aktivierung der Markenerkennung
	ld	a,b
	or	7		;'MEN=inaktiv' & 'DEN=aktiv' & 'CRCEN=aktiv'
	ld	(st_min),a	;Steuerbits fuer den CRC-Test
	ld	a,b
	or	3		;'MEN=inaktiv' & 'DEN=aktiv' & 'CRCEN=inaktiv'
	ld	(st_dak),a	;Steuerbits waehrend langer Transfers
	ld	a,b
	or	4		;'DEN=inaktiv' & 'CRCEN=aktiv' & 'MEN=aktiv'
	ld	(st_vb),a	;Steuerbits fuer die Vorbereitung der
				;inaktiven Disk-Ansteuerung
; Steuerbytes fuer das Schreiben ('WG=aktiv')
	set	3,b		;'WG=aktiv'
	ld	a,b
	or	0dh
	ld	(st_wa),a	;alles aktiv
	ld	a,b
	or	0bh		;'DEN=aktiv' & 'MEN=inaktiv' & 'CRCEN=inak'
	ld	(st_wm),a
	ld	a,b
	or	0fh		;auch 'CRCEN'=aktiv
	ld	(st_wc),a

; Initialisierung der Parametertabelle mit vorlaeufigen Werten
	ld	hl,100
	ld	(cyls),hl
	ld	(pre_cy),hl
	ld	a,2
	ld	(hds),a
	ld	a,18
	ld	(secs),a
	ld	a,1
	ld	(rp_mod),a

; Initialisierung von 'kf_br' und 'dt_br'
	ld	hl,kf
	ld	(kf_br),hl
	ld	hl,dt
	ld	(dt_br),hl

; Berechnung des Disk-Adresszaehlers fuer das Kennfeld
	ld	hl,(kf_br)
	call	b_daz
	ld	(dsk_ad),hl

;-------------------------------------------------
; Ready-Test der angeschlossenen Laufwerke
;-------------------------------------------------
; Loeschen Ready-Byte
	ld	a,87h		;INIT-Bit gesetzt
	ld	(d_rdy),a
; Ready-Test Laufwerk 0 (mit Abwarten der Startzeit)
	ld	b,30		;max. Startzeit (in sec.)
	inc	b
	inc	b
	ld	a,12h
	out	(DSKC2),a	;Select LW0
rdy0	in	a,(ST_PRT)
	bit	2,a
	jr	z,rdy1		;Sprung, wenn "ready"
	ld	de,100		;100 * 10ms = 1s
	call	time2		;ca 10 ms
	djnz	rdy0
	ld	hl,d_rdy	;Ready-Byte
	res	0,(hl)		;LW nicht "ready"
; Ready-Test Laufwerk 1 (mit Abwarten der Restzeit)
rdy1	ld	b,2
	ld	a,22h
	out	(DSKC2),a	;Select LW1
rdy11	in	a,(ST_PRT)
	bit	2,a
	jr	z,rdy2		;Sprung, wenn "ready"
	ld	de,100		;100 * 10ms = 1s
	call	time2		;ca 10 ms
	djnz	rdy11
	ld	hl,d_rdy	;Ready-Byte
	res	1,(hl)		;LW nicht "ready"
; Ready-Test Laufwerk 2 (mit Abwarten der Restzeit)
rdy2	ld	b,2
	ld	a,32h
	out	(DSKC2),a	;Select LW2
rdy21	in	a,(ST_PRT)
	bit	2,a
	jr	z,rdy_ok	;Sprung, wenn "ready"
	ld	de,100		;100 * 10ms = 1s
	call	time2		;ca 10 ms
	djnz	rdy21
	ld	hl,d_rdy	;Ready-Byte
	res	2,(hl)		;LW nicht "ready"
rdy_ok	ld	hl,d_rdy
	ld	a,(hl)
	cp	80h		;kein LW "ready" ?
	jr	nz,posit0
	set	6,(hl)		;ERR_IN-Bit setzen
	jp	begin

;-------------------------------------------------------
; Positionieren der Koepfe der Ready-Laufwerke auf Zyl 0
;-------------------------------------------------------
; Richtung der Kopfbewegung
posit0	ld	a,(dsk_c1)
	res	3,a		;SD nach aussen
	ld	(dsk_c1),a
	out	(DSKC1),a
; Laufwerk 0
	ld	hl,d_rdy
	bit	0,(hl)
	jp	z,p_lw1
	ld	a,12h
	out	(DSKC2),a
	call	pos_t0
	jr	z,p_lw00
	ld	hl,d_rdy
	set	3,(hl)		;Zyl 0 bei LW 0 nicht erreicht
	res	0,(hl)		;READY-Bit fuer LW 0 ruecksetzen
	set	6,(hl)		;Fehler waehrend der Initialisierung
	jr	p_lw1
p_lw00	ld	hl,0		;Kopfposition eintragen
	ld	(cy_tab),hl
; Laufwerk 1
p_lw1	ld	hl,d_rdy
	bit	1,(hl)
	jp	z,p_lw2
	ld	a,22h
	out	(DSKC2),a
	call	pos_t0
	jr	z,p_lw10
	ld	hl,d_rdy
	set	4,(hl)		;Zyl 0 bei LW 1 nicht erreicht
	res	1,(hl)		;READY-Bit fuer LW 1 ruecksetzen
	set	6,(hl)		;Fehler waehrend der Initialisierung
	jr	p_lw2
p_lw10	ld	hl,0		;Kopfposition eintragen
	ld	(cy_tab+2),hl
; Laufwerk 2
p_lw2	ld	hl,d_rdy
	bit	2,(hl)
	jp	z,t0_ok
	ld	a,32h
	out	(DSKC2),a
	call	pos_t0
	jr	z,p_lw20
	ld	hl,d_rdy
	set	5,(hl)		;Zyl 0 bei LW 2 nicht erreicht
	res	2,(hl)		;READY-Bit fuer LW 2 ruecksetzen
	set	6,(hl)		;Fehler waehrend der Initialisierung
	jr	t0_ok
p_lw20	ld	hl,0		;Kopfposition eintragen
	ld	(cy_tab+4),hl
t0_ok

;----------------------------------------------------------------
; Lesen der Sektoren 0/0/1 der Laufwerke
; Ueberpruefung der Parameter- und der Defektspurtabellen
;----------------------------------------------------------------
; Laufwerk 0
	ld	hl,d_rdy	;Ready-Byte
	bit	0,(hl)
	jp	z,df_1
	ld	hl,cl_crc	;Tabelle der Steuerbytes der DSS
	ld	b,9
	ld	c,10h		;Select LW0
sdf0	ld	a,(hl)
	and	0fh		;Loeschen der Select-Bits
	or	c
	ld	(hl),a
	inc	hl
	djnz	sdf0
	ld	a,0
	ld	(cmd_br-7),a	;LW im Kommandobereich
	call	def_ad
	call	rd_dfk		;Lesen des Sektors 0/0/1
	jr	nz,df_00	;Sprung, wenn Lesefehler
	call	t_par		;Ueberpruefung der Parametertabelle
	jr	nz,df_01
	call	v_par		;Umladen der Parameter
	ld	hl,s_rdy
	set	0,(hl)
	jr	df_03
df_01	ld	hl,err_in
	set	3,(hl)
	ld	hl,d_rdy
	set	6,(hl)		;ERR_IN-Bit setzen
	jr	df_1
df_03	call	t_btt		;Ueberpruefung der BTT
	jr	nz,df_02
	call	v_btt		;Umladen der BTT
	ld	hl,s_rdy
	set	3,(hl)
	jr	df_1
df_02	ld	hl,err_in
	set	3,(hl)
	ld	hl,d_rdy
	set	6,(hl)
	jr	df_1
df_00	ld	hl,err_in
	set	0,(hl)
	ld	hl,d_rdy
	set	6,(hl)
; Laufwerk 1
df_1	ld	hl,d_rdy	;Ready-Byte
	bit	1,(hl)
	jp	z,df_2
	ld	hl,cl_crc	;Tabelle der Steuerbytes der DSS
	ld	b,9
	ld	c,20h		;Select LW1
sdf1	ld	a,(hl)
	and	0fh		;Loeschen der Select-Bits
	or	c
	ld	(hl),a
	inc	hl
	djnz	sdf1
	ld	a,1
	ld	(cmd_br-7),a	;LW im Kommandobereich
	call	def_ad
	call	rd_dfk		;Lesen des Sektors 0/0/1
	jr	nz,df_10
	call	t_par		;Ueberpruefung der Parameter
	jr	nz,df_11	;Sprung, wenn Fehler
	ld	hl,s_rdy
	bit	0,(hl)
	jr	z,df_12
	call	parram		;Uebereinstimmung PAR mit RAM-Eintrag ?
	jr	z,df_13		;Sprung, wenn PAR ok
df_11	ld	hl,err_in
	set	4,(hl)
	ld	hl,d_rdy
	set	6,(hl)
	ld	hl,s_rdy
	bit	0,(hl)
	jr	nz,df_14
	jr	df_2
df_12	call	v_par		;Umladen der Parameter
df_13	ld	hl,s_rdy
	set	1,(hl)
df_14	call	t_btt		;Ueberpruefung der BTT
	jr	nz,df_15
	call	v_btt
	ld	hl,s_rdy
	set	4,(hl)
	jr	df_2
df_15	ld	hl,err_in
	set	4,(hl)
	ld	hl,d_rdy
	set	6,(hl)
	jr	df_2
df_10	ld	hl,err_in
	set	1,(hl)
	ld	hl,d_rdy
	set	6,(hl)
; Laufwerk 2
df_2	ld	hl,d_rdy	;Ready-Byte
	bit	2,(hl)
	jp	z,df_3
	ld	hl,cl_crc	;Tabelle der Steuerbytes der DSS
	ld	b,9
	ld	c,30h		;Select LW2
sdf2	ld	a,(hl)
	and	0fh		;Loeschen der Select-Bits
	or	c
	ld	(hl),a
	inc	hl
	djnz	sdf2
	ld	a,2
	ld	(cmd_br-7),a	;LW im Kommandobereich
	call	def_ad
	call	rd_dfk		;Lesen des Sektors 0/0/1
	jp	nz,df_20
	call	t_par		;Ueberprufung der Parameter
	jr	nz,df_22
	ld	hl,s_rdy
	bit	0,(hl)
	jr	nz,df_21
	bit	1,(hl)
	jr	nz,df_21
	call	v_par		;Eintragen der Parameter
df_23	ld	hl,s_rdy
	set	2,(hl)
	jr	df_24
df_21	call	parram		;Uebereinstimmung mit eingetr. Par. ?
	jr	z,df_23
df_22	ld	hl,err_in
	set	5,(hl)
	ld	hl,d_rdy
	set	6,(hl)
	ld	hl,s_rdy
	bit	0,(hl)		;PAR-Bit-LW0
	jr	nz,df_24	;Sprung, wenn PAR-Tab von LW0 ok
	bit	1,(hl)		;PAR-Bit-LW1
	jr	z,df_3		;Sprung, wenn PAR-Tab von LW1 nicht ok
df_24	call	t_btt		;Ueberpruefung der BTT
	jr	nz,df_25
	call	v_btt		;Umladen der BTT
	ld	hl,s_rdy
	set	5,(hl)
	jr	df_3
df_25	ld	hl,err_in
	set	5,(hl)
	ld	hl,d_rdy
	set	6,(hl)
	jr	df_3
df_20	ld	hl,err_in
	set	2,(hl)
	ld	hl,d_rdy
	set	6,(hl)
df_3

;---------------------------------------------------
; Bestimmung 'Anzahl Bloecke pro Zylinder'
; und 'Anzahl Bloecke pro Laufwerk'
;---------------------------------------------------
	ld	hl,0
	ld	a,(hds)		;Anzahl Koepfe
	ld	b,a
	ld	d,0
	ld	a,(secs)	;Blockanzahl pro Spur
	ld	e,a
	ld	a,0
blz0	add	hl,de
	jr	nc,blz1
	inc	a
blz1	djnz	blz0
	ld	(bl_zyl),hl
	ld	(bl_zyh),a
	ld	hl,0
	ld	(bl_dr0),hl
	ld	(bl_dr1),hl
	ld	d,0
	ld	a,(secs)	;Blockanzahl pro Spur
	ld	e,a
	ld	hl,(cyls)	;Zylinderanzahl pro Laufwerk
	dec	hl
blz4	push	hl
	ld	a,(hds)		;Spuranzahl pro Zylinder
	ld	b,a
blz2	ld	hl,(bl_dr0)
	add	hl,de
	ld	(bl_dr0),hl
	jr	nc,blz3
	xor	a
	ld	hl,(bl_dr1)
	inc	hl
	ld	(bl_dr1),hl
blz3	djnz	blz2
	pop	hl
	dec	hl
	ld	a,h
	or	l
	jp	nz,blz4
	ld	hl,bl_dr0	;Lowest
	ld	b,4
d_bl	dec	(hl)
	ld	a,(hl)
	cp	0ffh
	jr	nz,d_bn
	inc	hl
	djnz	d_bl
d_bn
	jp	begin

;---  pos_t0  ------------------------------------
; Schrittweises Zurueckfahren der Koepfe auf Zyl 0
; Rueckkehr: Z ok.
;            NZ Fehler
;-------------------------------------------------
pos_t0	ld	hl,3000		;mehr als Max von Zylindern
wt_see	ld	bc,400		;max. 400 ms warten auf SEEKC
wt_se1	in	a,(ST_PRT)	;Lesen des Statusports
	bit	3,a		;'SEEKC=aktiv' ?
	jr	z,rd_stp	;Sprung, wenn 'SEEKC=aktiv'
	ld	de,1
	call	time1		;ca. 1 ms
	dec	bc
	ld	a,b
	or	c
	jr	nz,wt_se1
	inc	a		;Rueckkehr mit Fehler (nz)
	ret			;Rueckkehr mit Fehler
rd_stp	in	a,(ST_PRT)	;Lesen des Statusports
	bit	6,a		;'T0=aktiv' ?
	ret	z		;Rueckkehr ok.
	ld	a,(dsk_c1)
	set	0,a		;'ST=aktiv'
	di
	out	(DSKC1),a
	res	0,a		;'ST=inaktiv'
	out	(DSKC1),a
	ei
	dec	hl
	ld	a,h
	or	l
	jr	nz,wt_see
	inc	a
	ret			;Rueckkehr mit Fehler (nz)
 
;---  cy_pos  ---------------------------------------------
; Bestimmung der Position in 'cy_tab'
; IN : A = Laufwerk
; OUT: HL= Adresse der Eintragung in der Zylindertabelle
;----------------------------------------------------------
cy_pos	add	a,a
	ld	c,a
	ld	b,0
	ld	hl,cy_tab	;Positionstabelle
	add	hl,bc
	ret

;---  rd_dfk  ----------------------------------------------
; Lesen des Sektors der Parameter- und der Defektspurtabelle
;-----------------------------------------------------------
rd_dfk	ld	de,ident
	ld	hl,id_df
	ld	bc,5
	ldir
	ld	de,c_zt
	ld	hl,fe_n
	ld	bc,7
	ldir			;Setzen der Fehlerzaehler
	ld	hl,200h
	ld	(cmd_br-2),hl	;Laenge festlegen
	ld	(dt_len),hl
	ld	a,0
	ld	(rd_mod),a	;Lesen von Kenn- und Datenfeld
	call	rd_id
	ret

;--- t_par -----------------------------------------------------
; Ueberpruefung der Parametertabelle
; Daten muessen im RAM-Bereich fuer den ersten Sektor stehen
;---------------------------------------------------------------
; Test auf 'PARMTR'
t_par	ld	hl,(dt_br)
	ld	bc,269
	add	hl,bc		;erstes Byte der Parametertabelle
	res	4,h
	ld	de,txt_pm	;Kenntext der Parametertabelle
	ld	b,6
sl_pm	ld	a,(de)
	cp	(hl)
	jp	nz,t_pare
	inc	hl
	inc	de
	djnz	sl_pm
; Test auf ASCII-Zeichen in der Laufwerksbezeichnung
	ld	b,12
sl_asc	ld	a,(hl)
	cp	20h		;unterste Grenze
	jp	c,t_pare	;Fehler
	cp	7bh		;oberste Grenze + 1
	jp	nc,t_pare	;Fehler
	inc	hl
	djnz	sl_asc
	inc	hl
; Test der Zylindernummer
	inc	hl
	inc	hl
	ld	a,(hl)		;Zylinder Low
	inc	hl
	ld	h,(hl)		;Zylinder High
	ld	l,a
	push	hl
	pop	de		;Zylinderanzahl
	push	hl
	ld	bc,100		;mind. 100 Zylinder
	xor	a
	sbc	hl,bc
	pop	hl		;Zylinder
	jp	c,t_pare
	ld	bc,3000		;max. 3000 Zylinder
	xor	a
	sbc	hl,bc
	jp	nc,t_pare
; Test der Kopfanzahl
	ld	hl,(dt_br)
	ld	bc,292
	add	hl,bc
	res	4,h		;HL zeigt Kopfanzahl
	ld	a,(hl)
	cp	2		;mind. 2 Koepfe
	jp	c,t_pare
	cp	17		;max. 16 Koepfe
	jp	nc,t_pare
; Test der Sektoranzahl
	inc	hl
	ld	a,(hl)		;Sektoranzahl
	cp	17
	jp	c,t_pare
	cp	19
	jp	nc,t_pare
; Test der Praekompensationseinschaltung
	inc	hl
	ld	a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a
	push	hl		;Zylindernummer der Praek.
	ld	bc,0
	xor	a
	sbc	hl,bc
	pop	hl
	jp	c,t_pare
	xor	a
	ex	de,hl
	sbc	hl,de		;HL enthaelt Zylinderanzahl
	jp	c,t_pare
; Test von 'rp_mod'	
	ld	hl,(dt_br)
	ld	bc,296
	add	hl,bc
	res	4,h
	ld	a,(hl)
	cp	1
	jp	c,t_pare
	cp	22
	jp	nc,t_pare
; Test der Zeitkonstanten
	ld	hl,(dt_br)
	ld	bc,303
	add	hl,bc
	res	4,h
	ld	a,(hl)		;Zeitkonstante 40 MHz
	cp	0b0h		;untere Grenze
	jp	c,t_pare
	inc	hl
	cp	(hl)		;Zeitkonstante 41,4 MHz
	jp	nc,t_pare
	cp	0f1h		;obere Grenze + 1
	jp	nc,t_pare
; Test der Grenzwerte fuer 40MHz/41,4MHz-Umschaltung
	inc	hl
	ld	b,(hl)		;'zmn_40'
	inc	hl
	ld	a,(hl)		;'zmx_40'
	cp	b
	jp	c,t_pare
	inc	hl
	ld	c,(hl)		;'zmn_41'
	inc	hl
	ld	a,(hl)		;'zmx_41'
	cp	c
	jp	c,t_pare
	ld	a,c		;'zmn_41'
	sub	b		;'zmn_40'
	jp	nc,t_pare
	xor	a
	ret			;Rueckkehr ok
t_pare	ld	a,1
	cp	0
	ret			;Fehlerrueckkehr

;--- parram ---------------------------------------------------
; Vergleich der im RAM (3000) eingetragenen Parameter mit den
; von 0/0/1 gelesenen Werten
;--------------------------------------------------------------
parram	ld	hl,(dt_br)
	ld	bc,275
	add	hl,bc
	res	4,h		;Beginn der Inf. im Sektorbereich
	ld	de,p_blk	;Beginn der Inf. im RAM-Bereich
	ld	b,12
prr1	ld	a,(de)
	cp	(hl)
	ret	nz
	inc	de
	inc	hl
	djnz	prr1
	inc	de
	inc	de
	inc	de
	inc	hl
	inc	hl
	inc	hl
	ld	b,19
prr2	ld	a,(de)
	cp	(hl)
	ret	nz
	inc	de
	inc	hl
	djnz	prr2
	xor	a
	ret

;--- v_par ----------------------------------------------------
; Umladen der Parametertabelle aus dem Sektorbereich in den
; PAR-RAM-Bereich
;--------------------------------------------------------------
v_par	ld	hl,(dt_br)
	ld	bc,275
	add	hl,bc
	res	4,h
	ld	bc,12
	ld	de,p_blk
	ldir
	inc	hl
	inc	hl
	inc	hl
	xor	a
	inc	de
	ld	(de),a
	inc	de
	ld	(de),a
	inc	de
	ld	(de),a
	ld	bc,19
	ldir
	ret

;--- par_v ---------------------------------------------------------------
; Umladen von PAR aus dem 3000-Bereich in den Datenbereich des 1. Sektors
;-------------------------------------------------------------------------
par_v	ld	hl,(dt_br)
	ld	bc,275
	add	hl,bc
	res	4,h
	ex	de,hl
	ld	bc,34
	ld	hl,p_blk
	ldir
	ret

;--- btt_cl ----------------------------------------------------------
; Loeschen der BTT im 3000-RAM
;---------------------------------------------------------------------
btt_cl	ld	hl,(df_end)
	ld	(hl),0
	inc	hl
	ld	(hl),0
	ld	hl,(df_tab)
	ld	b,40*3+3
	xor	a
btt_c1	ld	(hl),a
	inc	hl
	djnz	btt_c1
	ld	hl,(df_tab)
	ld	(hl),0ffh
	inc	hl
	ld	(hl),0ffh
	inc	hl
	ld	(hl),0ffh
	ret

;--- t_btt --------------------------------------------------------
; Ueberpruefung der Defektspurtabelle
; Daten muessen im RAM-Bereich fuer den ersten Sektor stehen
;------------------------------------------------------------------
; Test auf 'DEFEKT'
t_btt	ld	hl,(dt_br)
	ld	bc,13
	add	hl,bc		;erstes Datenbyte
	res	4,h
	ld	de,txt_df	;Text der BTT
	ld	b,6
sl_df	ld	a,(de)
	cp	(hl)
	jp	nz,t_btte
	inc	hl
	inc	de
	djnz	sl_df
; Test, ob Zaehler Vielfaches von 3
t_btt1	ld	e,(hl)
	inc	hl
	ld	d,(hl)		;DE: Anzahl der Eintragungen
df_dek	bit	7,d
	jp	nz,t_btte
	ld	a,d
	or	d
	jr	nz,df_dk1
	ld	a,e
	or	e
	jr	z,df_ok
df_dk1	dec	de
	dec	de
	dec	de
	jr	df_dek
; Test auf Maximalwert des Zaehlers
df_ok	inc	hl
	push	hl		;Beginn der BTT-Eintragungen
	dec	hl
	dec	hl		;Anzahl der Eintragungen
	ld	e,(hl)
	inc	hl
	ld	d,(hl)		;DE: Anzahl der belegten Plaetze in der BTT
	ld	hl,40*3		;Maximum von Eintragungen
	xor	a
	sbc	hl,de
	pop	hl		;Beginn der Eintragungen
	jp	c,t_btte	;Fehler, da Zaehler zu gross
; Test auf richtige Anordnung der Defektspuren
	push	hl
	add	hl,de		;HL: zeigt auf Endekennzeichen
	push	hl
	push	de
	exx
	pop	de
	pop	hl
	exx
	pop	hl		;Beginn der Eintragungen
	ld	a,d
	or	e
	jr	z,er_df5	;keine Eintragungen
er_df3	ld	d,(hl)		;Zyl high
	inc	hl
	ld	e,(hl)		;Zyl low
	push	de		;Zylinder
	inc	hl
	inc	hl
	ld	b,(hl)		;Zyl high
	inc	hl
	ld	c,(hl)		;Zyl low
	dec	hl
	dec	hl
	dec	hl		;HL zeigt auf Zyl low
	ex	de,hl
	pop	hl		;Zylinder
	xor	a
	sbc	hl,bc		;Zyl - Zyl_nex
	jr	c,er_df4	;Zyl_nex groesser
	jp	nz,t_btte	;Fehler: Reihenfolge falsch
	ex	de,hl
	inc	hl
	ld	a,(hl)		;Kopf
	inc	hl
	inc	hl
	inc	hl
	ld	b,(hl)		;Kopf_nex
	sub	b		;Kopf - Kopf_nex
	jp	nc,t_btte	;Fehler: Zyl gleich, Kopfreihenfolge falsch
	dec	hl
	dec	hl
	dec	hl
	dec	hl		;HL zeigt auf Zyl low
	ex	de,hl
; Test auf Maximalwert fuer Zylinder
er_df4	ex	de,hl		;HL zeigt auf Zyl low
	ld	e,(hl)
	dec	hl
	ld	d,(hl)
	ex	de,hl		;HL: Zyl, DE: zeigt auf Zyl high
	ld	bc,(cyls)
	xor	a
	sbc	hl,bc
	jp	nc,t_btte	;Fehler: Zylinder zu gross
; Test auf Maximalwert fuer Kopf
	ex	de,hl
	inc	hl
	inc	hl
	ld	a,(hds)
	ld	b,a
	ld	a,(hl)		;Kopf
	sub	b
	jp	nc,t_btte
	inc	hl		;HL zeigt auf Zyl high
	exx
	dec	de		;Endetest
	dec	de
	dec	de
	ld	a,d
	or	e
	exx
	jp	nz,er_df3
; Test auf Endekennzeichen
er_df5	exx
	push	hl
	exx
	pop	hl		;HL: zeigt auf Endekennzeichen
	ld	a,(hl)
	cp	0ffh
	jr	nz,t_btte	;Fehler, da kein exaktes Endekennzeichen
	inc	hl
	cp	(hl)
	jr	nz,t_btte	;Fehler, da kein exaktes Endekennzeichen
	inc	hl
	cp	(hl)
	jr	nz,t_btte	;Fehler, da kein exaktes Endekennzeichen
	xor	a
	ret			;Ruecksprung ok
t_btte	ld	a,1
	cp	0
	ret			;Fehler-Ruecksprung

;--- v_btt -----------------------------------------------------------
; Umladen der BTT aus dem Datenbereich des 1. Sektors in den 3000-RAM
;---------------------------------------------------------------------
v_btt	ld	hl,(dt_br)
	ld	bc,19
	add	hl,bc
	res	4,h
	ld	bc,40*3+2+3
	ld	de,(df_end)
	ldir
	ret

;--- btt_v -----------------------------------------------------------
; Umladen der BTT aus 3000-RAM in den Datenbereich des 1. Sektors
;---------------------------------------------------------------------
btt_v	ld	hl,(dt_br)
	ld	bc,19
	add	hl,bc
	res	4,h
	ex	de,hl
	ld	hl,(df_end)
	ld	bc,40*3+2+3
	ldir
	ret

;--- def_ad ---------------------------------------------
; Bestimmung der aktuellen Adressen der Defektspurtabelle
; cmd_br-7 : Laufwerk
;--------------------------------------------------------
def_ad	ld	a,(cmd_br-7)	;Laufwerk
	ld	hl,dfend0
	ld	(df_end),hl
	ld	hl,dfend1
	ld	de,dfend2
	cp	1
	jr	c,d_adr
	jr	z,d_ad1
	ld	(df_end),de
	jr	d_adr
d_ad1	ld	(df_end),hl
d_adr	ld	hl,(df_end)	;Adresse von 'dfendx'
	inc	hl
	inc	hl
	ld	(df_tab),hl	;Adresse des Beginns der aktuellen BTT
	ret

;--- par_s ---------------------------------------------------
; UP Setzen des PAR-Bits in 's_rdy' fuer das Laufwerk
;-------------------------------------------------------------
par_s	ld	a,(cmd_br-7)	;Laufwerk aus Kommando
	ld	b,a
	inc	b
	xor	a
	scf
par_s1	rla
	djnz	par_s1
	ld	hl,s_rdy
	or	(hl)
	ld	(s_rdy),a
	ret

;--- btt_s ---------------------------------------------------
; UP Setzen des BTT-Bits in 's_rdy' fuer das Laufwerk
;-------------------------------------------------------------
btt_s	ld	a,(cmd_br-7)	;Laufwerk aus Kommando
	ld	b,a
	inc	b
	xor	a
	scf
	rla
	rla
	rla
btt_s1	rla
	djnz	btt_s1
	ld	hl,s_rdy
	or	(hl)
	ld	(s_rdy),a
	ret

;--- par_r ---------------------------------------------------
; UP Ruecksetzen des PAR-Bits in 's_rdy' fuer das Laufwerk
;-------------------------------------------------------------
par_r	ld	a,(cmd_br-7)	;Laufwerk aus Kommando
	ld	b,a
	inc	b
	xor	a
	scf
par_r1	rla
	djnz	par_r1
	cpl
	ld	hl,s_rdy
	and	(hl)
	ld	(s_rdy),a
	ret

;--- btt_r ---------------------------------------------------
; UP Ruecksetzen des BTT-Bits in 's_rdy' fuer das Laufwerk
;-------------------------------------------------------------
btt_r	ld	a,(cmd_br-7)	;Laufwerk aus Kommando
	ld	b,a
	inc	b
	xor	a
	scf
	rla
	rla
	rla
btt_r1	rla
	djnz	btt_r1
	cpl
	ld	hl,s_rdy
	and	(hl)
	ld	(s_rdy),a
	ret
 eject
begin
;-----------------------------------------------------------------------
; Host-Schnittstelle auf die Eingabe eines Kommandos (9 Byte) einstellen
;-----------------------------------------------------------------------
	ld	bc,9		;Eingabelaenge
	ld	hl,cmd_br	;Ende des Kommandobereich
	ld	a,1		;Status '1'
	call	p_h_wc		;Programmierung der Host-Schnittstelle
 
;---------------------------------------
; Auswertung des eingelaufenen Kommandos
;---------------------------------------
; Inkrement Zugriffszaehler
	ld	b,10
	ld	hl,z_cnt
zc_inc	inc	(hl)
	jr	nz,zc_ok
	inc	hl
	djnz	zc_inc
; Fehler in der Initialisierung ?
zc_ok	ld	hl,d_rdy
	bit	7,(hl)		;INIT-Bit
	jr	z,not_in	;nicht erste Abfrage nach Initialisierung
	res	7,(hl)		;INIT-Bit ruecksetzen
	bit	6,(hl)		;ERR_IN-Bit
	jr	z,not_in	;kein Fehler in der Initialisierung
	ld	a,(hl)
	and	3fh		;ZY0- und RDY-Bits
	jr	nz,nt_27	;kein LW "ready"
	ld	a,27h		;kein LW "ready"
	ld	(err_cd),a
	jp	error
nt_27	ld	a,(hl)
	and	38h		;ZY0-Bits
	jr	z,inerr		;kein ZY0-Fehler
	srl	a
	srl	a
	srl	a		;Laufwerkscodierung
	or	28h		;CYL0-Fehler
	ld	(err_cd),a
	jp	error
inerr	ld	hl,err_in
	ld	a,(hl)
	and	7		;0/0/1 nicht lesbar
	jr	z,nt_30
	or	30h
	ld	(err_cd),a
	jp	error
nt_30	ld	a,(hl)
	and	38h		;PAR&BTT-Fehler
	srl	a
	srl	a
	srl	a
	or	38h
	ld	(err_cd),a
	jp	error

; Loeschen Inkrementierungszaehler
not_in	xor	a
	ld	(l_inc),a
; Warten, bis HS-Treiber frei
wt_hst	ld	hl,hst_by
	bit	7,(hl)
	jr	nz,wt_hst
; Auswertung des Kommando-Codes
	ld	a,(cmd_br-8)	;Kommandocode
	cp	21h		;Block lesen ?
	jr	z,t_com
	cp	22h		;Block schreiben ?
	jr	z,t_com1
	ld	hl,com_tb	;Tabelle aller Kommandos
com_ts	cp	(hl)
	jr	z,com_ok
	cp	0ffh		;Endekennzeichen
	inc	hl
	jr	nz,com_ts
	jp	err_1		;Fehler 1 : falscher Kommandocode
com_ok	bit	3,a		;WDC-RAM-Kommandos ?
	jp	nz,mv_ok
	bit	1,a		;HD-Schreiben ?
	jp	z,t_com
	cp	0c2h		;HD-Schreiben Defektspurtabelle ?
	jr	z,t_com
; Uebernahme der zu schreibenden Daten vom Host
t_com1	ld	hl,dt
	ld	bc,13
	add	hl,bc
	res	4,h
	ld	bc,(cmd_br-2)	;Laenge
	add	hl,bc
	dec	hl		;Endadresse Daten
	ld	a,2		;Status 2
	call	p_h_wc
; Datenlaenge umspeichern
t_com	ld	hl,(cmd_br-2)	;Datenlaenge aus Kommando
	ld	(dt_len),hl
; Bestimmung der aktuellen BTT
	call	def_ad
; Einfuegen der Select-Bits in die Steuerbytes der DSS
	ld	a,(cmd_br-7)	;angefordertes Laufwerk
	and	3
	inc	a
	rlca
	rlca
	rlca
	rlca
	ld	c,a
	ld	hl,cl_crc	;Beginn der Steuerbytes
	ld	b,9
einf_d	ld	a,(hl)
	and	0fh		;Loeschen der Select-Bits
	or	c
	ld	(hl),a
	inc	hl
	djnz	einf_d
; Laufwerkstest
	ld	a,(d_rdy)	;Ready-Byte
	ld	b,a
	ld	a,(cmd_br-7)	;Laufwerk
	cp	3
	jp	nc,err_3	;max. drei LW moeglich
	cp	0
	jp	nz,lt_1
	bit	0,b
	jp	z,err_3
	jr	lt_ok
lt_1	cp	1
	jr	nz,lt_2
	bit	1,b
	jp	z,err_3		;gefordertes LW nicht 'ready'
	jr	lt_ok
lt_2	bit	2,b
	jp	z,err_3		;gefordertes LW nicht 'ready'
lt_ok
; weitere Auswertung des Kommandocodes
	ld	a,(cmd_br-8)	;Kommandocode
	bit	2,a		;Formatier-Kommandos ?
	jp	nz,t_cd3
	bit	0,a		;Lese-Kommandos ?
	jp	nz,t_cd
	bit	1,a		;Schreib-Kommandos ?
	jp	z,mv_ok
t_cd	bit	5,a		;Blocknummern ?
	jr	z,t_cd1
; Test der Blocknummer
	ld	hl,cmd_br-3	;hoechstwertiges Byte im Kommando
	ld	de,bl_dr1+1	;hoechstwertiges Byte in Blockanzahl
	ld	b,4
t_bn	ld	a,(de)
	cp	(hl)
	jr	c,bn_err
	jr	nz,bn_ok
	dec	hl
	dec	de
	djnz	t_bn

;------------------------------------------------------------
; Bestimmung der physischen Sektoradresse aus der Blocknummer
; Blocknummer in cmd_br-6,...,cmd_br-3 (low..high)
; Sektoradresse in ident+1,...,ident+4
; Blocknummer 0: Zyl 1, Hd 0, Sc 1
;------------------------------------------------------------
bn_ok	ld	de,(bl_zyl)	;Anzahl der Bloecke pro Zylinder
; Bloecke auf Zylinder
	ld	bc,(cmd_br-6)
	ld	hl,(cmd_br-4)
	ld	a,17
	or	a
bl2	sbc	hl,de
	jr	nc,bl4
	add	hl,de
bl4	ccf
	rl	c
	rl	b
	adc	hl,hl
	dec	a
	jr	nz,bl2
	rr	h
	rr	l
	inc	bc
	ld	(ident+1),bc
; Bestimmung der Kopfnummer
	ex	de,hl
	ld	hl,ident+3	;zeigt auf Kopfnummer
	ld	a,(secs)
	ld	c,a
	ld	b,0		;Sektoranzahl pro Spur
	ld	(hl),-1		;Loeschen der Kopfnummer
bl5	inc	(hl)
	ex	de,hl
	xor	a
	sbc	hl,bc
	ex	de,hl
	jr	nc,bl5
	ex	de,hl
; Bloecke auf Sektoren
	add	hl,bc		;restliche Bloecke
	ld	a,l
	inc	a
	ld	(ident+4),a
	ld	a,0feh
	ld	(ident),a
; ENDE der Blocknummernumrechnung ---------------------------------
	call	sc_def
	jp	hd_mov
bn_err	ld	a,5		;Fehlercode 5: Blocknummer zu gross
	ld	(err_cd),a
	jp	error
t_cd1	bit	4,a		;Defektspurberuecksichtigung ?
	jr	z,t_cd2
	ld	hl,cmd_br-6
	call	id_ein
	call	sc_def
	jp	hd_mov
t_cd2	bit	6,a		;Defektspurtabelle ?
	jr	z,t_cd3
	ld	de,ident
	ld	hl,id_df
	ld	bc,5
	ldir
	jp	hd_mov
t_cd3	ld	hl,cmd_br-6
	call	id_ein
	jp	hd_mov
 eject
;--------------------------------------------------------------------
; Routine zur Bewegung der Schreib-Lese-Koepfe (Kamm)
;--------------------------------------------------------------------
hd_mov
; Test der Zylindernummer im Kommando
	ld	hl,(ident+1)	;Zylinder
	ld	bc,(cyls)	;Anzahl der Zylinder
	xor	a
	sbc	hl,bc
	jp	nc,err_4	;Zylindernummer im Kommando zu gross
; Laufwerk selektieren
	ld	a,(st_ina)
	out	(DSKC2),a
; Bestimmung der aktuellen Kopfposition
	ld	a,(cmd_br-7)	;Laufwerk
	add	a,a
	ld	c,a
	ld	b,0
	ld	hl,cy_tab	;Tabelle der Kopfpositionen
	add	hl,bc		;Zeiger auf Kopfposition
	ld	c,(hl)		;Low-Teil
	inc	hl
	ld	b,(hl)		;High-Teil
	inc	bc		;ffff-Test
	ld	a,b
	or	c
	jp	z,err_6
	dec	bc
	ld	hl,(ident+1)	;angeforderter Zylinder
	xor	a
	sbc	hl,bc		;Positionsdifferenz
	jp	z,mv3		;Position ok.
	jp	c,st_out	;Kopfbewegung nach aussen
; Kopfbewegung nach innen
	ld	a,(dsk_c1)
	set	3,a		;SD nach innen
	ld	(dsk_c1),a
	out	(DSKC1),a
	jr	mv0
; Kopfbewegung nach aussen
st_out
	ld	a,(dsk_c1)
	res	3,a		;SD nach aussen
	ld	(dsk_c1),a
	out	(DSKC1),a
	push	hl
	pop	bc
	ld	hl,0
	xor	a
	sbc	hl,bc
; Bestimmung der Art der Schrittbewegung
mv0	push	hl		;Step-Anzahl
	ld	a,(rp_mod)
	ld	c,a
	ld	b,0
	xor	a
	sbc	hl,bc
	pop	hl
	jp	z,si_step
	jp	nc,ramp

; Stepausfuehrung im Single- und Slow-Step-Mode, HL: Schrittanzahl
si_step	ld	a,(dsk_c1)	;Ausgabebyte fuer DSKC1
	xor	1		;Step-Impuls ein
	di
	out	(DSKC1),a
	xor	1		;Step-Impuls aus
	out	(DSKC1),a
	ei
	dec	hl
	ld	a,h
	or	l
	jr	z,mv1
	ld	de,320
	call	time
	jr	si_step
; Stepausfuehrung im Ramp-Mode, HL: Schrittanzahl
ramp	ld	a,(hst_by)	;Test auf Abschluss von Host-Transfers
	bit	7,a
	jr	nz,ramp
	ld	de,(dsk_c1)		;Ausgabebyte fuer DSKC1 nach e
	set	0,e		;Step-Impuls an
rmp	ld	a,e
	out	(DSKC1),a
	xor	1		;Step-Impuls aus
	out	(DSKC1),a
	dec	hl
	ld	a,h
	or	l
	jp	nz,rmp
mv1	in	a,(ST_PRT)	;Lesen des Status-Ports
	bit	3,a		;'SEEKC=aktiv' ?
	jr	nz,mv1
	ld	hl,(ident+1)	;angeforderter Zylinder
	ld	a,h
	or	l		;Track0 angefordert ?
	jr	nz,mv2
	in	a,(ST_PRT)
	bit	6,a		;'Track0=aktiv' ?
	jr	z,mv2
	ld	a,7		;Fehlercode fuer 'Track0 nicht erreicht'
	ld	(err_cd),a
	jp	error
; Eintragen der neuen Kopfposition
mv2	ld	a,(cmd_br-7)	;Laufwerk
	call	cy_pos
	ld	bc,(ident+1)	;geforderte Position
	ld	(hl),c
	inc	hl
	ld	(hl),b
;------------
; Kopfauswahl
;------------
mv3	ld	a,(hds)
	ld	b,a
	ld	a,(ident+3)	;Kopfnummer
	cp	b
	jp	nc,err_2	;Kopfnummer zu gross
	rrca
	rrca
	rrca
	rrca
	ld	b,a
	ld	a,(dsk_c1)
	and	0fh		;Loeschen der HS-Bits
	or	b		;Einfuegen der neuen HS-Bits
	ld	(dsk_c1),a
	out	(DSKC1),a
;-----------------------------
; Auswertung des Kommandocodes
;-----------------------------
mv_ok
	ld	a,(cmd_br-8)	;Kommandocode
	cp	21h		;Blocklesen ?
	jp	z,read1
	cp	22h		;Blockschreiben ?
	jp	z,write
	cp	0a1h		;Block-Kopieren ?
	jp	z,read1
	cp	81h		;Testlesen ?
	jp	z,read5
	cp	11h		;Lesen mit BTT-Test ?
	jp	z,read1
	cp	1		;Sektorlesen ohne BTT-Test ?
	jp	z,read1
	cp	0		;Ready-Test ?
	jp	z,rdy_ts
	bit	1,a		;Schreib-Kommandos ?
	jp	nz,write
	bit	2,a		;Formatier-Kommandos ?
	jp	nz,format
	bit	3,a		;WDC-RAM-Transfers ?
	jp	nz,ram_tr
	jp	err_1
;-------------------------
; Ready-Test des Laufwerks
;-------------------------
rdy_ts
	jp	begin
 eject
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; Lesen eines Sektors, der durch seine Sektoradresse angegeben ist
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
read1
; Setzen der Fehlerzaehler
	ld	de,c_zt
	ld	hl,fe_n
	ld	bc,7
	ldir
; Lesen von Hard-Disk
	ld	a,0
	ld	(rd_mod),a	;Lesen von Kenn- und Datenfeld
	call	rd_id
	jp	nz,error
; Test auf Block-Kopieren
	ld	a,(cmd_br-8)
	cp	0a1h
	jr	nz,read11
	ld	a,22h
	ld	(cmd_br-8),a	;Block-Schreiben
	ld	a,0
	ld	(cmd_br-7),a	;Schreiben auf Laufwerk 0
	jp	t_com
; Uebergabe der gelesenen Daten an den Host
read11	ld	hl,dt
	ld	bc,13
	add	hl,bc		;Anfangsadresse der gelesenen Daten
	res	4,h		;eventuelles Bit 12 ruecksetzen
	ld	bc,(cmd_br-2)	;Datenlaenge
	add	hl,bc
	dec	hl		;Endadresse der gelesenen Daten
	ld	a,3		;Status '3'
	call	p_wc_h		;Programmierung der Host-Schnittstelle
	jp	begin
 eject
read5
;+++++++++++++++++++++++++++++++++++++++++++
; Testlesen eines Sektors
;+++++++++++++++++++++++++++++++++++++++++++
; Datenlaenge fuer einen Sektor einstellen
	ld	hl,200h
	ld	(dt_len),hl
; Loeschen der RAM-Zellen fuer Auswertung beim Testlesen
	ld	hl,t_cnt
	ld	b,8
clr_fm
	ld	(hl),0
	inc	hl
	djnz	clr_fm
; Setzen der Fehlerzaehler
rd5_sl
	ld	de,c_zt
	ld	hl,fe_t
	ld	bc,7
	ldir
; Lesen von Hard-Disk
	ld	a,0
	ld	(rd_mod),a	;Lesen von Kenn- und Datenfeld
	call	rd_id
; Auswertung der Leseversuche im Testmodus
	jr	nz,t_err
	ld	hl,t_kf		;Anzahl der richtigen Leseversuche (Test)
	inc	(hl)
	jp	t_end
; Fehlerauswertung im Test-Modus
t_err
	ld	a,(err_cd)	;Fehlercode
	cp	0fh		;CRC-Fehler im Datenfeld ?
	jp	z,asw_f
	ld	hl,f_ande
	inc	(hl)
	jp	t_end
; Auswertung der CRC-Fehler im Datenfeld
asw_f
	ld	hl,f_0f
	inc	(hl)
	ld	hl,(dt_br)	;Beginn des Datenfeldes
	ld	bc,13
	add	hl,bc
	res	4,h
	ld	bc,200h		;Laenge des Datenfeldes
sl_t1	ld	a,(hl)
	cp	0e5h		;Wert ok. ?
	jr	nz,diff1
	inc	hl
	dec	bc
	ld	a,c
	or	b
	jr	nz,sl_t1
	ld	a,(f_eb)
	cp	1
	jp	z,t_end
	ld	hl,f_unkl
	inc	(hl)
	jp	t_end
diff1
	ld	d,a		;Wert retten
	inc	hl
	dec	bc
	ld	a,b
	or	c
	jr	nz,sl_t2
	ld	hl,f_eb
	ld	(hl),1
	jp	t_end
sl_t2
	ld	a,(hl)		;naechster Wert
	cp	0e5h		;Wert ok. ?
	jr	nz,ausras
	push	hl
	ld	hl,f_eb
	ld	(hl),1
	pop	hl
	jp	sl_t1
ausras
	ld	hl,f_ausr
	inc	(hl)		;Fehler: ausgerastet
	jp	t_end
; Endebehandlung des Testmodus
t_end
	ld	a,(f_eb)
	cp	1		;Einzelbit-Fehler ?
	jr	nz,k_eb
	ld	hl,f_ebit
	inc	(hl)
k_eb	ld	a,(t_cnt)
	inc	a
	ld	(t_cnt),a
	cp	0ffh
	jp	z,t_ueb
	xor	a
	ld	(f_eb),a
	jp	rd5_sl
; Uebergabe der Testwerte an den Host
t_ueb
	ld	bc,(cmd_br-2)
	ld	hl,t_cnt
	add	hl,bc
	dec	hl
	ld	a,3
	call	p_wc_h
	xor	a
	ld	(err_cd),a
	jp	begin
 eject
;**************************************************************************
; UP rd_id
; Lesen eines Kennfeldes (und des Datenfeldes) von Hard-Disk
; 'ident' : Kennfeld, 'rd_mod' : 0=Lesen des Kennfeldes und des Datenfeldes
;                                1=Lesen des Kennfeldes
;**************************************************************************
rd_id
; Initialisierung von 'dt_br'
	ld	hl,dt
	ld	(dt_br),hl
; Test auf Abschluss eventueller Host-Datentransfers
rd_id1	ld	a,(hst_by)
	bit	7,a
	jr	nz,rd_id1
; Loeschen bzw. Umladen der Zaehler
	xor	a
	ld	(err_cd),a
	ld	(k_flg),a
	ld	a,(c_zt)
	ld	(ct_zt),a	;Zaehler fuer Zeitueberwachung
	ld	a,(c_cd)
	ld	(ct_cd),a	;Zaehler fuer CRC-Fehler im Datenfeld
	ld	a,(c_sc)
	ld	(ct_sc),a	;Zaehler fuer 'kein richtiges Kennfeld'
nx_sc
	ld	a,40
	ld	(sc_cnt),a	;Sektorzaehler
nx_sc1
; Ausschaltimpuls fuer DSS
	ld	a,(st_ina)
	out	(DSKC2),a
	out	(IMPAUS),a
; Ausgabe der Bitmuster der Marken fuer Takt und Daten
	ld	a,0a1h		;Datenbits
	out	(BM_D),a
	ld	a,0ah		;Taktbits
	out	(BM_T),a
; Berechnung des Disk-Adresszaehlers fuer das Datenfeld
	ld	hl,(dt_br)
	ld	bc,11
	add	hl,bc
	res	4,h
	call	b_daz
	ld	(dsk_az),hl
; Vorbereiten der '-Register
	exx
	ld	a,(st_ina)
	ld	b,a
	ld	a,(st_akt)
	ld	c,a
	ld	a,0f0h		;DEND zur Steuerungsumschaltung
	ld	e,a
	exx

;--------------------------------
; Schleife zum Suchen des Sektors
;--------------------------------
nx_sec
; Loeschen eventuellen CRC-Fehler
	in	a,(ST_PRT)
; Richtung des Transfers ausgeben
	ld	a,(cnt_st)	;Ausgabebyte
	res	7,a		;DR/W: Disk-->RAM
	set	6,a		;DA12=1
	ld	(cnt_st),a
	out	(CNTST),a
; Vorbereiten der Ausgabewerte fuer die ISR
	exx
	ld	hl,(dsk_az)	;HL: Adresse des Datenfeld
	exx
; ISR-Adresse fuer Markenerkennung beim Kennfeld
	ld	hl,isr_m9
	ld	a,(rd_mod)
	cp	1
	jr	z,is_ok
	ld	hl,isr_m1
is_ok	ld	(isr_mk),hl
; Laden des Disk-Adresszaehlers
	ld	hl,(dsk_ad)
	ld	(hl),a
; Laden der Disk-Endadresse fuer den CRC-Test des Kennfeldes
; Angabe der Anzahl der zu erkennenden Marken in den unteren drei Bits
	ld	hl,(kf_br)
	ld	bc,7
	add	hl,bc		;HL: Kennfeldadresse fuer CRC-Test & CM
	ld	a,l
	out	(DSKEA),a
; Vorbereiten der Register
	ld	a,(st_min)	;'MEN=inaktiv'
	ld	b,a
; CTC/0 fuer Markenerkennung programmieren
	ld	a,0c7h		;EI & Counter & High-Low-Flanke
	out	(CTC),a
	ld	a,1
	out	(CTC),a
; Beginn des CRC-Loeschens
	ld	a,(cl_crc)
	out	(DSKC2),a	;Loeschen des CRC-Gen./Checkers auf 'ff'
; Einschalten der Zeitueberwachung
	ld	a,0a7h		;Timer & VT256
	out	(CTC+3),a
	ld	a,30h		;(20h) ca. 2,0 ms
	out	(CTC+3),a
; Eintragen der ISR-Adresse fuer die Zeitueberwachung
	ld	hl,isr_zt
	ld	(isr_ix),hl
; Aktivieren der Schnittstelle zur Markenerkennung
	ld	a,(st_akt)
	out	(DSKC2),a	;alles aktiv zur Markenerkennung
	ld	a,b		;Vorbereitung  fuer ISR
	halt			;Warten auf das Erkennen der Marke A1
; ------- Warten auf drei Marken A1 vor dem Kennfeld ----------
 
	halt			;Warten auf Marke
; ------- Warten auf Marke A1 vor dem Datenfeld ---------
 
; Loeschen der Zeitueberwachung und der Markenerkennung
hlt_2	di
	ld	a,3
	out	(CTC),a
	out	(CTC+3),a
	ei
;-----------------------------------
; Auswertung der eingelaufenen Daten
;-----------------------------------
; Test des Identifiers
	ld	b,5
	ld	hl,(kf_br)
	inc	hl		;Beginn des eingelaufenen Idents
	ld	de,ident	;Beginn des geforderten Idents
tst_id	ld	a,(de)
	cp	(hl)
	jp	nz,id_e
	inc	hl
	inc	de
	djnz	tst_id
; CRC-Test des Identifiers
	in	a,(ST_PRT)
	bit	7,a
	jp	z,id_e
; geforderter Sektor ist gefunden
	ld	a,(rd_mod)	;Nur Kennfeld ?
	cp	1
	ret	z		;Rueckkehr ok.
 
;-----------------------------------------
; Kennfeld gefunden, Lesen des Datenfeldes
;-----------------------------------------
; Test auf Datenfeldkennzeichen
	ld	hl,(dt_br)
	ld	bc,12
	add	hl,bc
	res	4,h
	ld	a,0fbh		;HL auf Datenfeldkennzeichen
	cp	(hl)
	jp	nz,tdc_er
; Zurueckschreiben des durch FB ueberschriebenen Wertes
	ld	a,(sav_fb)	;gerettetes Byte
	ld	(hl),a
; Programmierung der Disk-Schnittstelle fuer exakte Datenfelduebertragung
	ld	a,(st_dak)
	out	(DSKC2),a	;'DEN=aktiv' & 'MEN=inaktiv' & 'CRCEN=inaktiv'
	ld	hl,(dt_br)
	ld	bc,90h		;neue DEND-Adresse zur Steuerungsumschaltung
	add	hl,bc
	ld	a,l
	out	(DSKEA),a
; CTC/1 fuer DEND-Erkennung programmieren
	ld	a,0d7h		;EI & Counter & Low-High-Flanke
	out	(CTC+1),a
	ld	a,2		;Zaehler
	out	(CTC+1),a
	halt			;Warten auf das 2. DEND
; ------- Warten auf DEND vor dem CRC-Test -------

; Berechnung der Adresse fuer den CRC-Test
	ld	hl,(dt_br)
	ld	bc,526
	add	hl,bc
	ld	a,l
	out	(DSKEA),a
; Umprogrammierung der Schnittstelle auf CRC-Test
	ld	a,(st_min)
	out	(DSKC2),a	;'DEN=aktiv' & 'MEN=inaktiv' & 'CRCEN=aktiv'
; Vorbereitung fuer das Abschalten der Disk-Schnittstelle
	ld	a,(st_ina)	;alles inaktiv
	ld	c,a
	ld	a,0d7h
	out	(CTC+1),a
	ld	a,1
	out	(CTC+1),a
	halt
; ------- Warten auf das DEND zum CRC-Test -------
 
; Programmierung fuer das Abschalten der Schnittstelle
	ld	a,c
	out	(DSKC2),a	;alles inaktiv
	out	(IMPAUS),a	;Schnittstelle aus
	ld	a,3
	out	(CTC+1),a	;Loeschen der Interrupts fuer DEND
	out	(CTC),a		;Loeschen der Interrupts fuer MAERK
 
;----------------------------------------
; Auswertung des eingelesenen Datenfeldes
;----------------------------------------
; Test auf CRC-Fehler
	in	a,(ST_PRT)
	bit	7,a		;CRC-Fehler ?
	jp	z,tdc_er
; Retten des Anfangs des ersten Datenfeldes
	ld	hl,k_flg	;Steuerbyte
	bit	7,(hl)
	jr	nz,no_sav
	set	7,(hl)		;Anfang ist gerettet
	ld	hl,(dt_br)
	ld	bc,13
	add	hl,bc
	res	4,h
	ld	de,sav_20
	ld	b,20h
	call	ldir23
no_sav
; Retten des letzten Datenbytes des Datenfeldes
	ld	hl,(dt_br)
	ld	bc,524
	add	hl,bc
	res	4,h
	ld	a,(hl)
	ld	(sav_fb),a
; naechsten Sektor lesen ?
	ld	hl,(dt_len)	;aktuelle Datenlaenge
	ld	bc,200h
	xor	a
	sbc	hl,bc
	ld	(dt_len),hl
	jr	z,um_20
	jp	nc,n_sec
; Zurueckschreiben des Anfangs des ersten Datenfeldes
um_20	ld	hl,dt
	ld	bc,13
	add	hl,bc
	ex	de,hl
	res	4,d
	ld	hl,sav_20
	ld	b,20h
	call	ldir32
	ld	hl,k_flg
	res	7,(hl)		;Flag ruecksetzen

	xor	a		;Flags loeschen
	ret			;Rueckkehr, wenn Lesen ok.

n_sec
; Bestimmung des naechsten Bereichs im WDC-RAM
	ld	hl,(dt_br)
	ld	bc,200h
	add	hl,bc
	res	4,h
	ld	(dt_br),hl
; Berechnung des Disk-Adresszaehlers fuer das Datenfeld
	ld	hl,(dt_br)
	ld	bc,11
	add	hl,bc
	res	4,h
	call	b_daz
	ld	(dsk_az),hl
; Setzen der Fehlerzaehler fuer den neuen Sektor
	ld	a,10
	ld	(sc_cnt),a
	ld	a,(c_zt)
	ld	(ct_zt),a	;Zaehler fuer Zeitueberwachung
	ld	a,(c_cd)
	ld	(ct_cd),a	;Zaehler fuer Fehler im Datenfeld
; Bestimmung des naechsten Sektors
	ld	a,(secs)
	ld	b,a
	inc	b
	ld	hl,ident+4	;HL zeigt auf Sektor
	inc	(hl)
	ld	a,(hl)		;Sektornummer
	cp	b		;max. Sektor + 1
	jp	c,nx_sec
	ld	(hl),1		;naechste Spur, Beginn mit Sektor 1
	dec	hl		;HL zeigt auf Kopf
	ld	a,(l_inc)	;Inkrementierungszaehler
	ld	b,a
	inc	b
nx_s1	push	bc		;Zaehler retten
	ld	a,(hds)
	ld	b,a
	ld	hl,ident+3
	inc	(hl)		;Kopfnummer inkr.
	ld	a,(hl)		;Kopfnummer
	cp	b		;Max. Kopf + 1
	jr	nc,nx_cy	;Sprung, wenn Zylinderwechsel noetig
nx_s2	pop	bc
	djnz	nx_s1
	ld	a,(ident+3)
	call	hd_sl		;Kopfselektierung
	jp	nx_sc
; Zylinderwechsel waehrend eines Tranfers
nx_cy	ld	(hl),0		;naechster Zyl, Beginn mit Kopf 0
	dec	hl		;Zyl high
	ld	d,(hl)
	dec	hl		;HL zeigt auf Zyl low
	ld	e,(hl)
	inc	de		;naechster Zylinder
	push	hl
	ld	hl,(cyls)
	xor	a
	sbc	hl,de
	pop	hl
	jp	p,nxs		;Sprung, wenn Zyl ok
	pop	bc
	ld	a,04h		;Fehlercode: Zyl zu gross
	ld	(err_cd),a
	cp	0
	ret
nxs	ld	(hl),e
	inc	hl
	ld	(hl),d
	call	step1
	jr	nx_s2
	
;--------------------------------------------------------------------
; Fehlerbehandlung fuer Fehler waehrend des Lesevorgangs
;--------------------------------------------------------------------
; es wird nicht der richtige Sektor gefunden
id_e	ld	a,(st_ina)	;alles inaktiv
	out	(DSKC2),a
	out	(IMPAUS),a	;Ausschaltimpuls
	ld	hl,sc_cnt	;Sektorzaehler
	dec	(hl)
	jp	nz,nx_sec
	ld	hl,sum_sc
	ld	a,0ah		;Fehlercode 0A
	call	d_err
	jp	nz,nx_sc	;sofort weitersuchen
	jp	err_rt		;Fehlerabbruch
; keine Marke waehrend der Zeit der Zeitueberwachung
err_mk	ld	hl,sum_zt
	ld	a,0bh		;Fehlercode 0B
	call	d_err
	jp	nz,nx_sec	;sofort weitersuchen
	jp	err_rt		;Fehlerabbruch
; Fehler im Datenfeld
tdc_er	ld	hl,sum_cd
	ld	a,0ch		;Fehlercode 0C
	call	d_err
	jp	z,err_rt
	call	dis_1		;fast eine Umdrehung abwarten
	ld	a,10
	ld	(sc_cnt),a
	jp	nx_sc
; Rueckkehr mit Fehler
err_rt	ld	(err_cd),a	;Fehlercode
	cp	0		;Flags setzen
	ret
 
;---  isr_m1  -------------------------------------------------
; ISR fuer den Interrupt bei der Markenerkennung des Kennfeldes
; (fuer Kennfeldlesen mit anschliessendem Datenfeldlesen
; CTC-Kanal 0
;--------------------------------------------------------------
isr_m1
	out	(DSKC2),a	;'MEN=inaktiv'
	exx
	ld	a,b
	out	(DSKC2),a	;alles inaktiv
	out	(IMPAUS),a	;Abschaltimpuls
; Vorbereitung fuer das Lesen des Datenfeldes
	ld	(hl),a		;Laden Diskadresszaehler
	ld	hl,isr_m7	;ISR-Adresse fuer Int. vor Datenfeld
	ld	(isr_mk),hl	;Umladen der ISR-Adresse
	ld	a,e
	out	(DSKEA),a	;DEND-Adresse fuer Markenanzahl
	ld	a,(cnt_st)
	res	6,a		;DA12 = 0
	ld	(cnt_st),a
	out	(CNTST),a	;Umschaltung des RAM-Bereichs
	ld	a,c
	out	(DSKC2),a	;alles aktiv zur Markenerkennung
	exx

;--- isr_al  ------------------------------------------------
; ISR fuer allgemeine Verwendung
; alle CTC-Kanaele
;------------------------------------------------------------
isr_al	ei
	reti

;---  isr_m9  ------------------------------------------------
; ISR fuer Interrupt bei Markenerkennung des Kennfeldes
; (nur Kennfeldlesen)
; CTC-Kanal 0
;-------------------------------------------------------------
isr_m9
	out	(DSKC2),a	;'MEN=inaktiv'
	nop
	ld	a,(st_ina)
	out	(DSKC2),a	;alles inaktiv
	out	(IMPAUS),a	;Abschaltimpuls
	ld	a,3
	out	(CTC),a		;Reset Markenerkennung
	pop	hl		;Verwerfen der Returnadresse
	ld	hl,hlt_2	;neue Returnadresse
	push	hl
	ei
	reti

;---  isr_m7  -------------------------------------------------
; ISR fuer Interrupt bei Markenerkennung des Datenfeldes
; CTC-Kanal 0
;--------------------------------------------------------------
isr_m7	ld	a,3
	out	(CTC),a		;Reset Markenerkennung
	ei
	reti

;---  isr_zt  --------------------------------
; ISR fuer das Ansprechen der Zeitueberwachung
; (CTC-Kanal 3)
;---------------------------------------------
isr_zt
	ld	a,3
	out	(CTC),a
	out	(CTC+3),a	;Reset
	pop	hl		;Verwerfen der Returnadresse
	ld	hl,err_mk
	push	hl		;neue Returnadresse
	ei
	reti
 
;---  ct_um  ------------------------------------
; Umladen bzw. Loeschen der Maximalfehlerzellen
; hl: Adresse des Momentanwerts 'mx_xx'
; hl+1: Adresse des Maximalwerts 'max_xx'
;------------------------------------------------
ct_um	ld	a,(hl)		;'mx_xx'
	inc	hl		;'max_xx'
	cp	(hl)
	jr	c,ct_um1
	ld	(hl),a		;neuen Maxwert eintragen
ct_um1	xor	a
	dec	hl
	ld	(hl),a		;Loeschen Momentanwert 'mx_xx'
	ret

;---  d_err  -------------------------------------------------------------
; Ausschalten der Diskschnittstelle nach Fehler und Behandlung der Zaehler
; hl: Adresse des Beginns der speziellen Zaehlerzellen
; a: Fehlercode
;-------------------------------------------------------------------------
d_err	ld	b,a
	ld	a,(st_ina)	;alles inaktiv
	out	(DSKC2),a	;Steuerbits inaktiv
	out	(IMPAUS),a	;Ausschaltimpuls
	inc	(hl)		;Low-Teil 'sum_xx'
	push	af
	inc	hl		;Zeiger auf High-Teil 'sum_xx'
	pop	af
	jr	nz,sm_ok
	inc	(hl)		;Inc High-Teil 'sum_xx'
sm_ok	inc	hl		;Zeiger auf 'mx_xx'
;	inc	(hl)		;Inc 'mx_xx'
	inc	hl		;'max_xx' uebergehen
	inc	hl		;Zeiger auf 'ct_xx'
	dec	(hl)		;Dec 'ct_xx'
	ret	nz		;Return fuer Wiederholung
	xor	a
	ld	a,b		;Fehlercode
	ret			;Return im Fehlerfall

;---  dis_1  -------------------------------------------------
; Programmierung Zeitinterrupt fuer fast eine Plattenumdrehung
;-------------------------------------------------------------
dis_1	push	af
	ld	a,0a7h
	out	(CTC+1),a
	ld	a,230
	out	(CTC+1),a
	halt
	ld	a,3
	out	(CTC+1),a
	pop	af
	ret
 eject
;---  id_ein  -----------------------
; Eintragen des Kennfeldes
; hl: Zeiger auf 'Cy-L, Cy-H, Hd, Sc'
;------------------------------------
id_ein	ld	de,ident
	ld	a,0feh
	ld	(de),a
	inc	de
	ld	bc,4
	ldir
	ret

;--- step1  --------------------------------------------------------
; Verschiebung der Koepfe um einen Zylinder nach innen
;-------------------------------------------------------------------
step1	ld	a,(dsk_c1)	;Ausgabebyte fuer DSKC1
	set	3,a		;SD nach innen
	out	(DSKC1),a
	xor	1		;ST ein
	di
	out	(DSKC1),a
	xor	1		;ST aus
	out	(DSKC1),a
	ei
step11	in	a,(ST_PRT)
	bit	3,a
	jr	nz,step11
	ld	a,(cmd_br-7)	;Laufwerk
	add	a,a
	ld	c,a
	ld	b,0
	ld	hl,cy_tab	;Positionstabelle
	add	hl,bc
	ld	e,(hl)		;Low-Teil
	inc	hl
	ld	d,(hl)		;High-Teil
	inc	de		;Inc Zylinder
	ld	(hl),d
	dec	hl
	ld	(hl),e
	ret			;ok.

;---  hd_sl  ------------------------------------------------------
; Kopfselektierung
; A: Kopfnummer 0...max, Test auf Richtigkeit muss vorher erfolgen
;------------------------------------------------------------------
hd_sl	rrca
	rrca
	rrca
	rrca
	ld	b,a
	ld	a,(dsk_c1)
	and	0fh		;Loeschen der HS-Bits
	or	b		;neue HS-Bits
	ld	(dsk_c1),a
	out	(DSKC1),a
	ret

;---  sc_def  ------------------------------------------------
; Berechnung der Sektoradresse mit Defektspurberuecksichtigung
; Sektoradresse vorher und nachher in 'ident'
;-------------------------------------------------------------
; l_inc: Anzahl der Spurinkrementierungen, um bei Mehrblock-
; transfers auf die richtige Spuradresse zu kommen
;-------------------------------------------------------------
sc_def	xor	a
	ld	(l_inc),a	;Loeschen Spurinkrementierungen
	ld	de,(df_end)	;Adresse
	ld	a,(de)
	ld	l,a
	inc	de
	ld	a,(de)
	or	l
	ret	z		;keine Adressinkr. noetig
	ex	af,af'
	xor	a		;Loeschen Inkrements
	ex	af,af'
	ld	de,(df_tab)	;Adresse der Tabelle der Defektspuren
	dec	de
; naechste Spur aus der BTT
sc2	inc	de		;Zeiger auf Zylinder High
	ld	a,(de)		;Zyl High
	ld	h,a
	inc	de
	ld	a,(de)		;Zyl Low
	ld	l,a
	inc	de		;Zeiger auf Kopfnummer
; Endekennzeichen ?
	push	hl
	ld	bc,0ffffh	;Endekennzeichen
	xor	a
	sbc	hl,bc
	pop	hl
	jp	z,sc4		;Sprung, wenn Ende der Defektspuren
; Spur_def <= geforderte Spur ?
	ld	bc,(ident+1)	;Zyl-gef
	xor	a
	sbc	hl,bc
	jp	c,sc3
	jp	nz,sc4
; Cyl-gef = Cyl-def, Test der Kopfnummer
	ld	a,(de)		;Kopf defekt
	ld	b,a
	ld	a,(ident+3)	;Kopf
	sub	b		;Kopf - Kopf-def
	jp	c,sc4
; Zaehler erhoehen
sc3	ex	af,af'
	inc	a
	ex	af,af'
	jp	sc2
; Erhoehung der Spuradresse
sc4	dec	de		;Zeiger auf vorhergehenden BTT-Eintrag
	dec	de
	dec	de
	ex	af,af'
	ld	b,a
	xor	a		;Zaehler loeschen
	ex	af,af'
	ld	a,b
	cp	0
	jr	z,sc7
	ld	b,a		;Anzahl der Inkrementierungen
sc5	ld	a,(hds)
	ld	c,a
	ld	a,(ident+3)	;Kopf
	inc	a
	ld	(ident+3),a
	cp	c
	jr	nz,sc6
	xor	a
	ld	(ident+3),a
	ld	hl,(ident+1)	;Zylinder
	inc	hl
	ld	(ident+1),hl
sc6	djnz	sc5
	jp	sc2		;Test auf weitere BTT-Eintragungen
; Test, ob naechste Spur defekt ist (fuer Mehrblocktransfer)
sc7	ld	hl,(dt_len)
	ld	bc,201h
	xor	a
	sbc	hl,bc
	ret	c		;Return, wenn nur ein Block
	ld	hl,(ident+1)	;Zyl_akt
	ld	a,(ident+3)	;Hd_akt
	ld	b,a
sc9	inc	de		;DE: Zeiger auf Zyl_btt_high
	inc	b		;naechster Kopf
	ld	a,(hds)		;Kopfanzahl
	ld	c,a
	ld	a,b
	cp	c
	jr	c,sc8
	ld	b,0		;naechster Kopf ist 0
	inc	hl		;naechster Zyl
sc8	ld	a,(de)		;Zyl_btt_high
	cp	h		;Zyl_akt_high
	ret	nz		;naechste Spur ist keine BTT-Spur
	inc	de
	ld	a,(de)		;Zyl_btt_low
	cp	l		;Zyl_akt_low
	ret	nz		;naechste Spur ist keine BTT-Spur
	inc	de
	ld	a,(de)		;Hd_btt
	cp	b		;Hd_akt
	ret	nz		;naechste Spur ist keine BTT-Spur
	ld	a,(l_inc)	;Zaehler inkrementieren
	inc	a
	ld	(l_inc),a
	jr	sc9		;naechste Spur auch BTT-Spur ?
 eject
;***********************************************************
; Schreiben auf Hard-Disk
;***********************************************************
write
; Setzen der Fehlerzaehler
	ld	de,c_zt
	ld	hl,fe_n
	ld	bc,7
	ldir
; Auswertung des Kommandocodes
	ld	a,(cmd_br-8)	;Kommandocode
	cp	0c2h		;Schreiben 'PAR&BTT' ?
	jp	z,wr_df
	call	wr_dt		;Schreiben des Datenfeldes
	jp	nz,error
	jp	begin
	
;----------------------------------------------------------------------
; Schreiben 'PAR&BTT' auf den Sektor 0/0/1
;----------------------------------------------------------------------
wr_df
	ld	hl,200h
	ld	(cmd_br-2),hl
	ld	(dt_len),hl
; Eintragen 'DEFEKT'
	ld	hl,dt
	ld	(dt_br),hl
	ld	bc,13
	add	hl,bc
	res	4,h
	push	hl
	ex	de,hl
	ld	hl,txt_df
	ld	bc,6
	ldir
; Umladen der Defektspurtabelle
	ld	hl,(df_end)
	ld	bc,40*3+3+2
	ldir
; Eintragen 'PARMTR'
	pop	hl
	ld	bc,256
	add	hl,bc
	res	4,h
	ex	de,hl
	ld	hl,txt_pm
	ld	bc,6
	ldir
; Umladen der Parametertabelle
	call	par_v
; Untersuchung der umgeladenen Tabellen
	call	t_par		;Test der Parameter
	jp	nz,err_17	;Fehler, kein Schreiben
	call	t_btt		;Test der Defektspurtabelle
	jr	nz,wr_df1	;Sprung, wenn fehlerhafte BTT
	call	btt_s		;Setzen des BTT-Bits
	jr	wr_df2
; Loeschen des BTT-Bereichs 
wr_df1	call	btt_cl		;Loeschen der BTT im 3000-RAM
	call	btt_v		;Umladen in den Datenbereich des 1. Sektors
	call	btt_r		;Ruecksetzen des BTT-Bits
wr_df2	call	wr_dt		;Aufruf des Schreibprogramms
	jr	nz,wr_df7
	call	par_s		;Setzen des PAR-Bits
	jp	entry
wr_df7	call	par_r		;Ruecksetzen des PAR-Bits
	call	btt_r		;Ruecksetzen des BTT-Bits
	jp	error

;--- wr_dt -----------------------------------------------------------
; UP Aufruf des Schreibprogramms
;---------------------------------------------------------------------
wr_dt
	ld	a,4
	ld	(ct_ra),a	;Zaehler fuer Schreibwiederholungen
wr_dt1	call	wrt_dt		;Schreiben des Datenfeldes
	ret	nz		;Schreibfehler
	ld	a,(cmd_br-8)	;Kommandocode
	bit	7,a		;'Read after Write' ?
	ret	z
; Setzen der Fehlerzaehler fuer das Lesen
	ld	de,c_zt
	ld	hl,fe_n
	ld	bc,7
	ldir
; Lesen von Hard-Disk
	ld	a,0
	ld	(rd_mod),a
	call	rd_id
	ret			;Ruecksprung bei 'Read after Write'
; Fehler, Fehlerzaehlerbehandlung
	ld	hl,(sum_ra)
	inc	hl
	ld	(sum_ra),hl
	ld	hl,mx_ra
;	inc	(hl)
	ld	hl,ct_ra
	dec	(hl)
	jp	nz,wr_dt1
	ld	a,11h		;Fehlercode 11h
	ld	(err_cd),a
	cp	0
	ret			;Fehlerruecksprung bei 'Read after Write'
 eject
;***************************************************************************
; UP wrt_dt
; Schreiben des Datenfeldes eines Sektors
;***************************************************************************
wrt_dt
; Initialisierung von 'dt_br'
	ld	hl,dt
	ld	(dt_br),hl
; Setzen der Fehlerzaehler fuer das Schreiben
	ld	a,3
	ld	(ct_fw),a	;Zaehler fuer Fault-Write-Fehler
	ld	a,3
	ld	(ct_wi),a	;Zaehler fuer Identfehler vor dem Schreiben
; Umspeichern des aktuellen Sektors,
; da in 'ident' fuer das Lesen geaendert wird
	ld	a,(ident+4)	;geforderte Sektornummer
	ld	(sc_akt),a	;retten des aktuellen Sektors
; Bestimmung der vorhergehenden Sektornummer (fuer das Schreiben)
suche	ld	a,(sc_akt)	;aktuelle Sektornummer
	ld	hl,sc_tab	;Tabelle des Sektorversatz
vs_sl	cp	(hl)
	jr	z,vs_ok
	inc	hl
	jr	vs_sl
vs_ok	dec	hl		;HL zeigt auf Vorgaenger
	ld	a,(hl)
	cp	0ffh		;vor Sektor 1
	jr	nz,vs_ok1
	ld	a,(secs)
	ld	c,a
	ld	b,0
	ld	hl,sc_tab-1
	add	hl,bc
	ld	a,(hl)		;letzter Sektor vor Sektor 1
vs_ok1	ld	(ident+4),a	;vorhergehende Sektornummer
; Test auf Abschluss eventueller Host-Datentransfers
wr_sl1	ld	a,(hst_by)
	bit	7,a
	jr	nz,wr_sl1
; Retten des Praeambelbereichs des Sektors
	ld	hl,(dt_br)	;Beginn der Praeambel
	ld	de,s_prae	;Rettungsbereich
	ld	b,13
	call	ldir23		;Umladen
; Lesen des vorhergehenden Sektorkennfeldes
	ld	a,1		;Kennfeldlesen
	ld	(rd_mod),a
	call	rd_id1		;Kennfeldlesen ohne Bereichsweiterschaltung
	ret	nz		;Fehler im Kennfeld
; Retten des Postambel-Bereichs
nx_wrt	ld	hl,(dt_br)
	ld	bc,525
	add	hl,bc		;Beginn der Postambel
	res	4,h
	ld	de,s_post	;Rettungsbereich fuer Postambel
	ld	bc,12		;Laenge der Postambel
	ldir
; Eintragen der Praeambel und der Postambel
	ld	de,(dt_br)	;Beginn der Praeambel
	ld	hl,prae		;Praeambel
	ld	bc,11		;Laenge der Praeambel
	ldir
	res	4,d
	ld	bc,2
	ldir
	ld	hl,(dt_br)
	ld	bc,525
	add	hl,bc
	res	4,h
	ex	de,hl		;Beginn der Postambel
	ld	hl,post		;Postambel
	ld	bc,12		;Laenge der Postambel
	ldir
; Setzen des Sektor-Index-Zaehlers
	ld	a,4
	ld	(c_s_ix),a
; Vorbereitung des Schreibens
nx_wr1	ld	a,(cnt_st)
	res	7,a		;DR/W: Disk-->RAM (Lesen)
	set	6,a		;DA12=1
	ld	(cnt_st),a
	out	(CNTST),a
	ld	hl,(kf_br)
	dec	hl
	ld	a,l
	out	(DSKEA),a	;DEND-Adresse bereinigen
	ld	a,0a1h
	out	(BM_D),a	;Datenbits der Marke
	ld	hl,(kf_br)
	call	b_daz
	ld	(hl),a		;Diskadresszaehler fuer das Kennfeld
	ld	hl,isr_m2	;Eintragen der neuen ISR
	ld	(isr_mk),hl
	ld	hl,(dt_br)	;Anfangsadresse der Daten
	call	b_daz		;Berechnung des Disk-Adresszaehlers
	push	hl
	exx
	ex	af,af'
	ld	hl,(dt_br)	;DEND-Adresse fuer Markeneinblenden
	ld	bc,11
	add	hl,bc
	ld	e,l
	pop	hl		;Disk-Adresszaehler
	ld	a,(st_wa)
	ld	c,a		;'WG=aktiv'
	res	3,a		;'WG=inaktiv'
	ld	b,a
	ld	a,4		;PRAEOFF & MFM & eine Marke
	ld	d,a
	push	hl
	push	bc
	ld	a,(cmd_br-7)	;akt. Laufwerk
	call	cy_pos
	ld	b,(hl)		;Low-Teil Zylinderposition
	inc	hl
	ld	h,(hl)		;High-Teil Zylinderposition
	ld	l,b
	ld	bc,(pre_cy)	;Praekompensationsbeginn
	xor	a
	sbc	hl,bc
	pop	bc
	pop	hl
	jr	c,pre_ok
	ld	a,0ch		;PRAEON & MFM & eine Marke
	ld	d,a
pre_ok
	ld	a,(cnt_st)
	set	7,a		;DR/W: RAM-->Disk
	res	6,a		;DA12=0
	ld	(cnt_st),a
	ex	af,af'
	exx
; Programmierung CTC/0 auf MAERK
	ld	a,0c7h
	out	(CTC),a
	ld	a,1
	out	(CTC),a
; Einschalten der DSS zur Erkennung des Kennfeldes des zu schreibenden S.
	ld	hl,(kf_br)
	ld	bc,5
	add	hl,bc		;zeigt auf Sektornummer im Kennfeld
	ld	a,(sc_akt)	;aktuelle Sektornummer
	ld	b,a
	ld	a,(st_min)	;Schnittstelle aktiv mit 'MEN=inaktiv'
	out	(DSKC2),a
	ld	a,(st_vb)	;'DEN=inaktiv' als Vorbereitung zum Abschalten
	out	(DSKC2),a
	ld	a,3		;Vorbereitung fuer CTC-Reset
	halt
; ------ Warten auf drei A1 des Kennfeldes vor dem Schreiben ------
; ------ Interruptserviceroutine 'isr_m2' ------------------------- 
	ld	hl,(kf_br)
	ld	bc,5
	add	hl,bc
	ld	a,(sc_akt)
	ld	b,a
	ld	a,(hl)
	cp	b		;Sektornummer richtig ?
	jp	nz,wr_err
	ld	hl,(dt_br)
	ld	bc,93
	add	hl,bc
	ld	a,l
	out	(DSKEA),a	;DEND-Adresse fuer das Umschalten
	ld	a,0d5h
	out	(CTC+1),a
	ld	a,2		;Interrupt nach zwei DEND
	out	(CTC+1),a
; Rueckschreiben des Praeambelbereichs des Sektors
	ld	hl,(dt_br)
	ex	de,hl
	ld	hl,s_prae
	ld	bc,11
	ldir
	res	4,d
	ld	bc,2
	ldir
	halt			;Warten auf das 2. DEND
; ----  Warten auf Zwischeninterrupt  ------------------- 

	ld	hl,(dt_br)
	ld	bc,525		;DEND-Adresse zum CRC-Einblenden
	add	hl,bc
	ld	a,l
	out	(DSKEA),a
	ld	a,(st_wc)	;Steuerbits zum CRC-Einblenden
	out	(DSKC2),a
	ld	a,0d7h
	out	(CTC+1),a
	ld	a,1
	out	(CTC+1),a	;Interrupt beim CRC-Einblenden
; Vorbereitung der nachfolgenden Operationen
	ld	a,(st_ina)	;alles inaktiv
	halt			;Warten auf den CRC-Interrupt
; ---  Warten  ------------------- 

	out	(DSKC2),a
	out	(IMPAUS),a	;Disk-Schnittstelle aus
	ld	a,3
	out	(CTC+1),a	;CTC/1-Reset (DEND)
	in	a,(ST_PRT)
	bit	5,a		;'Fault write=aktiv ?'
	jp	z,wr_fw
; naechsten Sektor schreiben ?
	ld	hl,(dt_len)	;aktuelle Datenlaenge
	ld	bc,200h
	xor	a
	sbc	hl,bc
	ld	(dt_len),hl
	ret	z		;Schreiben ok.
	ret	c		;Schreiben ok.

; Berichtigung des Postambelbereichs
	ld	hl,(dt_br)
	ld	bc,525
	add	hl,bc
	res	4,h
	ex	de,hl		;Postambel
	ld	hl,s_post	;Rettungsbereich fuer Postambel
	ld	bc,12
	ldir
; Bestimmung der RAM-Adressen fuer den naechsten Sektor
	ld	hl,(dt_br)
	ld	bc,200h
	add	hl,bc
	res	4,h
	ld	(dt_br),hl
; Loeschen des Zylinderwechsel-Flag
	ld	hl,ch_cyl
	ld	(hl),0
; Bestimmung des naechsten Sektors
	ld	a,(secs)
	ld	b,a
	inc	b
	ld	hl,sc_akt	;aktueller Sektor
	inc	(hl)
	ld	a,(hl)		;Sektornummer
	cp	b		;max. Sektor + 1
	jp	c,nx_wrt
; Kopfwechsel waehrend eines Zugriffs
	ld	a,1
	ld	(sc_akt),a
	ld	hl,ident+4	;zeigt auf Sektornummer
	ld	(hl),a		;naechster Sektor, Beginn mit Sektor 1
	dec	hl		;HL zeigt auf Kopf
	ld	a,(l_inc)	;Inkrementierungszaehler
	ld	b,a
	inc	b
n_cy1	push	bc
	ld	a,(hds)
	ld	c,a
	ld	hl,ident+3	;Kopfnummer
	inc	(hl)		;Kopfnummer inkr.
	ld	a,(hl)		;Kopfnummer
	cp	c		;Max. Kopf + 1
	jr	nc,n_cy		;Sprung, wenn Zylinderwechsel
n_cy2	pop	bc
	djnz	n_cy1
	ld	a,(ident+3)
	call	hd_sl		;Kopfselektierung
	ld	a,(ch_cyl)
	cp	0
	jp	z,nx_wrt	;Schreiben des Sektors
	jp	suche		;Orientieren auf der Spur
; Zylinderwechsel waehrend eines Zugriffs
n_cy	ld	(hl),0		;naechster Zyl, Beginn mit Kopf 0
	dec	hl		;Zyl high
	ld	d,(hl)	
	dec	hl		;HL zeigt auf Zyl low
	ld	e,(hl)
	inc	de		;naechster Zylinder
	push	hl
	ld	hl,(cyls)	;max. Zylinder + 1
	xor	a
	sbc	hl,de
	pop	hl
	jp	p,nxd		;Sprung, wenn Zyl ok.
	pop	bc
	ld	a,04h		;Fehlercode: Zyl zu gross
	ld	(err_cd),a
	cp	0
	ret			;Fehlerrueckkehr
; Kopfauswahl bzw. Zylinderauswahl
nxd	ld	(hl),e
	inc	hl
	ld	(hl),d
	call	step1
	ld	hl,ch_cyl
	ld	(hl),1
	jr	n_cy2

; Fehler vor dem Schreiben (falsche Sektornummer)
wr_err	ld	hl,c_s_ix	;erlaubte falsche Sektornummern
	dec	(hl)
	jp	nz,nx_wr1	;sofort den naechsten Sektor schreiben
	ld	hl,sum_wi
	ld	a,12h		;Fehlercode 12
	call	d_err
	jr	nz,wr_er2
	ld	(err_cd),a
	cp	0
	ret
wr_er2	call	r_post		;Berichtigung des Postambelbereichs
	ld	hl,(dt_br)
	ex	de,hl
	ld	hl,s_prae
	ld	bc,11
	ldir
	res	4,d
	ld	bc,2
	ldir
	jp	suche		;neu orientieren auf der Spur

; Fehler beim Schreiben
wr_fw	ld	a,(st_ina)
	set	7,a		;'FR=aktiv'
	out	(DSKC2),a
	out	(IMPAUS),a	;Schnittstelle aus
	res	7,a		;FR ruecksetzen
	out	(DSKC2),a
	ld	hl,sum_fw
	ld	a,10h		;Fehlercode 10h
	call	d_err
	jr	nz,wr_fw1
	ld	(err_cd),a
	cp	0
	ret
wr_fw1	call	r_post		;Berichtigung des Postambelbereichs
	jp	suche
 
;--- isr_m2 ---------------------------------------------------------
; ISR fuer den Interrupt nach Erkennung von drei A1 vor dem Kennfeld
; beim Schreiben
;--------------------------------------------------------------------
isr_m2
	out	(CTC),a		;Reset CTC/0 (MAERK)
	out	(IMPAUS),a	;Schnittstelle aus
	ld	a,(hl)
	cp	b
	jr	nz,isr_m3
	exx
	ex	af,af'
	out	(CNTST),a	;Richtungsumsteuerung der Schnittstelle
	ld	(hl),a		;neuer Disk-Adresszaehler
	ld	a,e		;DEND-Adresse zum Markeneinblenden holen
	out	(DSKEA),a
	ld	a,d
	out	(BM_D),a	;MFM
	ld	a,b
	out	(DSKC2),a	;alles aktiv zum Schreiben
	ld	a,c		;'WG=aktiv'
	out	(DSKC2),a
	ex	af,af'
	exx
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	ld	a,(st_wm)
	out	(DSKC2),a	;'MEN=inaktiv' & 'CRCEN=inaktiv'
isr_m3
	ei
	reti

;--  r_post  --------------------------------------------------
; Rueckschreiben Postambelbereich
;--------------------------------------------------------------
r_post	ld	hl,(dt_br)
	ld	bc,525
	add	hl,bc
	res	4,h
	ex	de,hl		;Postambel
	ld	hl,s_post	;Rettungsbereich fuer Postambel
	ld	bc,12
	ldir
	ret

;---  ldir23  ------------------------------------------------
; Umladen von RAM-Bereich 2000...2fff nach RAM-Bereich 3000,
; wobei die Adresse im 2000-Bereich "rund" laeuft
; HL: Quelle (2000...2fff),  DE: Ziel (3000), B: Anzahl
;-------------------------------------------------------------
ldir23	ld	a,(hl)
	ld	(de),a		;Umladen
	inc	hl
	res	4,h
	inc	de
	djnz	ldir23
	ret

;---  ldir32  ------------------------------------------------
; Umladen von RAM-Bereich 3000 nach RAM-Bereich 2000...2fff,
; wobei die Adresse im 2000-Bereich "rund" laeuft
; HL: Quelle (3000),  DE: Ziel (2000...2fff), B: Anzahl
;-------------------------------------------------------------
ldir32	ld	a,(hl)
	ld	(de),a		;Umladen
	inc	hl
	inc	de
	res	4,d
	djnz	ldir32
	ret
 eject
;**********************************
; Formatierprogramme fuer Hard-Disk
;**********************************
format
	ld	hl,200h
	ld	(cmd_br-2),hl	;Laenge setzen
	ld	(dt_len),hl
; Zeitkonstante schon bestimmt ?
	ld	a,(ztk)
	and	a
	jp	nz,ztk_ok
	ld	a,(cmd_br-8)
	cp	84h		;Spurloeschen ?
	jp	z,ztk_ok
;-----------------------------------------------------------
; Bestimmung der Zeitkonstanten fuer das Formatieren
;-----------------------------------------------------------
; Laufwerk mit 17 Sektoren ?
	ld	a,(ztk_41)
	ld	(ztk),a		;Voreinstellen ztk
	ld	a,(secs)
	cp	17
	jp	z,ztk_ok
	ld	hl,isr_al
	ld	(isr_ix),hl	;ISR-Adresse
	ld	hl,ztk_f	;HL zeigt auf Hilfsfeld
	ld	b,8		;8 Durchlaeufe
ztk_1	ld	a,0d5h		;Counter, EI
	out	(CTC+3),a
	ld	a,1
	out	(CTC+3),a
	halt
;---- Warten auf den naechsten IX-Impuls ----
;---- ISR 'isr_al' --------------------------
	ld	a,027h		;Timer ohne Interrupt
	out	(CTC+1),a
	ld	a,0
	out	(CTC+1),a
	halt
;---- Warten auf den naechsten IX-Impuls ----
;---- ISR 'isr_al' --------------------------
	ld	a,3		;RESET
	out	(CTC+3),a
	in	a,(CTC+1)	;Lesen Zaehlerstand
	ld	(hl),a		;sichern
	ld	a,3		;RESET
	out	(CTC+1),a
	inc	hl
	djnz	ztk_1
; Summieren des Hilfsfeldes
	ld	b,8
	ld	hl,0
	ld	de,ztk_f
ztk_2	push	bc
	ld	b,0
	ld	a,(de)
	ld	c,a
	add	hl,bc
	inc	de
	pop	bc
	djnz	ztk_2
; Division durch 8
	ld	b,3
ztk_3	srl	l
	srl	h
	jr	nc,ztk_4
	set	7,l
ztk_4	djnz	ztk_3
	ld	a,l
; 'ztk' im 41,4-MHz-Bereich ?
	ld	hl,zmn_41	;Zeiger auf Grenzwerte
	cp	(hl)
	jr	c,ztk_6		;Sprung, wenn kleiner
	inc	hl		;Zeiger auf 'zmx_41'
	ld	b,(hl)
	inc	b
	cp	b
	jr	nc,ztk_5	;Sprung, wenn groesser
	ld	a,(ztk_41)	;Zeitkonstante fuer 41,4 MHz
	ld	(ztk),a
	jr	ztk_ok
; 'ztk' im 40,0-MHz-Bereich ?
ztk_5	dec	hl
	dec	hl
	dec	hl		;Zeiger auf 'zmn_40'
	cp	(hl)
	jr	c,ztk_6		;Sprung, wenn kleiner
	inc	hl		;Zeiger auf 'zmx_40'
	ld	b,(hl)
	cp	b
	jr	nc,ztk_6	;Sprung, wenn groesser
	ld	a,(ztk_40)	;Zeitkonstante fuer 40,0 MHz
	ld	(ztk),a
	jr	ztk_ok
; Fehler waehrend der Zeitkonstantenbestimmung
ztk_6	xor	a
	ld	(ztk),a
	jp	err_15
; Ende der Zeitkonstantenbestimmung

ztk_ok	ld	a,(cmd_br-8)
	cp	04h
	jp	z,frmt1
	cp	14h
	jp	z,frmt2
	cp	24h
	jp	z,frmt3
	cp	44h
	jp	z,frmt4
	cp	84h
	jr	z,frmt1
	jp	err_1
;---------------------------------
; Einfaches Formatieren einer Spur
;---------------------------------
frmt1
	call	ft_trk
	jp	nz,error
	jp	begin
 
;---------------------------------------------
; Formatieren einer Spur mit Test der Sektoren
;---------------------------------------------
frmt2
; Formatieren der Spur
	call	ft_trk
	jp	nz,error
; Ruecklesen der Einzelsektoren
frmt4	
	ld	de,c_zt
	ld	hl,fe_n
	ld	bc,7
	ldir
	ld	a,1
	ld	(ident+4),a	;Sektor
	ld	hl,2400h	;18 Sektoren
	ld	a,(secs)
	cp	18
	jr	z,frmt41
	ld	hl,2200h	;17 Sektoren
frmt41	ld	(dt_len),hl
	ld	a,0
	ld	(rd_mod),a
	call	rd_id
	jp	nz,error
	jp	begin
 
;-------------------------------------------------------------
; Formatieren einer Spur mit Defektfeststellung und Eintragung
; in die Defektspurtabelle
;-------------------------------------------------------------
frmt3
	ld	de,c_zt
	ld	hl,fe_f1
	ld	bc,7
	ldir
	ld	a,3
	ld	(dl_f),a	;Anfangswert
f301
	ld	hl,dl_f
	dec	(hl)
	jp	z,f_def
	call	ft_trk		;Formatieren einer Spur
	jp	nz,f301
; Ruecklesen der Einzelsektoren
	ld	a,2
	ld	(dl_r),a	;Anfangswert
f302	ld	hl,dl_r
	dec	(hl)
	jp	z,begin		;Formatierung ok.
	ld	a,1
	ld	(ident+4),a	;Sektor
	ld	hl,2400h	;18 Sektoren
	ld	a,(secs)
	cp	18
	jr	z,f303
	ld	hl,2200h	;17 Sektoren
f303	ld	(dt_len),hl
	ld	a,0
	ld	(rd_mod),a
	call	rd_id
	jp	nz,f301
	jp	f302
 
; Eintragen der Adresse der Defektspur in die Defektspurtabelle
; df_tab : Anfangsadresse der Defektspurtabelle
; df_end : Adresse des Beginns der Eintragung des Endekennzeichens
f_def
	xor	a
	ld	(err_cd),a	;Loeschen Fehlercode
	ld	hl,(df_end)
	ld	c,(hl)
	inc	hl
	ld	b,(hl)		;BC: Anzahl schon eingetragener Spuren
	ld	hl,39*3		;Maximum = 120 ( -3)
	xor	a
	sbc	hl,bc
	jp	c,err_16	;Fehler 16: BTT voll
	ld	hl,(df_tab)	;Beginn der aktuellen BTT
f350	ld	a,(ident+2)	;High-Teil Zylinder
	ld	b,a
	ld	a,(hl)
	cp	b
	jp	c,f351
	jp	nz,f358
	inc	hl
	ld	a,(ident+1)	;Low-Teil Zylinder
	ld	b,a
	ld	a,(hl)
	cp	b
	jp	c,f352
	jp	nz,f357
	inc	hl
	ld	a,(ident+3)	;Kopf
	ld	b,a
	ld	a,(hl)
	cp	b
	jp	c,f353
	jp	z,begin		;Eintragung schon vorhanden
; Verschieben der Resttabelle nach hinten
	dec	hl
f357	dec	hl
f358	ex	de,hl		;DE: unterste Adresse zum Umladen
	push	de
	ld	hl,(df_end)	;Adresse des Endwertes
	push	de
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	ex	de,hl
	pop	de
	ld	bc,(df_tab)
	add	hl,bc
	inc	hl
	inc	hl
	inc	hl
	push	hl		;HL: erster freier Platz nach Endek.
	xor	a
	sbc	hl,de
	push	hl
	pop	bc		;BC: Anzahl der umzuladenden Bytes
	pop	hl
	dec	hl		;Quelle: oberstes Byte
	push	hl
	pop	de
	inc	de
	inc	de
	inc	de		;Ziel: oberstes Byte
	lddr			;Verschieben der Resttabelle
	pop	de
	ex	de,hl
; Eintragen der defekten Spuradresse
	ld	a,(ident+2)	;High-Teil Zylinder
	ld	(hl),a
	inc	hl
	ld	a,(ident+1)	;Low-Teil Zylinder
	ld	(hl),a
	inc	hl
	ld	a,(ident+3)	;Kopf
	ld	(hl),a
	ld	de,(df_end)
	ld	a,(de)
	ld	l,a
	inc	de
	ld	a,(de)
	ld	h,a
	ld	bc,3
	add	hl,bc
	dec	de
	ld	a,l
	ld	(de),a
	inc	de
	ld	a,h
	ld	(de),a
; Ausgabe der Adresse des defekten Sektors an den Host	
	ld	bc,3
	ld	hl,ident+3
	ld	a,3		;Status 3
	call	p_wc_h
	jp	begin
; naechste Eintragung
f351	inc	hl
f352	inc	hl
f353	inc	hl
; Ende-Test
	ld	a,(hl)
	cp	0ffh
	jp	nz,f350
	jp	f358
 eject
;*************************************************
; Formatieren der aktuellen Spur
;*************************************************
ft_trk
	ld	a,(st_ina)
	out	(DSKC2),a	;alles inaktiv
	out	(IMPAUS),a
; Loeschen des RAM von 2000 bis 2fff
	ld	hl,ram		;Anfangsadresse
	ld	bc,0fffh	;Laenge
	ld	de,ram+1
	ld	(hl),0		;Loeschbyte
	ldir
; Programmierung der Transfer-Richtung
	ld	a,(cnt_st)
	set	7,a		;RAM-->Disk
	res	6,a		;DA12=0
	ld	(cnt_st),a
	out	(CNTST),a
; Laden des Diskadresszaehlers
	ld	hl,2000h
	call	b_daz
	ld	(hl),a
; Einschalten der Disk-Schnittstelle
	ld	a,(st_wm)	;'WG=aktiv' & 'DEN=aktiv'
	ld	b,a
	res	3,a
	out	(DSKC2),a	;Schnittstelle in Betrieb
	ld	a,b
	out	(DSKC2),a
; Warten auf den 2.Index
	ld	hl,isr_al
	ld	(isr_ix),hl	;Eintragen der ISR-Adresse
	ld	a,0d5h		;Counter & L-H-Flanke
	out	(CTC+3),a
	ld	a,2
	out	(CTC+3),a
	halt			;Warten auf Interrupt nach zwei Impulsen
; --- warten --------------------
 
; Ausschalten der Schnittstelle nach dem Spurloeschen
	ld	a,(st_ina)
	out	(DSKC2),a	;alles inaktiv
	out	(IMPAUS),a	;Disk_Schnittstelle aus
	ld	a,3
	out	(CTC+3),a	;CTC/3-Reset
; Ende, wenn "Spurloeschen"
	ld	a,(cmd_br-8)
	cp	84h
	ret	z
; Sektoraufbau
	ld	hl,kf_top	;Beginn der Praeambel
	ld	b,18
id1	ld	(hl),0ffh
	inc	hl
	djnz	id1
	ld	b,3
id2	ld	(hl),0a1h	;Vorbytes
	inc	hl
	djnz	id2
	ld	(hl),0feh	;Identmarke
	inc	hl
	ex	de,hl
	ld	bc,3
	ld	hl,ident+1
	ldir			;Cylinder & Head
	ex	de,hl
	ld	(hl),1		;Beginn mit Sektornummer 1
	inc	hl
	ld	b,12
id3	ld	(hl),0ffh	;2 Byte CRC und 10 Byte Luecke
	inc	hl
	djnz	id3
 
;---------------------------------------------------
; Vorbereiten der Disk-Schnittstelle zum Formatieren
;---------------------------------------------------
; Bestimmung Disk-Adresszaehler
	ld	hl,3600h
	call	b_daz
	ld	(dsk_az),hl
	ld	(hl),a		;Laden	des Zaehlers
; Markenaufbau
	ld	a,4		;MFM & PRAEOFF
	ld	d,a
	ld	a,(cmd_br-7)	;akt. Laufwerk
	call	cy_pos
	ld	b,(hl)		;Low-Teil Zylinderposition
	inc	hl
	ld	h,(hl)		;High-Teil Zylinderposition
	ld	l,b
	ld	bc,(pre_cy)	;Praekompensationsbeginn
	xor	a
	sbc	hl,bc
	jr	c,pr_ok
	ld	a,0ch		;MFM & PRAEON
	ld	d,a
pr_ok	ld	a,d
	out	(BM_D),a
	ld	a,0ah		;Taktbits der Marken
	out	(BM_T),a
; Einstellen des RAM-Bereichs
	ld	a,(cnt_st)
	set	6,a		;RAM12 = 1
	ld	(cnt_st),a
	out	(CNTST),a
; Eintragen der ISR-Adressen
	ld	hl,isr_al
	ld	(isr_ix),hl	;ISR-Adresse
; DEND-Adresse festlegen
	ld	a,0ffh
	out	(DSKEA),a
; Bereitstellung der Sektorreihenfolge in 'fs_tab'
	ld	hl,fs_tab	;RAM-Tabelle
	ld	a,(ident+3)	;Kopfnummer
	ld	b,0
	ld	c,a
	add	hl,bc		;Beginn der Sektornummern fuer akt. Kopf
	ex	de,hl		;DE: Position der Sektornummer 1
	ld	b,a		;Kopfnummer
	ld	a,(secs)	;Anzahl der Sektoren
	sub	b		;Anzahl der umzuladenden Sektoren
	ld	b,0
	ld	c,a		;BC: Anzahl
	ld	hl,sc_tab	;Reihenfolge fuer Kopf 0
	ldir
	ld	a,(ident+3)	;Kopfnummer
	cp	0
	jr	z,fs_ta1	;kein weiteres Umladen
	ld	c,a
	ld	b,0
	ld	de,fs_tab	;Beginn der RAM-Tabelle
	ldir			;Umladen des Rests
; Sektornummer initialisieren
fs_ta1	ld	hl,fs_tab	;Beginn der Sektortabelle
	ld	a,(hl)
	ex	de,hl
	ld	hl,kf_top+19h	;Speicherplatz der Sektornummer
	ld	(hl),a
	ld	a,(secs)
	ld	b,a		;Sektorenzaehler

;-----------------------------------------------------------------
; Formatieren einer Spur (Schreiben der Kennfelder)
;-----------------------------------------------------------------
; CTC-Programmierung fuer Indexerkennung vor dem Formatieren
	ld	a,0d5h		;Counter
	out	(CTC+3),a
	ld	a,1
	out	(CTC+3),a
	halt			;Warten auf den naechsten Index
; --- warten --- warten --- warten --- warten --- warten
 
f_nsec
	ld	a,3
	out	(CTC+3),a	;Reset IX-Interrupt
	ld	a,(st_wa)	;alles aktiv
	ld	c,a
	res	3,a		;'WG=inaktiv'
	out	(DSKC2),a	;Schnittstelle an (ohne WG)
	ld	a,c
	out	(DSKC2),a	;Schnittstelle an (mit WG=aktiv)
	ld	a,12h
	out	(DSKEA),a	;DEND zum Markeneinblenden und -anzahl
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	ld	a,(st_wc)
	out	(DSKC2),a	;CRC-Einblenden
	ld	a,1ah
	out	(DSKEA),a	;DEND zum CRC-Einblenden
	nop
	nop
	nop
	nop
	nop
	ld	a,(st_ina)
	out	(DSKC2),a	;Schnittstelle aus
	out	(IMPAUS),a
	ld	hl,(dsk_az)
	ld	(hl),a		;Diskadresszaehler neu laden
	ld	a,87h		;Timer & VT16
	out	(CTC+3),a
	ld	a,(ztk)		;berechnete Zeitkonstante
	out	(CTC+3),a
	inc	de		;Tabelle der Reihenfolge der Sektoren
	ld	a,(de)		;Sektornummer
	ld	hl,kf_top+19h	;Adresse der Sektornummer
	ld	(hl),a
	dec	b
	jp	z,ft_end
	halt			;Warten auf Zeitinterrupt
;---  Warten -----
	jp	f_nsec
ft_end
	ld	a,3
	out	(CTC+3),a	;Reset CTC/3
	xor	a

;-----------------------------------------------------
; Initialisieren der Datenfelder
;-----------------------------------------------------
	ld	hl,ram
	ld	de,ram+1
	ld	bc,0fffh
	ld	(hl),0e5h
	ldir

;----------------------------------------------------
; Schreiben der Datenfelder
;----------------------------------------------------
; Setzen der Fehlerzaehler fuer das Lesen der Kennfelder
	ld	de,c_zt
	ld	hl,fe_n
	ld	bc,7
	ldir
; Schreiben der Datenfelder der Sektoren
	ld	a,4
	ld	(ct_ra),a	;Zaehler fuer Schreibwiederholungen
	ld	a,1		;Beginn mit Sektor 1
	ld	(ident+4),a
	ld	hl,2400h	;18 Sektoren
	ld	a,(secs)
	cp	18
	jr	z,wrt_d1
	ld	hl,2200h	;17 Sektoren
wrt_d1	ld	(dt_len),hl
	call	wrt_dt		;Schreiben der Datenfelder
	ret			;Schreibfehler
 eject
;***********************************************************************
; Datentransfer zwischen Host und WDC-RAM
;***********************************************************************
ram_tr
	ld	hl,cmd_br-7	;RAM-Anfangsadresse
	ld	a,(hl)
	ld	e,a
	inc	hl
	ld	a,(hl)
	ld	d,a		;(DE): Anfangsadresse
	ld	hl,cmd_br-2	;Laenge
	ld	a,(hl)
	ld	c,a
	inc	hl
	ld	a,(hl)
	ld	b,a		;(BC): Laenge
	ex	de,hl
	add	hl,bc
	dec	hl
	ld	a,(cmd_br-8)	;Kommandocode
	cp	08h		;Lesen WDC-RAM ?
	jp	z,rd_ram
	cp	18h		;Schreiben WDC-RAM ?
	jp	z,wt_ram
	cp	28h		;Lesen WDC-Parameterblock ?
	jp	z,rd_pb
	cp	78h		;Schreiben WDC-Parameterblock ?
	jp	z,wr_pb
	cp	38h		;Lesen WDC-Fehlerstatistik ?
	jp	z,rd_fs
	cp	58h		;Lesen WDC-RAM-BTT (auf den Host-RAM) ?
	jp	z,rd_def
	cp	68h		;Schreiben WDC-RAM-BTT (aus Host-RAM) ?
	jp	z,wr_def
	cp	48h		;Loeschen BTT im WC-RAM ?
	jp	nz,err_1
;-----------------------------------------------------
; Loeschen Defektspurtabelle
;-----------------------------------------------------
	call	def_ad
	call	btt_cl
	jp	begin
;-----------------------------------------------------
; Lesen der BTT aus dem WDC-RAM in den Host-RAM
;-----------------------------------------------------
rd_def
	call	def_ad
	ld	hl,(df_end)
	push	bc
	call	t_btt1
	pop	bc
	jp	nz,err_18
	ld	hl,(df_end)
	add	hl,bc
	dec	hl
	ld	a,3
	call	p_wc_h
	jp	begin
;-----------------------------------------------------
; Schreiben der BTT im WDC-RAM aus dem Host-RAM
;-----------------------------------------------------
wr_def
	call	def_ad
	ld	hl,(df_end)
	add	hl,bc
	dec	hl
	ld	a,2
	call	p_h_wc
	jp	begin
;-----------------------------------------------------
; Lesen WDC-RAM
;-----------------------------------------------------
rd_ram
	ld	a,3
	call	p_wc_h
	jp	begin
;-----------------------------------------------------
; Schreiben WDC-RAM
;-----------------------------------------------------
wt_ram
	ld	a,2
	call	p_h_wc
	jp	begin
;-----------------------------------------------------
; Schreiben WDC-Parameterblock
;-----------------------------------------------------
wr_pb
	ld	hl,p_blk
	add	hl,bc
	dec	hl
	ld	a,2
	call	p_h_wc
	jp	begin
;-----------------------------------------------------
; Lesen WDC-Parameterblock
;-----------------------------------------------------
rd_pb
; Eintragen der Parameter
	push	bc
	ld	hl,string	;Zeichenkette zur Identifizierung
	ld	de,p_str
	ld	bc,7
	ldir
; Umladen der Anzahl der Bloecke
	ld	hl,bl_dr0
	ld	de,zmx_41
	inc	de
	ld	bc,4
	ldir
; Umladen 'd_rdy', s_rdy, 'err_in', Anzahl Laufwerke
	ld	a,(d_rdy)
	ld	(de),a
	inc	de
	ld	a,(s_rdy)
	ld	(de),a
	inc	de
	ld	a,(err_in)
	ld	(de),a
	inc	de
	ld	a,(d_rdy)
	ld	b,3
	ld	c,0
dr_t0	rrc	a
	jr	nc,dr_t1
	inc	c
dr_t1	djnz	dr_t0		;Anzahl der angeschlossenen Laufwerke
	ld	a,c
	ld	(de),a
; Ausgabe der Parameter
	pop	bc
	ld	hl,p_str
	add	hl,bc
	dec	hl
	ld	a,3
	call	p_wc_h
	jp	begin
;-----------------------------------------------------
; Lesen WDC-Fehlerstatistik
;-----------------------------------------------------
rd_fs	ld	hl,sum_zt
	add	hl,bc
	dec	hl
	ld	a,3
	call	p_wc_h
	jp	begin
 eject
;---------------------------------------------------------------
; PROGRAMMIERUNG DER HOST-SCHNITTSTELLE
;---------------------------------------------------------------
; A: Status des WDC
; BC: Laenge der Ausgabe
; HL: Endadresse des Ausgabebereichs
;---------------------------------------------------------------
p_wc_h				;WDC-RAM --> Host
; bei 'WDC-->Host' muss ein Byte mehr uebertragen werden
; (wird vom Host nicht abgeholt, nur zur Ready-Abschaltung PIO)
	push	af
p_sl1	ld	a,(hst_by)	;Besetzt-Anzeiger
	bit	7,a
	jr	nz,p_sl1	;Warten in Schleife, wenn Treiber besetzt
	set	7,a		;Treiber besetzt
	ld	(hst_by),a
	inc	hl
	inc	bc
	ld	a,(cnt_st)
	set	5,a		;WDC-RAM --> Host
	ld	(cnt_st),a
	pop	af
	jr	p_h_1
;------------------------------------------------------------
p_h_wc				;Host --> WDC-RAM
	push	af
p_sl2	ld	a,(hst_by)	;Besetzt-Anzeiger
	bit	7,a
	jr	nz,p_sl2	;Warten in Schleife, wenn Treiber besetzt
	set	7,a		;Treiber besetzt
	ld	(hst_by),a
	ld	a,(cnt_st)
	res	5,a		;WDC-RAM <-- Host
	ld	(cnt_st),a
	pop	af
;------------------------------------------------------------
p_h_1	push	hl		;Endadresse retten
	push	bc		;Laenge retten
	push	af		;Status retten
; Wert des Adresszaehlers berechnen
	xor	a
	sbc	hl,bc
	inc	hl		;Beginn des E/A-Bereiches
	pop	af
	push	hl		;Beginn retten
	push	af
	res	5,h		;Kennzeichnung RAM ruecksetzen (Bit 13)
; Ausgabebyte fuer CNTST aktualisieren
	ld	a,(cnt_st)	;Programmierbyte
	res	4,a		;HA12 = 0
	bit	4,h		;Adressbit 12 gesetzt ?
	jr	z,hrt1
	set	4,a		;HA12 = 1
hrt1	ld	(cnt_st),a
	pop	af
	push	af
	ld	b,a
	ld	a,(cnt_st)
	set	3,a		;HEN ein
	or	b		;Status einfuegen
	ld	(cnt_st),a	;Ausgabebyte
; Host-Adresszaehler-Kennzeichnung einfuegen
	ld	a,h
	and	0ch		;Bit 11,10 der Adresse
	rlca
	rlca
	rlca
	rlca			;Bit 15,14
	ld	b,a
	ld	a,h
	and	03h		;Bit 9,8 der Adresse
	or	b		;Bit 15,14,9,8
	or	038h		;Kennzeichnung einfuegen
	ld	h,a		;Host-Adresszaehler fertig
	pop	af
	pop	de		;Beginn des Bereichs
	pop	bc		;Laenge des Bereichs
; Host-Adresszaehler laden
	ld	(hl),a
; Bestimmung der Schaltflanke fuer den CTC-Kanal
	pop	hl		;Endadresse des E/A-Bereiches
	ld	a,0c7h		;Uebergang '1...0'
	bit	0,l		;gerade oder ungerade
	jr	nz,hrt11
	ld	a,0d7h		;Uebergang '0...1'
hrt11	ld	(ctc_st),a
; Bestimmung der CTC-Clocks
	inc	bc
	xor	a
	rr	b
	rr	c		;Laenge/2
	ld	hl,100h
	xor	a
	sbc	hl,bc
	jr	c,gr_100

; CTC-Clocks kleiner/gleich 100h
hrt17	ld	a,(ctc_st)	;CTC-Steuerwort
	out	(CTC+2),a
	ld	a,c		;Anzahl der Impulse
	out	(CTC+2),a
	ld	hl,cnt_st
	ld	a,(hl)
	res	3,(hl)		;'HEN=inaktiv'
	out	(CNTST),a	;Ansteuerung aktiv
	ld	a,(hl)
	out	(CNTST),a	;Vorbereitung von 'Schnittstelle inaktiv'
	ret
 
; CTC-Clocks groesser 100h
gr_100	xor	a
	cp	c		;C = 0 ?
	jr	nz,gr_101
	dec	b
gr_101	ld	a,b
	ld	(hst_bl),a	;Anzahl der Bloecke auf der HS
	ld	hl,isr_h1	;ISR-Adresse
	ld	(isr_hs),hl
	ld	a,(ctc_st)	;CTC-Steuerwort
	out	(CTC+2),a
	ld	a,c
	out	(CTC+2),a
	ld	a,(ctc_st)	;CTC-Steuerwort
	res	1,a		;kein Reset
	out	(CTC+2),a
	ld	a,0
	out	(CTC+2),a	;256 Impulse
	ld	a,(cnt_st)
	out	(CNTST),a
	ret
 
;---------------------------------------------------------------
; Interruptroutinen fuer den Host-Adresszaehler
;---------------------------------------------------------------
; ISR fuer Zwischeninterrupts bei langen Uebertragungen
isr_h1	push	af
	push	hl
	ld	hl,hst_bl	;Zaehler fuer HS-Bloecke
	dec	(hl)
	jr	nz,isr_h2	;Sprung, wenn Zaehler nicht abgelaufen
	ld	hl,isr_h0	;ISR fuer letzen Interrupt
	ld	(isr_hs),hl
	ld	a,(cnt_st)
	res	3,a		;'HEN=inaktiv'
	ld	(cnt_st),a
	out	(CNTST),a
isr_h2	pop	hl
	pop	af
	ei
	reti
; ISR fuer letzten Interrupt waehrend eines Transfers
isr_h0	push	af
	ld	a,(cnt_st)
	and	0d0h		;'TR=inaktiv' & 'HEN=inaktiv' & Status=0
	ld	(cnt_st),a
	out	(CNTST),a
	ld	a,3
	out	(CTC+2),a
	ld	a,(hst_by)
	res	7,a		;Treiber frei
	ld	(hst_by),a
	pop	af
	ei
	reti
 eject
;---------------------------------------------------------------------
; Fehlerauswertung und Uebergabe an den Host
;---------------------------------------------------------------------
err_1	ld	a,1
	jr	error1
err_2	ld	a,2
	jr	error1
err_3	ld	a,3
	jr	error1
err_4	ld	a,4
	jr	error1
err_5	ld	a,5
	jr	error1
err_6	ld	a,6
	jr	error1
err_7	ld	a,7
	jr	error1
err_15	ld	a,15h
	jr	error1
err_16	ld	a,16h
	jr	error1
err_17	ld	a,17h
	jr	error1
err_18	ld	a,18h
	jr	error1

error1	ld	(err_cd),a
; Programmierung der Host-Schnittstelle
error	ld	a,(err_cd)
	ld	(err_br),a
	ld	bc,1		;Ausgabelaenge
	ld	hl,err_br	;auszugebendes Byte
	ld	a,7		;Status '7'
	call	p_wc_h		;Programmierung der Host-Schnittstelle
	xor	a
	ld	(err_cd),a	;Loeschen des Fehlercodes
	jp	begin
 
;---  b_daz  ---------------------------------------------
; Berechnung des Disk-Adresszaehlers
; in : HL Adresse
; out: HL Disk-Adresszaehler
;---------------------------------------------------------
b_daz	res	5,h		;Kennzeichnung RAM ruecksetzen
	ld	a,h
	and	0ch		;Bit 11,10 der Adresse
	rlca
	rlca
	rlca
	rlca
	ld	b,a
	ld	a,h
	and	03h		;Bit 9,8 der Adresse
	or	b		;Bit 15,14,9,8 des Disk-Adresszaehlers
	or	03ch		;Kennzeichnung einfuegen
	ld	h,a		;Adresszaehler fertig
	ret
 
;---  time  --------------------------------------
; ca. 0,01 ms pro Durchlauf, 4MHz-CPU, Wait-Zyklen
; DE: Zaehlwert
;-------------------------------------------------
time	ld	a,1
	ld	a,1
	dec	de
	bit	7,d
	jr	z,time
	ret

;---  time1  ----------------------------------
; ca. 1 ms pro Durchlauf, 4MHz-CPU, Wait-Zyklen
; DE: Zaehlwert
;----------------------------------------------
time1	push	de
	ld	de,130
time10	dec	de
	bit	7,d
	jr	z,time10
	pop	de
	dec	de
	ld	a,d
	or	e
	jr	nz,time1
	ret

;---  time2  -----------------------------------
; ca. 10 ms pro Durchlauf, 4MHz-CPU, Wait-Zyklen
; DE: Zaehlwert
;-----------------------------------------------
time2	push	de
	ld	de,1288
time20	dec	de
	bit	7,d
	jr	z,time20
	pop	de
	dec	de
	ld	a,d
	or	e
	jr	nz,time2
	ret
 
;---  crc_br  -----------------------------------
; CRC-Berechnung
;------------------------------------------------
crc_br	ld	de,-1
crc1	ld	a,(hl)
	xor	d
	ld	d,a
	rrca
	rrca
	rrca
	rrca
	and	0fh
	xor	d
	ld	d,a
	rrca
	rrca
	rrca
	push	af
	and	1fh
	xor	e
	ld	e,a
	pop	af
	push	af
	rrca
	and	0f0h
	xor	e
	ld	e,a
	pop	af
	and	0e0h
	xor	d
	ld	d,e
	ld	e,a
	inc	hl
	dec	bc
	ld	a,b
	or	c
	jr	nz,crc1
	ret
 
;---------------- PROGRAMMTEXT-ENDE ---------------------------------------
 
	begin	1
; Tabelle der Kommandocodes
com_tb	db	021h	;HD-Lesen von einer Blocknummer
	db	022h	;HD-Schreiben auf eine Blocknummer
	db	001h	;HD-Lesen von einer Sektoradresse
	db	081h	;HD-Testlesen von einer Sektoradresse
	db	011h	;HD-Lesen von einer Sektoradresse mit BTT-Berueck.
	db	002h	;HD-Schreiben auf eine Sektoradresse
	db	082h	;HD-Schreiben auf eine Sektoradresse mit Ruecklesen
	db	012h	;HD-Schreiben auf eine Sektoradresse mit BTT-Berue.
	db	092h	;HD-Schreiben auf eine Sektoradresse mit BTT-
			;Beruecksichtigung und Ruecklesen
	db	0a1h	;HD-Kopieren von einer Blocknummer des LW 1 auf LW 0
	db	0a2h	;HD-Schreiben auf eine Blocknummer mit Ruecklesen
	db	0c2h	;HD-Schreiben von PAR und BTT auf das Laufwerk
	db	004h	;HD-Formatieren einer Spur
	db	014h	;HD-Formatieren einer Spur mit Ruecklesen
	db	024h	;HD-Formatieren einer Spur mit Defektspurfeststellung
	db	044h	;HD-Ruecklesen einer Spur (Verify)
	db	084h	;HD-Loeschen einer Spur
	db	008h	;Lesen WDC-RAM
	db	018h	;Schreiben WDC-RAM
	db	028h	;Lesen des WDC-Parameterblocks
	db	038h	;Lesen der WDC-Fehlerstatistik
	db	048h	;Loeschen der BTT im WDC-RAM
	db	058h	;Lesen der BTT aus dem WDC-RAM in den Host-RAM
	db	068h	;Schreiben der BTT aus dem Host-RAM in den WDC-RAM
	db	078h	;Schriben des WDC-Parameterblocks
	db	0ffh	;Endekennzeichen
; Praeambel beim Schreiben eines Datenfeldes (13 Byte)
prae	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0a1h
	db	0fbh
; Postambel beim Schreiben eines Datenfeldes (12 Byte)
post	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
	db	0ffh
; Kennfeld der Defektspurtabelle
id_df	db	0feh
	db	00h
	db	00h
	db	00h
	db	01h
; Kennung der Parametertabelle
txt_pm	text	'PARMTR'
; Kennung der Defektspurtabelle
txt_df	text	'DEFEKT'
; Fehlerzaehler fuer normales Lesen
fe_n	db	4
	db	8
	db	4
	db	4
	db	4
	db	4
	db	4
; Fehlerzaehler fuer das Formatieren
fe_f1	db	2
	db	2
	db	2
	db	2
	db	2
	db	2
	db	2
; Fehlerzaehler fuer das Testlesen
fe_t	db	1
	db	1
	db	1
	db	1
	db	1
	db	1
	db	1
; Tabelle des Sektorversatz
	db	0ffh
sc_tab	db	1
	db	10
	db	2
	db	11
	db	3
	db	12
	db	4
	db	13
	db	5
	db	14
	db	6
	db	15
	db	7
	db	16
	db	8
	db	17
	db	9
	db	18
	db	0ffh
 
	end

