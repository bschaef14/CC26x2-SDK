IAR_PATH="C:\Program Files (x86)\IAR Systems\EWARM-8.50.1\common\bin\IarBuild.exe"
EWP_PATH="$(CURDIR)\iar\app\cc26x2r1lp_bim_offchip.ewp"
VARFILE="$(CURDIR)\iar\bim_offchip.custom_argvars"

all: clean Release Debug Release_unsecure Debug_unsecure

Release:
	$(IAR_PATH) $(EWP_PATH) -build Release -varfile $(VARFILE)

Debug:
	$(IAR_PATH) $(EWP_PATH) -build Debug -varfile $(VARFILE)

Release_unsecure:
	$(IAR_PATH) $(EWP_PATH) -build Release_unsecure -varfile $(VARFILE)

Debug_unsecure:
	$(IAR_PATH) $(EWP_PATH) -build Debug_unsecure -varfile $(VARFILE)

clean:
	$(IAR_PATH) $(EWP_PATH) -clean Release -varfile $(VARFILE)
	$(IAR_PATH) $(EWP_PATH) -clean Debug -varfile $(VARFILE)
	$(IAR_PATH) $(EWP_PATH) -clean Release_unsecure -varfile $(VARFILE)
	$(IAR_PATH) $(EWP_PATH) -clean Debug_unsecure -varfile $(VARFILE)
