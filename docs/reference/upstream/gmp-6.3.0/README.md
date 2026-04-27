# GMP upstream reference material

This directory contains local reference material derived from the GNU GMP
documentation.

The files under `source/` and `generated/` are upstream documentation material.
They are not part of the BSD-2-Clause-licensed `gmpxx_mkII` library source.

## License Notes

The upstream GMP material in this directory keeps the upstream notices from
the copied files. It must not be treated as BSD-2-Clause project source.

| Path | Contents | License / Notice |
|---|---|---|
| `source/gmp.texi` | GNU MP manual Texinfo source | GNU Free Documentation License, version 1.3 or later, with no Invariant Sections, with the Front-Cover Texts and Back-Cover Texts stated in the file. |
| `generated/gmp.info`, `generated/gmp.html`, `generated/gmp.txt` | Generated formats derived from the GNU MP manual | GNU Free Documentation License, version 1.3 or later, same manual terms as `source/gmp.texi`. |
| `source/gmp.info`, `source/gmp.info-1`, `source/gmp.info-2` | Upstream info files copied with the source reference material | GNU Free Documentation License, version 1.3 or later, same manual terms as `source/gmp.texi`. |
| `source/fdl-1.3.texi` | GNU Free Documentation License text | GFDL license text from the Free Software Foundation. Its own notice permits verbatim copying but does not permit changing the license document. |
| `source/configuration` | Upstream GMP development/configuration note | GNU MP Library notice: LGPLv3-or-later or GPLv2-or-later, or both in parallel. |
| `source/projects.html` | Upstream GMP development projects page | GNU MP Library notice: LGPLv3-or-later or GPLv2-or-later, or both in parallel. |
| `source/tasks.html` | Upstream GMP development tasks page | GNU MP Library notice: LGPLv3-or-later or GPLv2-or-later, or both in parallel. |
| `source/version.texi` | Upstream version metadata included by the GMP manual source | Upstream reference file; keep with the surrounding GMP manual source material. |
| `source/isa_abi_headache` | Upstream reference note copied with the GMP documentation material | Upstream reference file; keep its upstream notice if one is added or recovered. |

The files under `curated/` are project-specific notes for implementing and
reviewing `gmpxx_mkII`.
