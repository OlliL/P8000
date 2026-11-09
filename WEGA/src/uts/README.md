# WEGA

Hier sind die WEGA Kernel-Quellen zu finden. Diese sind teilweise aus zur Verfügung stehenden Original-Quellen von WEGA entstanden, und teilweise durch Disassemblierung der Original-Objects und "Rückwärtsübersetzung" in C-Code unter Zuhilfnahme der im Netz verfügbaren Quellen von V7, SYSIII und Plexis.

In seinem aktuellen Zustand kann der Kernel vollständig compiliert werden und ist von seiner Lauffähigkeit her kompatibel zu dem original ausgelieferten WEGA-Kernel.

Folgende Objekte haben Abweichungen zum Originalkernel und werden deshalb entweder direkt als Object verwendet, oder abweichend zum Originalkernel aus Assembler-Code erzeugt da keine Rueckuebersetzung in die Originalquelle moeglich war.

## ``conf/u.s``

Das Array "u" (wird für den user-Struct verwendet) muss auf der Adresse 0xf600 im Segment 62 (0x3e) liegen. Der PLZ/ASM-Assembler `as` kann einem absoluten Symbol mit `$ABS` nur den Offset geben, als Segment trägt er die Nummer der aktuellen Sektion ein. `as` erzeugt aus `u.s` daher alle Bytes des Original-Objects bis auf die zwei, die das Segment von `_u` enthalten; diese setzt `conf/mku.sh` nach dem Assemblieren.
