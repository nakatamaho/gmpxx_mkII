# Documentation and Reference Material

This directory contains project documentation and local upstream reference
material used while implementing `gmpxx_mkII`.

The repository's own library source is BSD-2-Clause, but not every file under
`docs/` is BSD-2-Clause. In particular, the copied GMP upstream reference
material keeps its upstream GNU documentation or library-file license.

## License Layout

| Path | Contents | License / Notice |
|---|---|---|
| `docs/reference/upstream/gmp-6.3.0/source/gmp.texi` | Upstream GMP manual source | GNU Free Documentation License, version 1.3 or later, with the cover-text terms stated in the file. |
| `docs/reference/upstream/gmp-6.3.0/generated/gmp.*` | Generated GMP manual formats derived from `gmp.texi` | GNU Free Documentation License, version 1.3 or later, same manual terms as the source. |
| `docs/reference/upstream/gmp-6.3.0/source/fdl-1.3.texi` | GFDL license text included with the GMP manual source | GNU Free Documentation License text; changing the license document is not allowed by its own notice. |
| `docs/reference/upstream/gmp-6.3.0/source/configuration` | Upstream GMP development/configuration note | GNU MP Library notice: LGPLv3-or-later or GPLv2-or-later, or both in parallel. |
| `docs/reference/upstream/gmp-6.3.0/source/projects.html` | Upstream GMP development projects page | GNU MP Library notice: LGPLv3-or-later or GPLv2-or-later, or both in parallel. |
| `docs/reference/upstream/gmp-6.3.0/source/tasks.html` | Upstream GMP development tasks page | GNU MP Library notice: LGPLv3-or-later or GPLv2-or-later, or both in parallel. |
| `docs/reference/upstream/gmp-6.3.0/curated/` | Project-specific implementation notes derived from review work | Repository documentation, BSD-2-Clause unless a file states otherwise. |

Do not treat the copied upstream GMP documentation as part of the
BSD-2-Clause `gmpxx_mkII` source license. Preserve upstream notices when
updating or regenerating this material.
