SUBDIRS = src

ACLOCAL_AMFLAGS = -I m4

.PHONY: all
all:
	$(MAKE) -C ./src
	mkdir -p dist
	cp -r template/pvr.epgstation dist/
	cp ChangeLog.txt dist/pvr.epgstation/
	cp LICENSE dist/pvr.epgstation/
	cp src/$(ADDONNAME) dist/pvr.epgstation/$(ADDONNAME)
	$(STRIP) -x dist/pvr.epgstation/$(ADDONNAME)

.PHONY: release
release: all
	cd dist; zip -9 -r ../pvr.epgstation.zip ./pvr.epgstation

.PHONY: clean
clean:
	$(MAKE) clean -C ./src
	rm -rf ./dist/
	rm -f ./pvr.epgstation.zip
