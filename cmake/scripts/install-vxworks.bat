echo Installing %2 with %1
setlocal EnableDelayedExpansion
(
set \n=^
%=Do not remove this line=%
)

echo target wtx target!\n!load %2!\n!!\n! | gdbi86
