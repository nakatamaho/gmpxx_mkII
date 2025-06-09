for f in *.cpp *.cc *.h *.h.in; do
  clang-format-19 -i -style '{
    BasedOnStyle: llvm,
    IndentWidth: 4,
    ColumnLimit: 10000,
    SortIncludes: false,
    AlignEscapedNewlines: LeftWithLastLine
  }' "$f"
done

