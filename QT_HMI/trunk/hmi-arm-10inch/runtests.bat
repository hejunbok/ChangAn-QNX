cd %1
cd bin\
set PATH=%1\lib;%PATH%
FOR %%A in ("tst_*.exe") DO %%A
