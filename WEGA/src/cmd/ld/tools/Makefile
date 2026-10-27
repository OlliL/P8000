# sld analysis, the same setup as popt/tools (see ../../popt/tools): inventory of the original /bin/sld, later the
# comparison of our sources with it. Native make: '=' assignments, no GNU extensions.
#
#   make inventory        function starts and call edges of /bin/sld (JSON in $(WORK))
#   make pipeline         inventory, then findings_md.py
#
WEGA = /mnt/share/cvs/P8000/tmp/wega31_root
WORK = /mnt/share/cvs/P8000/tmp/tmp/sld
SLD  = $(WEGA)/bin/sld

inventory:
	mkdir -p $(WORK)
	python3 inventory.py $(SLD) > $(WORK)/inventory.json

pipeline: inventory
	python3 findings_md.py

.PHONY: inventory pipeline
