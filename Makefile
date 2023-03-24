
rf24hub: rf24hubd

doc: html_doc linux_doc avr_doc esp_doc 

clean:
	cd linux; make clean
	make clean_doc

rf24hubd:
	cd linux; make rf24hubd

html_doc:
	cd doc; make doc

linux_doc:
	doxygen doc/Doxyfile.linux

avr_doc:
	doxygen doc/Doxyfile.avr

esp_doc:
	doxygen doc/Doxyfile.esp

clean_doc:
	rm -rf ../wilmsn.github.io/rf24hub/linux
	rm -rf ../wilmsn.github.io/rf24hub/avr
	rm -rf ../wilmsn.github.io/rf24hub/esp
	rm ../wilmsn.github.io/rf24hub/*.html

.PHONY: help

help:
	@echo "Folgende Befehle sind eingebaut:"
	@echo "make clean:      Aufrümen des Arbeitsverzeichnisses"
	@echo "make linux_doc:  Doxygen zur Dokumentation des linux Verzeichnisses starten"
	@echo "make avr_doc:    Doxygen zur Dokumentation des arduino Verzeichnisses starten"
	@echo "make esp_doc:    Doxygen zur Dokumentation des esp Verzeichnisses starten"
	@echo "make clean_doc:  Kompletten doxygen output löschen"
