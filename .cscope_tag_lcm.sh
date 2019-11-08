#!/bin/bash
CUR_PATH=`pwd`
mkdir -p ./cscope_lcm
find ${CUR_PATH}/ -type d -name .git -prune -o -name "*.h" -print -o -name "*.c" -print -o -name "*cc" -print -o -name "*.cpp" -print -o -name "*.mak" -print -o -name "Makefile" -print -o -name "*.hh" -print > ./cscope_lcm/cscope.files
#find ${CUR_PATH}/ssp/ -name "*.[h|c|cc|cpp]" > cscope.files
cscope -bkq -i ./cscope_lcm/cscope.files -f ./cscope_lcm/cscope.out
#ctags -L -< ./cscope_lcm/cscope.files -f GTAGS
ctags -L -< ./cscope_lcm/cscope.files
export CSCOPE_DB=./cscope_lcm/cscope.out

##notice##
## 1, Before the dir that need to be prune, add the `-type d`
## 2, After the -prune option, add the -print after the wildcards to exclude the prune content in the stdout
