# TODO (Khangaroo): Make this process a lot less hacky (no, export did not work)
# See MakefileNSO

.PHONY: all clean starlight send

SMOVER ?= 100
BUILDVER ?= 99.37
IP ?= 192.168.1.136
DEBUGLOG ?= 1 # defaults to disable debug logger 
SERVERIP ?= 64.201.219.20 # put debug logger server IP here

PROJNAME ?= StarlightBase

all: starlight

starlight:
	$(MAKE) all -f MakefileNSO SMOVER=$(SMOVER) BUILDVER=$(BUILDVER) DEBUGLOG=$(DEBUGLOG) SERVERIP=${SERVERIP}
	$(MAKE) starlight_patch_$(SMOVER)/*.ips
	
	mkdir -p starlight_patch_$(SMOVER)/atmosphere/exefs_patches/$(PROJNAME)/
	mkdir -p starlight_patch_$(SMOVER)/atmosphere/contents/0100000000010000/exefs/
	
	mv starlight_patch_$(SMOVER)/3CA12DFAAF9C82DA064D1698DF79CDA1.ips starlight_patch_$(SMOVER)/atmosphere/exefs_patches/$(PROJNAME)/3CA12DFAAF9C82DA064D1698DF79CDA1.ips
	mv $(shell basename $(CURDIR))$(SMOVER).elf starlight_patch_$(SMOVER)/subsdk1.elf
	mv $(shell basename $(CURDIR))$(SMOVER).nso starlight_patch_$(SMOVER)/atmosphere/contents/0100000000010000/exefs/subsdk1

starlight_patch_$(SMOVER)/*.ips: patches/*.slpatch patches/configs/$(SMOVER).config patches/maps/$(SMOVER)/*.map \
								build$(SMOVER)/$(shell basename $(CURDIR))$(SMOVER).map scripts/genPatch.py
	@rm -f starlight_patch_$(SMOVER)/*.ips
	python3 scripts/genPatch.py $(SMOVER)

# builds project with the file structure used in the yuzu emulator
yuzu:
	$(MAKE) all -f MakefileNSO SMOVER=$(SMOVER) BUILDVER=$(BUILDVER)
	$(MAKE) starlight_patch_$(SMOVER)/*.ips

	mkdir -p starlight_patch_$(SMOVER)/yuzu/

	mv starlight_patch_$(SMOVER)/3CA12DFAAF9C82DA064D1698DF79CDA1.ips starlight_patch_$(SMOVER)/yuzu/3CA12DFAAF9C82DA064D1698DF79CDA1.ips
	mv $(shell basename $(CURDIR))$(SMOVER).elf starlight_patch_$(SMOVER)/subsdk1.elf
	mv $(shell basename $(CURDIR))$(SMOVER).nso starlight_patch_$(SMOVER)/yuzu/subsdk1
# builds and sends project to FTP server hosted on provided IP
send: all
	python3.8 scripts/sendPatch.py $(IP) $(PROJNAME) 

log: all
	python3.8 scripts/tcpServer.py $(SERVERIP)

sendlog: all
	python3.8 scripts/sendPatch.py $(IP) $(PROJNAME) $(USER) $(PASS)
	python3.8 scripts/tcpServer.py $(SERVERIP)

clean:
	$(MAKE) clean -f MakefileNSO
	@rm -fr starlight_patch_*