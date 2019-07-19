SetTitleMatchMode, RegEx

winid:=WinActive("A")
WinActivate .*IAR
WinWaitActive .*IAR
Send, ^d
Send, ^r
WinActivate ahk_id %winid%
