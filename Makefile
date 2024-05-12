VERSION_MAJOR := 1
VERSION_MINOR := 0
VERSION_REVISION := 0

all:
	mkdir -p plugin/build
	sed -e 's/VERSION_MAJOR/$(VERSION_MAJOR)/' -e 's/VERSION_MINOR/$(VERSION_MINOR)/' -e 's/VERSION_REVISION/$(VERSION_REVISION)/' plugin/ArticBase.plgInfo > plugin/build/ArticBase.plgInfo
	$(MAKE) -C plugin VERSION_MAJOR=$(VERSION_MAJOR) VERSION_MINOR=$(VERSION_MINOR) VERSION_REVISION=$(VERSION_REVISION)
	bin2c -d app/includes/plugin.h -o app/sources/plugin.c plugin/ArticBase.3gx
	$(MAKE) -C app VERSION_MAJOR=$(VERSION_MAJOR) VERSION_MINOR=$(VERSION_MINOR) VERSION_REVISION=$(VERSION_REVISION)

clean:
	$(MAKE) -C plugin clean
	rm -f app/sources/plugin.c
	rm -f app/includes/plugin.h
	$(MAKE) -C app clean

re: clean all