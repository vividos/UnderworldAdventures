del doxygen-warnings.txt
echo j | del html\*.html
echo j | del html\*.hhc
echo j | del html\*.hhp
echo j | del html\*.hhk
REM rmdir html
PATH=%PATH%;d:\devel\tools\doxygen\bin
doxygen ua-doxygen.cfg 
