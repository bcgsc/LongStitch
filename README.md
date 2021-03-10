# LongStitch
A long-read scaffolding pipeline that consists of three stages:
* [Tigmint](https://github.com/bcgsc/tigmint) cuts the draft assembly at potentially misassembled regions
* [ntLink](https://github.com/bcgsc/ntLink) is then used to scaffold the corrected assembly
* followed by [ARCS](https://github.com/bcgsc/arcs) for further scaffolding

### Dependencies
* [Tigmint](https://github.com/bcgsc/tigmint)
* [ntLink](https://github.com/bcgsc/ntLink)
* [ARCS](https://github.com/bcgsc/arcs)
* [ABySS](https://github.com/bcgsc/abyss)
* [LINKS](https://github.com/bcgsc/links)

### Installation
```
cd src
make
```

### Usage
To run the LongStitch pipeline, you can use the Makefile driver script `longstitch`.
```
Usage: ./longstitch [COMMAND] [OPTION=VALUE]…

Commands:
all     		run full LongStitch pipeline: Tigmint, ntLink, then ARCS
tigmint-ntLink-arks	run full LongStitch pipeline: Tigmint, ntLink, then ARKS
tigmint-ntLink		run Tigmint, then run ntLink with the output of Tigmint
tigmint-arcs		run Tigmint, then run ARCS in default mode with the output of Tigmint
tigmint-arks		run Tigmint, then run ARCS in kmer mode with the output of Tigmint
ntLink-arcs		run ntLink, then run ARCS in default mode with the output of ntLink
ntLink-arks		run ntLink, then run ARCS in kmer mode with the output of ntLink

General options (required):
draft			draft name [draft]. File must have .fa extension
reads			read name [reads]. File must have .fq.gz or .fa.gz extension

General options (optional):
t			number of threads [8]
z			minimum size of contig (bp) to scaffold [1000]

Tigmint options:
span			min number of spanning molecules to be considered correctly assembled [auto]
dist			maximum distance between alignments to be considered the same molecule [auto]
G			haploid genome size (bp) for calculating span parameter. (e.g. '3e9' for human genome) [-1]

ntLink options:
k_ntLink		k-mer size for minimizers [32]
w			window size for minimizers [100]

ARCS+LINKS options:
s			minimum sequence identity (used in default mode) [70]
j			minimum fraction of read kmers matching a contigId (used in kmer mode) [0.05]
k_arks			size of a k-mer (used in kmer mode) [20]
c			minimum aligned read pairs per molecule [4]
l			minimum number of links to compute scaffold [4]
a			maximum link ratio between two best contain pairs [0.3]

Notes:
	- by default, span is automatically calculated as 1/4 of the sequence coverage of the input long reads
	- by default, dist is automatically calculated as p5 of the input long read lengths
```

For example, to run the entire pipeline on a draft assembly `draft-assembly.fa` with the reads `reads.fa.gz` and a genome size of `gsize`:
```
longstitch all draft=draft-assembly reads=reads G=gsize
```

## License
LongStitch Copyright (c) 2020 British Columbia Cancer Agency Branch. All rights reserved.

LongStitch is released under the GNU General Public License v3

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see http://www.gnu.org/licenses/.

For commercial licensing options, please contact Patrick Rebstein (prebstein@bccancer.bc.ca).

