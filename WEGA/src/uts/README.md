# WEGA

Hier sind die WEGA Kernel-Quellen zu finden. Diese sind teilweise aus zur Verfügung stehenden Original-Quellen von WEGA entstanden, und teilweise durch Disassemblierung der Original-Objects und "Rückwärtsübersetzung" in C-Code unter Zuhilfnahme der im Netz verfügbaren Quellen von V7, SYSIII und Plexis.

In seinem aktuellen Zustand kann der Kernel vollständig compiliert werden und ist von seiner Lauffähigkeit her kompatibel zu dem original ausgelieferten WEGA-Kernel.

Folgende Objekte haben Abweichungen zum Originalkernel und werden deshalb entweder direkt als Object verwendet, oder abweichend zum Originalkernel aus Assembler-Code erzeugt da keine Rueckuebersetzung in die Originalquelle moeglich war.

## ``sys/u.s``

Das Array "u" (wird für den user-Struct verwendet) muss auf der Adresse 0xf600 im Segment 62 (0x3e) liegen. Leider ist es mir aktuell nicht möglich dies so zu vereinbaren. Die Assemblerdirektive $ABS kann nur im Segment 1 arbeiten, und ich habe noch nicht rausgefunden, wie ich den Assembler überreden kann, sich auf das Segment 62 absolut zu beziehen.
