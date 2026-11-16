# `compare/` – alles, was wir über ein Programm wissen, außer seinem Quelltext

*(English version: [README.md](README.md))*

Das WEGA-Projekt baut die Programme des Systems WEGA 3.1 (das UNIX des DDR-Rechners P8000, einer Zilog-Z8000-Maschine)
aus Quelltext nach, und zwar so, dass die nachgebauten Programme **Byte für Byte identisch** mit den Originalen sind. Die
Quellen liegen in `WEGA/src/`. Dieses Verzeichnis, `WEGA/compare/`, enthält alles andere, was zu einem Programm gehört:
wie wir es gegen das Original testen, was wir bei der Rekonstruktion herausgefunden haben und wie sein Quelltext mit
UNIX System III zusammenhängt.

`src/` muss ein sauberer Quellbaum bleiben, den man auf eine echte WEGA-Maschine kopieren und dort bauen könnte. Deshalb
liegt nichts von dem Material hier neben den Quellen.

Diese Datei erklärt, was man hier findet, und am ausführlichsten, wie der **Korpus** funktioniert und wie man selbst
einen schreibt.

---

## Inhalt

1. [Was in diesem Verzeichnis liegt](#1-was-in-diesem-verzeichnis-liegt)
2. [Befunde: `findings.json`, `FINDINGS.md`, `SYSIII.md`](#2-befunde-findingsjson-findingsmd-sysiiimd)
3. [`progcfg.py`: Einstellungen der Rekonstruktionswerkzeuge](#3-progcfgpy-einstellungen-der-rekonstruktionswerkzeuge)
4. [Der Korpus: die Idee](#4-der-korpus-die-idee)
5. [Einen Korpus laufen lassen](#5-einen-korpus-laufen-lassen)
6. [Wie ein Fall abläuft, Schritt für Schritt](#6-wie-ein-fall-abläuft-schritt-für-schritt)
7. [Drei Arten, Fälle zu schreiben](#7-drei-arten-fälle-zu-schreiben)
8. [Referenz: die Falldatei](#8-referenz-die-falldatei)
9. [Referenz: `corpus.py`](#9-referenz-corpuspy)
10. [Anleitung: ein Korpus von Grund auf](#10-anleitung-ein-korpus-von-grund-auf)
11. [Gute Fälle schreiben](#11-gute-fälle-schreiben)
12. [Was der Emulator kann und was nicht](#12-was-der-emulator-kann-und-was-nicht)
13. [Fehlersuche](#13-fehlersuche)
14. [Begriffe](#14-begriffe)

---

## 1. Was in diesem Verzeichnis liegt

Es gibt ein Unterverzeichnis pro Programm (oder pro Gruppe von Programmen, die sich einen Korpus teilen). Der Name ist
der Name des Programms: `compare/tar/` gehört zu `/bin/tar`, `compare/cpp/` zu `/lib/cpp`. Auch ein Programm, das in
`src/cmd` nur aus einer Quelldatei besteht (wie `rm.c`), hat hier ein eigenes Verzeichnis.

| Datei oder Verzeichnis | Was es ist | Geschrieben von |
|---|---|---|
| `corpus.py` | Die Definition des Korpus des Programms: welches Original, welches nachgebaute Binary, welche Fälle. | Hand |
| `tests.txt` | Fälle als Blöcke einer einzigen Textdatei (dc, rm, rmdir, mv, tar). | Hand |
| `cases/FALL/` | Fälle als je ein Verzeichnis: eine Datei `case` und die Startdateien in `in/` (ar). | Hand |
| `fixtures.py`, `tree.py`, `test/`, `link/`, `cmdlines.txt` | Material, aus dem ein Korpus seine Fälle macht: Testprogramme, Objektdateien, Kommandozeilen. | Hand |
| `findings.json` | Pro Funktion des Programms: wo sie im Original liegt und ob unsere übersetzte Funktion identisch ist. | Werkzeuge + Hand |
| `FINDINGS.md` | Ein lesbarer Bericht, erzeugt aus `findings.json`. | erzeugt |
| `SYSIII.md` | Wie der WEGA-Quelltext mit dem System-III-Quelltext zusammenhängt, aus dem er entstand. | Hand |
| `progcfg.py` | Einstellungen der Rekonstruktionswerkzeuge in `host/recon/` (Adressen, Module, Compiler). | Hand |
| `NOTES.md`, `HANDOFF.md`, `MODULES`, `ADDRESSES`, `DATA` | Arbeitsnotizen einer langen Rekonstruktion (popt, cas). | Hand |

Zwei Dateien gehören zu keinem einzelnen Programm:

- **`CORPORA`** – die Liste der Programme, deren Korpus `checkall` ausführt. Ein neuer Korpus kommt durch eine Zeile
  hier dazu; nichts wird automatisch gefunden.
- **`binutils/tree.py`** – ein gemeinsamer Helfer der Korpora von `size`, `strip` und `ranlib` (er bereitet einen Baum
  aus Objektdateien und Archiven vor, den alle drei benutzen).

Der heutige Stand (die Zahl der Fälle ändert sich, wenn sich Quellen ändern):

| Programm(e) | Korpus besteht aus | Kandidaten |
|---|---|---|
| `ar` | handgeschriebenen `cases/` | wega, host |
| `size`, `strip`, `ranlib` | erzeugt aus `binutils/tree.py` | wega, host |
| `cc`, `scc` | Kommandozeilen (`cc/cmdlines.txt`) | wega, host |
| `cpp`, `cparse`, `scparse`, `codgen`, `scodgen`, `popt`, `spopt` | erzeugt: jede C-Quelle von `src/cmd` und `src/lib` | wega, host |
| `cchain`, `scchain` | erzeugt: der ganze Compilertreiber auf jeder C-Quelle, dazu Links aus `cchain/link/` | host |
| `ld`, `sld`, `gld` | erzeugt aus `fixtures.py` und `test/` | wega, host (gld: host) |
| `cas`, `casold` | erzeugt: C-Quellen, Opcode-Tabellen, Sprungtests | wega, host (casold: host) |
| `dc`, `rm`, `rmdir`, `mv` (mit `cp`, `ln`), `tar` | `tests.txt` | wega |

---

## 2. Befunde: `findings.json`, `FINDINGS.md`, `SYSIII.md`

Ein Programm zu rekonstruieren heißt, C-Quelltext zu schreiben, den der *alte* Compiler seiner Zeit in genau den
Original-Maschinencode übersetzt. Solange diese Arbeit läuft, muss man Funktion für Funktion wissen, wie weit man ist.

**`findings.json`** hat einen Eintrag pro Funktion des Programms:

```json
"iputb": { "file": "ifile.c", "orig": "0x0042", "status": "exact" }
```

- `file` – die Quelldatei, in der die Funktion steht,
- `orig` – ihre Adresse im Original-Binary,
- `status` – `exact` (unsere übersetzte Funktion ist identisch, Sprung- und Aufrufziele eingeschlossen), `masked`
  (identisch bis auf einen bekannten Unterschied des alten Assemblers, `call` gegen `calr`) oder `open` (noch
  verschieden),
- `note` – nur wenn es etwas Besonderes zu wissen gibt: ein geratener Name, toter Code, eine ungewöhnliche Quellform.

Der Kopf der Datei sagt, mit welchem Compiler und welchem Vergleich gemessen wurde und bei welchem Commit. Den Status
schreibt `host/recon/findings.py`; die Notizen werden von Hand geschrieben.

**`FINDINGS.md`** wird aus `findings.json` erzeugt (`host/recon/findings.py NAME`). Nie von Hand ändern.

**`SYSIII.md`** vergleicht den WEGA-Quelltext mit dem System-III-Quelltext, von dem er abstammt (und mit den
Plexus-Quellen, einem verwandten Z8000-UNIX): wie viele Zeilen gleich sind, was WEGA hinzugefügt hat und wo der
Quelltext eine ungewöhnliche Form annehmen musste, damit die Original-Bytes herauskommen.

Fehler, die in einem Originalprogramm gefunden werden, stehen *nicht* hier. Sie kommen in eine zentrale Tabelle,
„Bugs and quirks of the originals", in `host/KNOWLEDGE.md`.

---

## 3. `progcfg.py`: Einstellungen der Rekonstruktionswerkzeuge

Die Werkzeuge in `host/recon/` (Disassemblieren, Vergleich Funktion für Funktion, Modelle der alten Compiler) nehmen ein
Programm beim Namen und lesen seine `progcfg.py`: den Pfad des Originals, wo sein eigener Code endet und die Bibliothek
beginnt, die Liste der Module in Linkreihenfolge, welcher Compiler es gebaut hat. `host/recon/README.md` erklärt die
Felder. Der Korpus benutzt diese Datei **nicht**; er hat seine eigene `corpus.py`.

---

## 4. Der Korpus: die Idee

### Wozu überhaupt ein Korpus?

Die Byte-Identität der Binaries beweist `make -C WEGA/host checkall`: Alles wird aus `src/` gebaut, und jede installierte
Datei wird mit dem Original verglichen. Warum dann auch noch das Verhalten testen?

Weil die Byte-Identität nur für den Build mit dem **alten Compiler** gilt, mit dem das Original gemacht wurde. Dieselben
Quellen werden ein zweites Mal gebaut, mit dem **ausgelieferten** Compiler von WEGA (`cc`, `scc`) und der ausgelieferten
C-Bibliothek – der „shipped build" (`make shipped` in `src/cmd`). Dieser Build ist nicht byte-identisch (anderer
Compiler, andere Bibliothek), deshalb prüft er etwas anderes: dass der Quelltext *richtig* ist und nicht nur zufällig mit
einem bestimmten Compiler die richtigen Bytes ergibt. Für die C-Compilerkette gibt es außerdem **Host**-Versionen
(Portierungen, die direkt unter Linux laufen); auch diese müssen sich wie die Originale verhalten.

Der Korpus beantwortet eine Frage: **Tut das nachgebaute Programm genau das, was das Original tut, und zwar bei vielen
Eingaben?**

### Das Orakel

Die Originale sind Z8000-Programme. Sie laufen unter Linux im Emulator `host/z8k-emu/z8k-emu`, der Z8000-Code
interpretiert und die WEGA-Systemaufrufe in Linux-Aufrufe übersetzt. Das Original ist damit das **Orakel**: Was es tut,
ist per Definition richtig – auch seine Fehler. Hat das Original einen Fehler, muss der Nachbau denselben Fehler haben.

### Original und Kandidaten

Ein Korpus lässt dieselben **Fälle** laufen auf:

- dem **Original** – dem Programm aus dem WEGA-3.1-Dateibaum (`tmp/wega31_root`, z. B. `/bin/tar`), im Emulator;
- jedem **Kandidaten** – einem nachgebauten Programm:
  - `wega`: der shipped build (`src/cmd/...`), ebenfalls im Emulator;
  - `host`: die Linux-Portierung, direkt ausgeführt (nur für die Compilerkette).

Verglichen wird alles, was das Programm hinterlässt: was es ausgibt, sein Exit-Status und hinterher jede Datei seines
Arbeitsverzeichnisses. Unterscheidet sich irgendetwas, meldet der Korpus den Fall.

### Ein Fall

Ein **Fall** ist ein kleiner, in sich geschlossener Test: ein Verzeichnis mit Startdateien und eine Liste von
**Schritten**. Ein Schritt startet ein Programm mit Argumenten. Beispiel (der Fall `mv-dir` von mv):

```
-- d1/f                  ← eine Startdatei d1/f (leer)
-- case
step: mv d1 d2           ← mv ausführen (Original oder Kandidat)
expect: missing d1       ← was für den Lauf des Originals gelten muss
expect: file d2/f contains ''
expect: exit 0
```

---

## 5. Einen Korpus laufen lassen

### Der normale Weg

```sh
host/tools/corpus.sh tar            # der Korpus von tar, alle Kandidaten
host/tools/corpus.sh --wega tar     # nur der shipped build
host/tools/corpus.sh --host cpp     # nur der Host-Build
host/tools/corpus.sh                # jeder Korpus aus compare/CORPORA
```

Die Kandidaten werden dort genommen, wo `make` sie ablegt (die Pfade in `corpus.py`, `CANDIDATES`). Ein anderes Binary
testet man so:

```sh
CORPUS_BIN_WEGA=/irgendwo/tar host/tools/corpus.sh --wega tar
```

Die Ausgabe endet mit einer Zeile pro Kandidat:

```
tar [wega]: 34 of 34 inputs identical (0 sources skipped)
TIME tar      wall     0.4   34 cases
```

„sources skipped" zählt erzeugte Fälle, die schon die *originalen* Werkzeuge nicht vorbereiten konnten (z. B. eine
C-Datei, die der alte Compiler ablehnt); das sind keine Tests.

### Innerhalb von `checkall`

`make -C WEGA/host checkall WEGA=<WEGA_ROOT>` baut alles und lässt dann jeden Korpus aus `compare/CORPORA` parallel gegen
den shipped build und die Host-Builds laufen. In der Zusammenfassung erscheint eine Zeile pro Kandidat:

```
corpus tar [wega]            34 of 34 inputs identical
```

`make -C WEGA/host checkall-nocache` macht dasselbe, ignoriert aber alle gespeicherten Ergebnisse (siehe „Der Speicher"
unten).

### Das Original ansehen: `--show`

Beim Schreiben von Fällen will man sehen, was das Original tatsächlich tut. `--show` lässt nur das Original laufen,
speichert nichts und braucht keinen Kandidaten:

```sh
python3 host/recon/corpusrun.py --show rmdir             # jeder Fall
python3 host/recon/corpusrun.py --show -f slash rmdir    # nur Fälle, deren Name „slash" enthält
```

```
=== slash-dot
    host 0: exit=0
    step 1 rmdir: exit=0
    files afterwards:
      d 755 d
    expectations: all hold (2)
```

### Optionen und Umgebung

| Option / Variable | Bedeutung |
|---|---|
| `-c wega` / `-c host` (`corpus.sh --wega/--host`) | nur dieser Kandidat |
| `-f TEXT` | nur Fälle, deren Name TEXT enthält |
| `-k` | die Arbeitsverzeichnisse abweichender Fälle behalten (zum Hineinschauen) |
| `--nocache`, `CORPUS_NOCACHE=1` | den Speicher ignorieren, alles neu laufen lassen |
| `--show` | nur das Original, jeden Fall ausgeben |
| `CORPUS_BIN_WEGA`, `CORPUS_BIN_HOST` | ein anderes Binary für diesen Kandidaten |
| `JOBS` | wie viele Fälle gleichzeitig laufen (Standard: Anzahl CPUs − 1) |
| `WEGA_ROOT` | der WEGA-3.1-Baum (Standard `../tmp/wega31_root` neben `WEGA/`) |
| `CORPUS_DB` | der Speicher (Standard `WEGA/.corpus`) |
| `CORPUS_TMP` | wohin die Arbeitsverzeichnisse kommen (Standard `/tmp/wega_corpus`) |
| `CORPUS_OPTS` | Optionen, die `corpus.sh` an `corpusrun.py` weitergibt |

### Der Speicher (`WEGA/.corpus`)

Die Originale im Emulator laufen zu lassen ist langsam; ein kalter Lauf aller Korpora dauert etwa eine Viertelstunde.
Deshalb werden Ergebnisse in `WEGA/.corpus/` aufbewahrt (nicht in git):

- das **Ergebnis des Originals** für jeden Fall, unter einem Schlüssel aus: dem Fall (sein Text und seine Startdateien),
  der Variante, dem Original-Binary, dem Emulator und dem Runner selbst;
- eine **„identisch"-Marke** pro Fall und Kandidat, unter einem Schlüssel, der das Kandidaten-Binary hinzunimmt.

Hat sich davon nichts geändert, läuft nichts: Nach der Änderung eines Programms laufen nur dessen Fälle neu. Zusätzlich
merkt sich jeder Lauf, welche Dateien des WEGA-Baums er über den Emulator gelesen hat (die **gemessenen
Abhängigkeiten**); ändert sich eine davon, wird das Ergebnis neu gemacht.

Das Original läuft immer **zweimal**; nur wenn beide Läufe übereinstimmen, wird das Ergebnis behalten. Ein Fall, dessen
Original jedes Mal etwas anderes liefert, wird als `UNSTABLE` gemeldet – er wäre kein fairer Test.

Eine Änderung am Emulator oder am Runner ändert jeden Schlüssel, alte Einträge würden sich ansammeln. Jede Benutzung
setzt die Zeit eines Eintrags; ein `checkall`, der durchläuft, löscht alles, was seine Korpora nicht benutzt haben
(`host/tools/corpus_prune.py`; nicht, solange ein anderer Korpuslauf läuft, nicht bei `checkall-nocache`). Von Hand:
`corpus_prune.py STEMPELDATEI -n` zählt, was älter als STEMPELDATEI ist.

---

## 6. Wie ein Fall abläuft, Schritt für Schritt

Wer das kennt, versteht jede Meldung des Runners.

1. **Startdateien.** Die Startdateien des Falls werden in ein frisches Verzeichnis kopiert, eines pro Seite:
   `CORPUS_TMP/corpusrun/NAME/orig/FALL.0`, `.../wega/FALL.0`, `.../host/FALL.0`. Alle Seiten benutzen ein Verzeichnis
   **gleichen Namens**, damit ein Programm, das sein Arbeitsverzeichnis ausgibt oder speichert, überall dieselben Bytes
   erzeugt.
2. **Der Kandidat** wird *neben* dieses Verzeichnis kopiert (`FALL.0.prog/`), nie hinein, damit ein Programm, das sein
   eigenes Verzeichnis auflistet, ihn nicht sieht.
3. **Die Schritte laufen der Reihe nach**, jeder in diesem Verzeichnis, mit `umask 022`:
   - `step: PROG ARGS` – auf der Seite des Originals `z8k-emu -r WEGA_ROOT -0 PROG /ORIG ARGS` (z. B. `/bin/tar`), auf
     der Seite `wega` dasselbe mit dem Kandidaten, auf der Seite `host` das Linux-Binary direkt. `argv[0]` ist das Wort
     so, wie es dasteht (`-0`), wie es eine Shell übergeben würde.
   - `step: ANDERES ARGS` (ein Programm, das nicht getestet wird, z. B. `step: ls -l`) – das originale `/bin/ANDERES`
     des WEGA-Baums auf *jeder* Seite: Es ist ein Werkzeug des Falls, nicht Gegenstand des Tests.
   - `host: ZEILE` – eine Zeile der Linux-Shell (`sh -c`), zur Vorbereitung: Dateien anlegen, Zeiten und Rechte setzen.
4. **Festgehalten** wird pro Schritt: stdout, dann stderr (markiert mit `-- stderr`), und der Exit-Status. Nach dem
   letzten Schritt: jeder Eintrag des Verzeichnisses (Typ, Rechte, Größe, Prüfsumme, Linkanzahl – nicht Eigentümer und
   nicht Zeiten).
5. **Erwartungen** (`expect:`-Zeilen) werden am Lauf des Originals geprüft.
6. **Verglichen** wird das Protokoll jedes Kandidaten mit dem Protokoll des Originals.

Das Protokoll eines Falls sieht so aus (das gibt `--show` aus, und darauf bezieht sich eine `DIFF`-Zeile):

```
host 0: exit=0
step 1 tar: exit=0
step 2 tar: exit=0
f1
f2
-- stderr
tar: blocksize = 6
files afterwards:
  f 644 3072 c9110ef5 1 a.tar        ← Typ f(ile), Rechte, Größe, crc32, Links, Name
  f 664 4 6b9c2d9a 1 f1
  d 755 sub                          ← Typ d(irectory), Rechte, Name
```

---

## 7. Drei Arten, Fälle zu schreiben

Alle drei ergeben dieselbe Art von Fall; man wählt, was zum Programm passt.

### a) `tests.txt`: viele kleine Fälle in einer Datei (empfohlen für Kommandos)

Benutzt von dc, rm, rmdir, mv, tar. Eine Textdatei, ein Block pro Fall. Das Format liest `host/recon/casefile.py`.

```
== interactive
# man: -i asks for every file; a line starting with y removes it
-- a
-- b
-- answers
y
n
-- case
step: rm -i a b
stdin: answers
expect: output is 'a: b: '
expect: missing a
expect: file b contains ''
```

Regeln eines Blocks:

- `== NAME` beginnt einen Fall. Namen kurz halten (sie werden zu Verzeichnisnamen).
- Die Zeilen nach `== NAME` bis zur ersten `-- `-Zeile sind die Standard-Eingabedatei (`t.in`, oder was der Korpus
  sagt, z. B. `t.dc` bei dc). Die meisten Fälle benutzen stattdessen benannte Dateien.
- `-- DATEI` – die folgenden Zeilen sind die Datei DATEI (ein Pfad wie `d/sub/f` legt die Verzeichnisse an). Ein
  `-- DATEI` ohne Zeilen ist eine leere Datei.
- `-- case` – die folgenden Zeilen sind Zeilen der Falldatei (Abschnitt 8): `host:`, `step:`, `stdin:`, `expect:` …
- `-- expect` – die folgenden Zeilen sind die **genaue** gesamte Ausgabe des letzten Schritts (wird zu
  `expect: output is …`).
- Eine Zeile, die mit `#` beginnt, ist ein Kommentar von `tests.txt` (dc selbst kennt keine Kommentare).
- `\ooo` in einer Dateizeile ist das Byte mit diesem Oktalwert. Damit schreibt man Bytes, die ein Editor verlieren
  würde: `\040` für ein Leerzeichen am Zeilenende, `\011` für einen Tabulator, `\344` für ein Byte über 127.
- Ohne jeden `step:` wird der Standardschritt des Korpus angehängt.

Die `corpus.py` eines solchen Korpus ist winzig (die vollständige von rmdir):

```python
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'rmdir'
ORIG = 'bin/rmdir'
PROG = 'rmdir'
CANDIDATES = {'wega': 'src/cmd/rmdir'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'rmdir t.in')
```

### b) `cases/FALL/`: ein Verzeichnis pro Fall

Benutzt von ar. Gut, wenn ein Fall echte binäre Startdateien braucht (die kommen nach `cases/FALL/in/`) oder sehr lang
ist.

```
compare/ar/cases/libc.a/case
```
```
# ar: the table of contents of libc.a of the system
host: cp $WEGA_ROOT/lib/libc.a .
step: ar tv libc.a
variant:
variant: TZ=MET-1MEST
variant: TZ=PST8PDT
expect: exit 0
expect: output contains doprnt.o
```

Der Runner findet diese Verzeichnisse selbst; `corpus.py` braucht dafür kein `generate()`.

### c) Ein Generator in `corpus.py`: Fälle aus dem Baum erzeugt

Benutzt von den Compilerpässen, den Linkern, cas. Eine Funktion `generate(g)` erzeugt Fälle aus vorhandenem Material,
zum Beispiel aus jeder C-Quelle des Baums. Ein Fall kann eine **Vorbereitung** haben: eine Python-Funktion, die die
Startdateien macht, oft durch Ausführen *originaler* Werkzeuge (der originale Präprozessor macht `t.i` für den Test von
`cparse`). Vorbereitete Eingaben werden ebenfalls gespeichert (`.corpus/inputs/`), so dass eine Vorbereitung nur einmal
läuft, bis sich ihre Quellen ändern.

Der Korpus von cpp (gekürzt):

```python
import corpusc   # host/recon/corpusc.py: Helfer für C-Quellen

NAME = 'cpp'
ORIG = 'lib/cpp'
PROG = 'cpp'
CANDIDATES = {'wega': 'src/cmd/cpp/cpp', 'host': 'host/cpp/cpp'}

def generate(g):
    for src in g.c_sources():                       # jede .c von src/cmd und src/lib
        b = os.path.basename(src)
        yield g.case(src,                           # gemacht aus dieser Datei (und ihrem Verzeichnis)
                     corpusc.prepare_cpp,           # Vorbereitung: das Verzeichnis kopieren
                     'step: cpp -I. %s t.i\n' % b)  # der Text des Falls
```

Was `g` anbietet:

| Aufruf | Bedeutung |
|---|---|
| `g.c_sources()` | jede C-Quelle, die git unter `src/cmd` und `src/lib` kennt |
| `g.case(src, prep, text, prep_files=(), name=None, sources=None)` | ein Fall: `prep(src, arbeitsverzeichnis)` macht die Startdateien (gibt `None` zurück, wenn die originalen Werkzeuge es nicht können – der Fall wird dann *übersprungen*); `text` ist die Falldatei; `name` ist standardmäßig der Pfad von `src`; `sources` sind die Dateien, die die Vorbereitung liest (Standard: die Dateien im Verzeichnis von `src`) – sie gehen in den Schlüssel ein |
| `g.name(src)` | der Name eines aus `src` gemachten Falls |
| `g.dir_files(src)` | die Dateien, die git im Verzeichnis von `src` kennt |

Varianten von Optionen: popt lässt jeden Fall dreimal laufen, ohne Option, mit `-l` und mit `-d`:

```
step: popt $V in.s out.s
variant:
variant: -l
variant: -d
```

`$V` wird durch die Wörter der Variantenzeile ersetzt (eine leere `variant:`-Zeile heißt: keine Wörter).

---

## 8. Referenz: die Falldatei

Ein Feld pro Zeile; eine Zeile, die mit `#` beginnt, ist ein Kommentar.

| Feld | Bedeutung |
|---|---|
| `step: WÖRTER` | Ein WEGA-Programm im Emulator ausführen. Die Wörter werden wie von einer Shell getrennt (Anführungszeichen wirken), aber es gibt **keine Shell**: kein `>`, `|`, `;`, `$(...)`. Ein Wort mit `*`, `?`, `[` wird im Fallverzeichnis expandiert. `$V` sind die Wörter der Variante. Das erste Wort ist `PROG` (das getestete Programm) oder ein Name aus `ALIASES`; jedes andere erste Wort startet auf jeder Seite das originale `/bin/WORT`. |
| `stdin: DATEI` | Die Standardeingabe des vorigen Schritts (eine Datei des Fallverzeichnisses). Ohne sie ist stdin leer. |
| `env: NAME=WERT` | Umgebung des vorigen Schritts; vor dem ersten Schritt: jedes Schritts. |
| `host: ZEILE` | Eine Zeile der Linux-Shell zwischen den Schritten (`sh -c`): Vorbereitung, `chmod`, `touch -d`, `ln`. `$WEGA_ROOT` ist gesetzt. Auch ihre Ausgabe und ihr Exit-Status werden festgehalten. |
| `variant: NAME=WERT … WORT …` | Der Fall läuft einmal pro `variant:`-Zeile: `NAME=WERT`-Paare kommen in die Umgebung, andere Wörter nach `$V`. Keine `variant:`-Zeile: Der Fall läuft einmal. |
| `expect: exit N` | Der letzte Schritt endet mit Status N. |
| `expect: output contains TEXT` | Die Ausgabe irgendeines Schritts enthält TEXT (mit Leerzeichen in Anführungszeichen: `'unable to open'`). |
| `expect: output is TEXT` | Die gesamte Ausgabe des letzten Schritts ist TEXT (`\n` für Zeilenende). stderr steht hinter `-- stderr\n`. |
| `expect: file PFAD contains TEXT` | Die Datei existiert hinterher und enthält TEXT (`''`: sie existiert einfach). |
| `expect: file PFAD same-as ANDERE` | Zwei Dateien des Verzeichnisses sind gleich (oder `in/DATEI` eines handgeschriebenen Falls). |
| `expect: missing PFAD` | Die Datei ist hinterher weg. |
| `terminal: console` | Das minimale Terminal: stdin und stdout jedes Schritts sind die Konsole (`isatty`, `ttyname` liefern `/dev/console`, die tty-ioctls funktionieren). Nötig für Programme, die nur an der Systemkonsole laufen (getud, putud, format). |
| (corpus.py) `ROOT_COPY = ['etc/mnttab']` | Dateien des Baums, die ein Fall ändert: jeder Fall bekommt eine eigene Kopie in seiner Schicht über ROOT. |
| (corpus.py) `INSTALLED = True` | Der Kandidat liegt an seinem Platz im Baum (`ORIG` und die Pfade von `ALIASES`) in einer Schicht über ROOT, die jeder Fall hat (z8k-emu `Z8K_OVERLAY`); die Schritte rufen ihn über diesen Pfad auf (`step: /etc/mfs`). Ein Shell-Skript startet `/bin/sh` mit `$0` = diesem Pfad, wie WEGAs sh. Was ein Fall in ein Verzeichnis der Schicht schreibt, bleibt in der Schicht (der Baum ändert sich nicht). |
| `device: driveN DATEI [ro]` | Diskettenlaufwerk N (0..3) ist das Image DATEI im Fallverzeichnis (`ro`: für den 8-Bit-Teil schreibgeschützt): jedes `/dev/fd*`, `/dev/rfd*` dieses Laufwerks liest und schreibt es. Eine UDOS-Diskette macht `host: python3 $WEGA_HOST/tools/udosfs.py format DATEI` (gefüllt mit `udosfs.py put`). |
| `operator: [at 'TEXT'] [insert driveN DATEI] [type 'TEXT']` | (nach einem `step:`, beliebig viele, der Reihe nach) Was der Bediener tut, wenn TEXT in der Ausgabe des Schritts erschienen ist (nach dem Text der Aktion davor; ohne `at`: wenn der Schritt beginnt): legt die Diskette DATEI des Falls in Laufwerk N (sie bleibt für die nächsten Schritte darin), tippt TEXT auf die Standardeingabe (`\n` ein Zeilenende). Die Standardeingabe des Schritts bleibt bis zur letzten Aktion offen. Beispiel: `compare/tar/tests.txt`, die disks-Fälle (ein Archiv über zwei Disketten). |
| `device: ud` | `/dev/ud` ist der 8-Bit-Teil des P8000 (`host/z8k-emu/models/ud8.py`) mit den Laufwerken des Falls; was er auf seiner eigenen Konsole ausgibt, steht am Ende des Protokolls als `8-bit console:`. Braucht `terminal: console` (wie unter WEGA). |
| `device: eprom DATEI TYP [board=N] [short] [busy=S]` | Der EPROM-Programmer hinter `/dev/ud` (braucht `device: ud`): das EPROM vom TYP (`e2708` ... `e27512`) in seiner Fassung ist DATEI (gelöscht, alles FFH, angelegt, wenn sie fehlt); `board=1` die alte Karte, `board=2` die neue (Vorgabe); `short`: die Karte meldet einen Kurzschluss; `busy=S`: die Karte arbeitet bei jeder Anforderung S Sekunden, damit ein `signal:` das wartende Programm erreicht (Beispiel: `compare/pburn/tests.txt`, die Signal-Fälle). |
| `device: NAME ART:ARG [excl] [console]` | Ein beliebiges Gerät aus WEGAs `/dev` mit einem Modell des Emulators (`image:`, `host:`, `proc:`, `console`; `host/z8k-emu/README.md`). |
| `time: SEKUNDEN` | Die Uhr des Gasts steht auf SEKUNDEN seit 1970: Zeiten, die ein Programm schreibt oder ausgibt, ändern sich zwischen den Läufen nicht. |
| `signal: SIG after SEKUNDEN` / `signal: SIG at 'TEXT'` | (nach einem `step:`) Das Signal SIG (ein WEGA-Name: `HUP INT QUIT ILL TRAP IOT EMT FPE KILL BUS SEGV SYS PIPE ALRM TERM USR1 USR2 CLD PWR`, oder seine Nummer) geht an die Prozessgruppe des Schritts, wie ^C eines Terminals: nach SEKUNDEN, oder sobald TEXT in seiner Ausgabe erschienen ist (stdout in eine Pipe ist gepuffert: TEXT wird erst gefunden, wenn das Programm ihn ausschreibt). Die Standardeingabe des Schritts bleibt bis dahin offen, ein lesendes Programm wartet also. Die Handler des Programms laufen wie unter WEGA. Ein durch ein Signal beendeter Schritt steht mit `exit=-N` und der WEGA-Nummer N im Protokoll (`expect: exit -3`). Beispiel: `compare/dc/tests.txt`, Fälle `interrupt` und `sigquit`. |
| `user: UID GID` | Die Schritte laufen als WEGA-Benutzer UID mit Gruppe GID (0 ist root), egal welcher Host-Benutzer: der Emulator entscheidet die Rechte nach den Regeln des Kernels, die Dateien des Falls gehören UID, GID, die des Baums root (`host/z8k-emu/README.md`, "Users"). Ein nativer Kandidat (`host`) läuft als Host-Benutzer: er läuft diese Fälle nicht (die Zusammenfassung zählt sie gesondert). Der Kandidat bekommt den Besitzer, den das Original im Baum hat (root): ein set-user-id-Programm (mv, rmdir) ist root wie unter WEGA. Beispiele: die user-Fälle von `compare/tar/tests.txt`, `mv`, `rm`, `rmdir`, `compare/ar/cases/user*`. |
| `fs:` | Reserviert für ein späteres Emulator-Modell; es zu benutzen ist ein Fehler, bis das Modell existiert. |

Erwartungen werden am **Original** geprüft. Sie belegen, dass ein Fall wirklich testet, was er behauptet (mehr in
Abschnitt 11).

---

## 9. Referenz: `corpus.py`

| Name | Pflicht | Bedeutung |
|---|---|---|
| `NAME` | ja | der Name des Korpus |
| `ORIG` | ja | das Original, ein Pfad im WEGA-Baum ohne führendes `/` (`'bin/tar'`) |
| `PROG` | ja | das Wort, das in `step:`-Zeilen „das getestete Programm" bedeutet |
| `CANDIDATES` | ja | `{'wega': 'src/cmd/tar'}` – Pfad jedes nachgebauten Binarys, relativ zu `WEGA/` |
| `ALIASES` | | ein Programm, mehrere Namen: `{'cp': 'bin/cp', 'ln': 'bin/ln'}` (Wort → Original). Der Kandidat läuft unter demselben Namen (mv wählt seine Arbeit nach seinem Namen). |
| `generate(g)` | | liefert erzeugte Fälle (Abschnitt 7c, oder `casefile.generate` für `tests.txt`) |
| `normalize(what, data)` | | macht undefinierte Teile vor dem Vergleich gleich: `what` ist `'output'` (die Ausgabe eines Schritts, ein String) oder ein Dateiname (deren Bytes). dc benutzt es für Adressen, die sein Debug-Befehl ausgibt. |
| `after_step(argv, rc, out, err, dir)` | | läuft nach jedem Schritt, darf Ausgabe kürzen oder Dateien entfernen, die das Original undefiniert hinterlässt; gibt `(out, err)` zurück. cas benutzt es: Nach einem Fehler hinterlässt das Original Dateien mit zufälligem Inhalt. |
| `TIMEOUT` | | Sekunden pro Schritt (Standard 600); ein Schritt, der länger läuft, endet mit Exit-Status `-999`. |
| `IN_TMP` | | `True`: Das Fallverzeichnis ist `/tmp/FALL` des Gasts (nötig, wenn ein Programm `/bin/pwd` fragt, wo es ist, wie tar). |
| `ROOT_FILES` | | Dateien des WEGA-Baums, die *außerhalb* des Emulators gelesen werden (z. B. von einer `host:`-Zeile kopiert); sie gehen in den Schlüssel ein. Was über den Emulator gelesen wird, wird ohnehin gemessen. |
| `ROOT_ENV` | | das Original liegt in einem anderen Baum, den diese Umgebungsvariable nennt (casold: `OLDROOT`). |
| `DEPENDS` | | `{'host': ['host/ar']}` – Verzeichnisse, von deren Inhalt ein Kandidat abhängt (Programme, die er startet). |
| `HOST_ENV`, `host_setup(dir)` | | Umgebung und Einrichtung für Host-Kandidaten (z. B. wo der Host-`cpp` die Include-Dateien findet). |

---

## 10. Anleitung: ein Korpus von Grund auf

Diese Anleitung geht durch, wie man einen Korpus für ein kleines Kommando anlegt. Wir nehmen `rmdir` (den gibt es schon,
so dass man das Ergebnis mit `compare/rmdir/` vergleichen kann).

### Schritt 1: das Programm kennenlernen

Die **Handbuchseite** lesen (im WEGA-Baum, mit dem originalen `pcat`):

```sh
cd WEGA
host/z8k-emu/z8k-emu -r ../tmp/wega31_root /usr/bin/pcat /usr/man/man1/rm.1    # rmdir ist in rm(1) beschrieben
```

Dann den **Quelltext** lesen (`src/cmd/rmdir.c`). Aufschreiben:

- alles, was das Handbuch verspricht (jede Option, jede Aufrufform),
- jede Meldung, die der Quelltext ausgeben kann, und wie man sie erreicht,
- alles Merkwürdige: Grenzen (Puffergrößen), ungewöhnliche Prüfungen, Stellen, an denen der Quelltext falsch aussieht.

Bei rmdir zeigt der Quelltext unter anderem: eine Usage-Meldung mit Exit-Status 2; „non-existent", „not a directory",
„not empty", „unreadable", „no permission", „cannot remove current directory", „cannot remove . or .."; und eine
eigenartige Prüfung auf `.` und `..` (sie schaut auf `strrchr(name, '/')`, das auf den Schrägstrich zeigt, so dass
`rmdir d/.` durchrutscht).

### Schritt 2: Verzeichnis und `corpus.py` anlegen

```sh
mkdir compare/rmdir
```

Die kleine `corpus.py` aus Abschnitt 7a kopieren und `NAME`, `ORIG`, `PROG`, `CANDIDATES` anpassen. Der Pfad des
Kandidaten ist der, an dem `make shipped` in `src/cmd` das Binary ablegt.

### Schritt 3: die ersten Fälle schreiben

`compare/rmdir/tests.txt`:

```
== empty
# man: rmdir removes the named directories, which must be empty
-- case
host: mkdir a b
step: rmdir a b
expect: missing a
expect: missing b
expect: exit 0

== not-empty
# src: "rmdir: NAME not empty" on the standard error, exit status 2
-- d/f
-- case
step: rmdir d
expect: output contains 'rmdir: d not empty'
expect: exit 2
```

Die Erwartungen **aus Handbuchseite und Quelltext** schreiben – bevor irgendetwas läuft.

### Schritt 4: das Original ansehen

```sh
python3 host/recon/corpusrun.py --show rmdir
```

Für jeden Fall vergleichen, was das Original getan hat, mit dem, was man erwartet hat:

- **Erwartung hält** – gut.
- **Erwartung schlägt fehl** – *im Quelltext* herausfinden, warum. Entweder war die Erwartung falsch (korrigieren und
  in den Kommentar schreiben, was man gelernt hat), oder das Original verhält sich überraschend oder hat einen Fehler
  (den Fall behalten, das Original hat recht, und den Fehler in die Tabelle „Bugs and quirks of the originals" in
  `host/KNOWLEDGE.md` eintragen).
- Nie die Ausgabe des Originals in eine Erwartung kopieren, ohne sie zu verstehen. Dann würde die Erwartung nichts
  beweisen.

### Schritt 5: gegen den Kandidaten laufen lassen

Das shipped-Programm bauen (`checkall` macht das, oder `make shipped` in `src/cmd` mit den WEGA-Werkzeugen), dann:

```sh
host/tools/corpus.sh rmdir
```

```
rmdir [wega]: 13 of 13 inputs identical (0 sources skipped)
```

Weicht ein Fall ab, erscheint eine `DIFF`-Zeile (Abschnitt 13). Eine Abweichung ist ein Befund: Das nachgebaute Programm
verhält sich nicht wie das Original. Behoben wird sie auf *unserer* Seite.

### Schritt 6: beweisen, dass der Korpus Fehler findet (Mutationstest)

Ein Korpus, der nie fehlschlägt, ist wertlos. Man macht eine absichtlich kaputte Kopie des Programms, baut sie und lässt
den Korpus darauf laufen: Er muss Abweichungen melden. Bei rmdir `exit(Errors? 2: 0)` in `exit(Errors? 1: 0)` ändern:

```sh
CORPUS_BIN_WEGA=/pfad/zum/kaputten/rmdir host/tools/corpus.sh rmdir
```
```
DIFF mixed: [wega] -step 1 rmdir: exit=2 | +step 1 rmdir: exit=1
rmdir [wega]: 5 of 13 inputs identical (0 sources skipped)
```

Die kaputte Kopie nicht committen.

### Schritt 7: in checkall aufnehmen

Eine Zeile `rmdir` in `compare/CORPORA` eintragen. `make -C WEGA/host checkall WEGA=<WEGA_ROOT>` laufen lassen; es muss
mit `ALL OK` enden.

---

## 11. Gute Fälle schreiben

### Jeder Fall muss etwas Echtes testen

Ein Emulator, dem eine Fähigkeit fehlt, lässt Original und Kandidat **auf dieselbe Weise** scheitern – und der Korpus
sagt „identisch", obwohl nichts getestet wurde. Beispiel aus der Geschichte: Ein Fall von ar, dessen Shell-Schleife
einen Tippfehler hatte, erzeugte auf keiner Seite ein Archiv; beide Seiten waren „gleich". Erst eine Erwartung
(`expect: file x.a contains …`) hat es aufgedeckt. Deshalb: **Jeder Fall bekommt mindestens eine Erwartung**, die zeigt,
dass das Original wirklich das getan hat, worum es im Fall geht.

### Erwartungen kommen aus Handbuch und Quelltext

Nicht aus einem Lauf des Originals. Wenn `--show` einem widerspricht, entscheidet der Quelltext, wer recht hat. Was man
dabei gelernt hat, kommt in den Kommentar des Falls, z. B.:

```
# src: % is the remainder of the quotient computed with the scale k (with 2k, 7.5/2 = 3.75 leaves 0)
```

### Beide Seiten des Programms abdecken

- **Die öffentliche Schnittstelle**: jede Option und Aufrufform der Handbuchseite, die Beispiele des Handbuchs, jede
  Diagnose, die es aufführt.
- **Was der Quelltext darüber hinaus hat**: versteckte Optionen, jede Meldung, Grenzen, ungewöhnliche Wege. Jeden Fall im
  Kommentar mit `man:` oder `src:` und der betroffenen Funktion kennzeichnen.

### Fälle deterministisch machen

Das Original läuft zweimal und einmal pro Kandidat, zu verschiedenen Zeitpunkten und in verschiedenen Verzeichnissen.
Alles, was sich zwischen Läufen ändert, macht einen Fall instabil oder erzeugt eine falsche Abweichung:

| Gefahr | Abhilfe |
|---|---|
| **Zeiten.** Eine Datei, die eine `host:`-Zeile anlegt, hat die aktuelle Zeit; Programme, die Zeiten speichern (tar), unterscheiden sich dann. | Zeiten setzen: `host: touch -d '1988-03-04 05:06:07 UTC' DATEI`. Der tar-Korpus macht das für jede Datei jedes Falls. |
| **Die Reihenfolge der Verzeichniseinträge** hängt vom Dateisystem ab. | Beide Seiten laufen auf demselben Dateisystem mit denselben Namen, die Reihenfolge ist also gleich; nur keine Erwartungen schreiben, die eine Reihenfolge annehmen. |
| **Prozessnummern** (Namen temporärer Dateien). | Nicht in die Ausgabe lassen; `normalize()`, wenn ein Programm sie ausgibt. |
| **Übrig gebliebener Speicher** (nicht initialisierte Puffer, die in Dateien geschrieben werden). | Alle Seiten benutzen denselben Verzeichnisnamen; der Rest muss durch den Aufbau gleich sein oder normalisiert werden. |
| **Adressen**, die Debug-Befehle ausgeben. | `normalize()` (dc macht das für `Y`). |
| **Leerzeichen am Zeilenende** in `tests.txt`. | Editoren entfernen sie: `\040` benutzen. |

### Was kein Fall werden darf

Verhalten, das vom **Speicherlayout des Binarys** abhängt – Schreiben über ein Array hinaus, Lesen von nicht
initialisiertem Speicher, der in jedem Build anders ist. Der shipped build ist anders übersetzt und kann nicht denselben
Datenmüll zeigen. Solches Verhalten wird als Fehler in `host/KNOWLEDGE.md` eingetragen und im Kopfkommentar von
`corpus.py` genannt („Not in the corpus: …").

Verhalten, das der Emulator noch nicht bieten kann (ein Terminal, ein Gerät), wird ebenfalls dort aufgeführt, bis das
Modell existiert.

### Fehler des Originals bleiben

Hat das Original einen Fehler, bleibt der Fall, und der erwartete Fehler ist das erwartete Ergebnis. Beispiel: dc läuft
bei einer Zahl mit zwei Dezimalpunkten (`1.2.3`) endlos; der Fall `two-points` erwartet genau das
(`expect: exit -999`, die Zeitgrenze).

---

## 12. Was der Emulator kann und was nicht

Der Emulator (`host/z8k-emu/`, Einzelheiten in seiner `README.md`) führt ein WEGA-Programm als normaler Linux-Benutzer
aus:

**Er bietet:** Argumente, Umgebung, Dateien unterhalb des WEGA-Baums und des Arbeitsverzeichnisses,
`fork`/`exec`/`wait`, Pipes, Verzeichnisse im WEGA-Format, auf Wunsch eine feste Uhrzeit (`Z8K_TIME`), das `/tmp` des
Gasts als eigenes Verzeichnis und einige für Korpora gebaute Modelle: einen Link auf ein Verzeichnis (`mv dir1 dir2`),
das `/tmp` des Gasts als eingehängtes Dateisystem, damit `/bin/pwd` funktioniert (`IN_TMP`), WEGAs `/dev` mit
Disketten-Images und dem 8-Bit-Teil hinter `/dev/ud` (`device:`) und eine minimale Konsole (`terminal: console`).

**Er bietet (noch) nicht:** ein Terminal, das seine Einstellungen umsetzt (Echo, Raw-Modus, Bildschirm: vi), die
Besitzer des Original-Baums (mit `user:` gehören seine Dateien root), das Einhängen von Dateisystemen, eine Core-Datei, wenn ein
Programm durch QUIT, SEGV und ähnliche stirbt. Ohne
`terminal: console` ist `isatty` falsch – `rm` verhält sich deshalb wie mit `-f`. Ein Programm, das so etwas braucht, braucht zuerst ein **Modell** im
Emulator. Nach der Regel des Projekts wird ein Modell gebaut, wenn das erste Programm es braucht, mit eigenen Tests. Die
Falldatei reserviert die Felder dafür schon (`terminal:`, `device:`, …).

---

## 13. Fehlersuche

**`DIFF NAME: [wega] -zeile | +zeile`** – der Kandidat weicht in diesem Fall vom Original ab. Zeilen mit `-` sind die des
Originals, Zeilen mit `+` die des Kandidaten (die ersten abweichenden Zeilen werden gezeigt). Mit `-k` laufen lassen,
um die Arbeitsverzeichnisse zu behalten (`CORPUS_TMP/corpusrun/NAME/wega/FALL.0`), und die Dateien ansehen.

**`DIFF NAME: EXPECT failed on the original: …`** – die eigene Erwartung gilt für das Original nicht. Mit `--show`
nachsehen, dann den Quelltext lesen.

**`DIFF NAME: UNSTABLE original: …`** – das Original hat zwei verschiedene Ergebnisse geliefert. Etwas ist nicht
deterministisch (siehe die Tabelle in Abschnitt 11). Ein solcher Fall wird nie gespeichert.

**`NAME: /pfad (wega) not built`** – das Kandidaten-Binary existiert nicht. Bauen oder mit `CORPUS_BIN_WEGA` eines
angeben.

**Ein Schritt endet mit `exit=-999` und `timeout`** – er lief länger als `TIMEOUT`. Eine Schleife im Fall? (In dc läuft
`[lyx]sy lyx` endlos: ein Aufruf am Ende eines Strings verschachtelt nicht.)

**`CASE …: unknown field` / `unknown expect`** – ein Tippfehler im Fall oder ein Problem mit Anführungszeichen:
`expect: output contains unable to open` hat zu viele Wörter; `'unable to open'` schreiben.

**Ergebnisse wirken veraltet** – einmal `--nocache` benutzen. Die Schlüssel umfassen den Fall, die Binaries, den
Emulator, den Runner und die gemessenen Dateien, deshalb sollte das selten sein; wenn es passiert, ist es ein Fehler,
den man melden sollte.

---

## 14. Begriffe

| Begriff | Bedeutung |
|---|---|
| **Original** | Das Programm, wie es in WEGA 3.1 ausgeliefert wurde (`tmp/wega31_root`). |
| **Orakel** | Das Original, das im Emulator läuft; was es tut, ist per Definition richtig. |
| **Kandidat** | Ein nachgebautes Programm, das gegen das Original getestet wird: `wega` (shipped build) oder `host` (Linux-Portierung). |
| **shipped build** | `src/cmd`, gebaut mit dem WEGA-`cc`/`scc` und der WEGA-C-Bibliothek (`make shipped`), im Gegensatz zum byte-identischen Build mit den alten Compilern. |
| **Fall** | Ein Test: Startdateien, Schritte, Erwartungen. |
| **Schritt** | Ein Lauf eines Programms innerhalb eines Falls. |
| **Variante** | Derselbe Fall noch einmal mit anderer Umgebung oder anderen Optionen. |
| **Erwartung** | Eine Aussage über den Lauf des Originals, die gelten muss (`expect:`). |
| **Speicher** | `WEGA/.corpus/`, wo Ergebnisse und vorbereitete Eingaben aufbewahrt werden. |
| **gemessene Abhängigkeiten** | Die Dateien des WEGA-Baums, die ein Lauf gelesen hat, aufgezeichnet vom Emulator (`Z8K_ROOTLOG`). |
| **checkall** | `make -C WEGA/host checkall`: baut alles, vergleicht jede installierte Datei mit dem Original, lässt jeden Korpus laufen. |
| **Modell** | Ein Teil des Emulators, der etwas des WEGA-Systems nachahmt, das der Host nicht auf dieselbe Weise hat (Verzeichnis-Links, `/tmp` als eingehängtes Dateisystem; später ein Terminal, Geräte). |

Siehe auch: `host/recon/corpusrun.py` (der Runner; sein Kopfkommentar ist die formale Referenz), `host/recon/casefile.py`
(das Format von `tests.txt`), `host/increments/110-corpus-architecture.md` (der Entwurf und seine Entscheidungen),
`host/z8k-emu/README.md` (der Emulator).
