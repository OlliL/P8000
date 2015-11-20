# WEGA

Hier sind die WEGA Kernel-Quellen zu finden. Diese sind teilweise aus zur Verfügung stehenden Original-Quellen von WEGA entstanden, und teilweise durch Disassemblierung der Original-Objects und "Rückwärtsübersetzung" in C-Code unter Zuhilfnahme der im Netz verfügbaren Quellen von V7, SYSIII und Plexis.

In seinem aktuellen Zustand kann der Kernel vollständig compiliert werden und ist von seiner Lauffähigkeit her kompatibel zu dem original ausgelieferten WEGA-Kernel.

Folgende Objekte haben Abweichungen zum Originalkernel und werden deshalb entweder direkt als Object verwendet, oder abweichend zum Originalkernel aus Assembler-Code erzeugt da keine Rueckuebersetzung in die Originalquelle moeglich war.

## ``sys/u.s``

Das Array "u" (wird für den user-Struct verwendet) muss auf der Adresse 0xf600 im Segment 62 (0x3e) liegen. Leider ist es mir aktuell nicht möglich dies so zu vereinbaren. Die Assemblerdirektive $ABS kann nur im Segment 1 arbeiten, und ich habe noch nicht rausgefunden, wie ich den Assembler überreden kann, sich auf das Segment 62 absolut zu beziehen.

## ``sys/lock.c``

Das Object wurde disassembliert und steht zummindest als Assembler-Quelle zur Verfuegung welche ein mit dem Originalkernel 100% identisches Objekt erzeugt.

Stand der Migration nach C:
```
0000 T _lkdata    not existing
0282 T _unlk      not existing
0392 T _locked    not existing
044a T _deadlock  not existing
04b6 T _unlock    not existing
0546 T _lockalloc not existing
05b8 T _lockfree  not existing
05f2 T _lockadd   not existing
0664 T _lkfront   not existing
```