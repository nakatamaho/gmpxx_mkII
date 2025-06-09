for f in *.cpp *.cc *.h *.h.in; do
  clang-format-19 -i -style '{
    BasedOnStyle: llvm,
    IndentWidth: 4,
    ColumnLimit: 0,
    SortIncludes: false,
    AlignEscapedNewlines: LeftWithLastLine
  }' "$f"
done

