@echo off
rem -k: Begin capture immediately
rem -i: Use the specified interface (in this case, the pipe pointing to tilogger)
rem -o: Modify a specific setting value. We use this to set up the DLT_USER_0 protocol, which is what we use for the logger output. The format of this setting is:
rem "uat:user_dlts:\"User N (DLT=[N+147])\",\"payload protocol\",\"header size\",\"header protocol\",\"trailer size\",\"trailer protocol\""
rem We don't use header or trailer protocols, and their size is set to zero.
rem -o part 2: Configure three additional columns for Level, Module and String. Each column is a tuple of "name", "type".
rem All quotes and backslashes are escaped in both invocations of -o.

start "" "C:\Program Files\Wireshark\Wireshark.exe" -k -i "\\.\pipe\tilogger-wireshark" -o "uat:user_dlts:\"User 0 (DLT=147)\",\"tilogger\",\"0\",\"\",\"0\",\"\"" -o "gui.column.format:\"No.\", \"%%m\", \"Time\", \"%%t\", \"Protocol\", \"%%p\", \"Length\", \"%%L\", \"Level\", \"%%Cus:tilogger.level:0:R\", \"Module\", \"%%Cus:tilogger.module:0:R\", \"String\", \"%%Cus:tilogger.string:0:R\""

rem To do this manually, you can add a Pipe configuration set to \\.\pipe\tilogger-wireshark using the options button in the main menu
rem and use the dialog under Edit->Preferences->Protocols->DLT_USER to add 'tilogger' under User 0.
rem Column settings are available under Preferences->Columns.