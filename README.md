# LongStitch
A genome assembly correction and scaffolding pipeline using long reads, consisting of up to three steps:
* [Tigmint](https://github.com/bcgsc/tigmint) cuts the draft assembly at potentially misassembled regions
* [ntLink](https://github.com/bcgsc/ntLink) is then used to scaffold the corrected assembly
* followed by [ARKS](https://github.com/bcgsc/arcs) for further scaffolding (optional extra step of scaffolding)

<img src="https://github.com/bcgsc/LongStitch/blob/master/LongStitch_overview.png" width="480">

### Credits
LongStitch was developed and designed by Lauren Coombe, Janet Li, Theodora Lo and Rene Warren.

### Citing LongStitch
If you use LongStitch in your research, please cite:

Coombe L, Li JX, Lo T, Wong J, Nikolic V, Warren RL and Birol I. LongStitch: High-quality genome assembly correction and scaffolding using long reads. bioRxiv. 2021;2021.06.17.448848. doi: https://doi.org/10.1101/2021.06.17.448848.


### Dependencies
* [Tigmint](https://github.com/bcgsc/tigmint) v1.2.3+
* [ntLink](https://github.com/bcgsc/ntLink) v1.0.0+
* [ARCS/ARKS](https://github.com/bcgsc/arcs) v1.2.2+
* [ABySS](https://github.com/bcgsc/abyss) v2.0.0+
* [LINKS](https://github.com/bcgsc/links) v1.8.5+
* [samtools](http://www.htslib.org/)

### Example command
For example, to run the default pipeline on a draft assembly `draft-assembly.fa` with the reads `reads.fa.gz` and a genome size of `gsize`:
```
longstitch run draft=draft-assembly reads=reads G=gsize
```
Note that specifying `G` is required when `span=auto` for Tigmint-long.

The output scaffolds can be found in soft-links with the suffix `longstitch-scaffolds.fa`

### LongStitch demo
To test your LongStitch installation and see examples of how to run the pipeline, see `tests/run_longstitch_demo.sh`

To run the demo script, ensure all dependencies are in your PATH, and run the bash script:
```
cd tests
./run_longstitch_demo.sh
```

### Full help page
To run the LongStitch pipeline, you can use the Makefile driver script `longstitch`.
```
Usage: ./longstitch [COMMAND] [OPTION=VALUE]…
	Commands:

	run     		run default LongStitch pipeline: Tigmint, then ntLink

	tigmint-ntLink-arks	run full LongStitch pipeline: Tigmint, ntLink, then ARCS in kmer mode
	tigmint-ntLink		run Tigmint, then ntLink (Same as 'run' target)
	ntLink-arks		run ntLink, then run ARCS in kmer mode

	General options (required):
	draft			draft name [draft]. File must have .fa extension
	reads			read name [reads]. File must have .fq.gz or .fa.gz extension

	General options (optional):
	t			number of threads [8]
	z			minimum size of contig (bp) to scaffold [1000]

	Tigmint options:
	span			min number of spanning molecules to be considered correctly assembled [auto]
	dist			maximum distance between alignments to be considered the same molecule [auto]
	G			haploid genome size (bp) for calculating span parameter. (e.g. '3e9' for human genome) [0]

	ntLink options:
	k_ntLink		k-mer size for minimizers [32]
	w			window size for minimizers [100]

	ARCS+LINKS options:
	j			minimum fraction of read kmers matching a contigId (used in kmer mode) [0.05]
	k_arks			size of a k-mer (used in kmer mode) [20]
	c			minimum aligned read pairs per molecule [4]
	l			minimum number of links to compute scaffold [4]
	a			maximum link ratio between two best contain pairs [0.3]

Notes:
	- by default, span is automatically calculated as 1/4 of the sequence coverage of the input long reads
	- G (genome size) must be specified if span=auto
	- by default, dist is automatically calculated as p5 of the input long read lengths
	- Ensure that all input files are in the current working directory, making soft-links if needed
```


## License
LongStitch Copyright (c) 2020 British Columbia Cancer Agency Branch. All rights reserved.

LongStitch is released under the GNU General Public License v3

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see http://www.gnu.org/licenses/.

For commercial licensing options, please contact Patrick Rebstein (prebstein@bccancer.bc.ca).

