IAR_PATH="C:\Program Files (x86)\IAR Systems\EWARM-8.50.1\common\bin\IarBuild.exe"
EWP_PATH="$(CURDIR)\iar\stack_wrapper.ewp"
VARFILE="$(CURDIR)\iar\stack_wrapper.custom_argvars"

all: clean Stack_Wrapper 

Stack_Wrapper:
	$(IAR_PATH) $(EWP_PATH) -build Stack_Wrapper -varfile $(VARFILE)

clean:
	$(IAR_PATH) $(EWP_PATH) -clean Stack_Wrapper -varfile $(VARFILE)
