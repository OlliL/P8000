
fp		:= r15;
sp		:= r15;
.comm	512,	__sibuf;
.comm	512,	__sobuf;
.even
__iob::
.addr	__sibuf
.word	%0
.addr	__sibuf
.byte	%1
.byte	%0
.word	%0
.word	%0
.word	%0
.byte	%2
.byte	%1
.word	%0
.word	%0
.word	%0
.byte	%6
.byte	%2
.=.+136
.even
__lastbuf::
.addr	__iob+160
