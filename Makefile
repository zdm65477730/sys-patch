MAKEFILES	:=	sysmod overlay
TARGETS		:= $(foreach dir,$(MAKEFILES),$(CURDIR)/$(dir))

# the below was taken from atmosphere + switch-examples makefile
VERSION := 1.5.9
export VERSION_WITH_HASH := v$(VERSION)-$(shell git describe --always)

export BUILD_DATE := -DDATE_YEAR=\"$(shell date +%Y)\" \
					-DDATE_MONTH=\"$(shell date +%m)\" \
					-DDATE_DAY=\"$(shell date +%d)\" \
					-DDATE_HOUR=\"$(shell date +%H)\" \
					-DDATE_MIN=\"$(shell date +%M)\" \
					-DDATE_SEC=\"$(shell date +%S)\" \

export CUSTOM_DEFINES := -DVERSION_WITH_HASH=\"$(VERSION_WITH_HASH)\" $(BUILD_DATE)

all: $(TARGETS)
	@rm -rf $(CURDIR)/SdOut
	@mkdir -p SdOut/
	@cp -R sysmod/out/* SdOut/
	@cp -R overlay/out/* SdOut/
	@cd $(CURDIR)/SdOut; zip -r -q -9 sys-patch.zip atmosphere switch config; cd $(CURDIR)

.PHONY: $(TARGETS)

$(TARGETS):
	@$(MAKE) -C $@

clean:
	@rm -rf $(CURDIR)/SdOut
	@rm -f sys-patch.zip
	@for i in $(TARGETS); do $(MAKE) -C $$i clean || exit 1; done;
