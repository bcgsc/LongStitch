# LongStitch
A long-read scaffolding pipeline that consists of three stages:
* [Tigmint](https://github.com/bcgsc/tigmint) cuts the draft assembly at potentially misassembled regions
* [ntJoin](https://github.com/bcgsc/ntJoin) is then used to scaffold the corrected assembly
* followed by [ARCS](https://github.com/bcgsc/arcs) for further scaffolding

### Dependencies
* Tigmint
* ntJoin
* ARCS
* ABySS
* LINKS

### Usage
To run the LongStitch pipeline, you can use the Makefile driver script `lrs-make`.
```
Usage: ./lrs-make [COMMAND] [OPTION=VALUE]…

Commands:
tigmint-ntJoin		run Tigmint, then run ntJoin with the output of Tigmint
tigmint-arcs		run Tigmint, then run ARCS with the output of Tigmint
ntJoin-arcs			run ntJoin, then run ARCS with the output of ntJoin
tigmint-ntJoin-arcs	run Tigmint, ntJoin, then ARCS

General options:
draft				draft name [draft]. File must have .fa extension
reads				read name [reads]. File must have .fq.gz or .fa.gz extension

Tigmint options:
span				min number of spanning molecules to be considered assembled [auto]
dist				maximum distance between alignments to be considered the same molecule [auto]
G					haploid genome size (bp) for calculating span parameter. can be given in scientific notation (e.g. '3e9' for human genome) or as an integer [-1]

ntJoin options:
k					k-mer size for minimizers [24]
w					window size for minimizers (bp) [250]

ARCS+LINKS options:
s					minimum sequence identity [70]
c					minimum aligned read pairs per molecule [4]
l					minimum number of links to compute scaffold [4]
a					maximum link ratio between two best contain pairs [0.3]

Notes:
	- by default, span is automatically calculated as 1/4 of the sequence coverage of the input long reads
	- by default, dist is automatically calculated as p5 of the input long read lengths
```

For example, to run the entire pipeline on a draft assembly `draft-assembly.fa` with the reads `reads.fa.gz` and a genome size of gsize:
```
lrs-make tigmint-ntJoin-arcs draft=draft-assembly reads=reads G=gsize
```
