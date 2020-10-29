IAR_PATH="C:\Program Files (x86)\IAR Systems\EWARM-8.50.1\common\bin\IarBuild.exe"
EWP_PATH="$(CURDIR)\tirtos\iar\persistent_app\cc2652rblp_persistent_app.ewp"
VARFILE="$(CURDIR)\tirtos\iar\ble5_persistent_app.custom_argvars"

all: clean FlashROM_Release FlashROM_Debug

FlashROM_Release:
	$(IAR_PATH) $(EWP_PATH) -build FlashROM_Release -parallel 4 -varfile $(VARFILE)

FlashROM_Debug:
	$(IAR_PATH) $(EWP_PATH) -build FlashROM_Debug -parallel 4 -varfile $(VARFILE)

clean:
	$(IAR_PATH) $(EWP_PATH) -clean FlashROM_Release -parallel 4 -varfile $(VARFILE)
	$(IAR_PATH) $(EWP_PATH) -clean FlashROM_Debug -parallel 4 -varfile $(VARFILE)
