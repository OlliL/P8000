import socket,sys,time,threading,os
port=int(sys.argv[1]); logf=sys.argv[2]; fifo=sys.argv[3]
s=socket.socket(); s.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1); s.bind(('127.0.0.1',port)); s.listen(1)
c,_=s.accept(); c.settimeout(0.2)
log=open(logf,'wb',buffering=0)
def rx():
    while True:
        try:
            d=c.recv(4096)
            if not d: break
            log.write(d)
        except socket.timeout: pass
        except OSError: break
threading.Thread(target=rx,daemon=True).start()
def even(ch): return ch|0x80 if bin(ch).count('1')%2 else ch
while True:
    with open(fifo) as f:
        for line in f:
            line=line.rstrip('\n')
            if line=='@quit': os._exit(0)
            par=False
            if line.startswith('P:'): par=True; line=line[2:]
            for ch in line.encode().decode('unicode_escape').encode('latin1'):
                c.send(bytes([even(ch) if par else ch])); time.sleep(0.05)
