FILES=`ls *.cpp *.cc *.h *.h.in`
for _file in $FILES; do
clang-format -style="{BasedOnStyle: llvm, IndentWidth: 4, ColumnLimit: 10000, SortIncludes: false, ReferenceAlignment: Left, PointerAlignment: Left, DerivePointerAlignment: false}" $_file > ${_file}__ ; mv ${_file}__ ${_file}
done



