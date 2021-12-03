#!/bin/bash

set -eux -o pipefail

echo "Launching LongStitch test runs..."
echo "Ensure all dependencies are in your PATH"

echo "Running first test - default LongStitch steps..."
longstitch -B run draft=test_scaffolds1 reads=test_reads1 G=482700 k_ntLink=24 

echo "Running second test - with optional arks-long step..."
longstitch  tigmint-ntLink-arks draft=test_scaffolds2 reads=test_reads2 G=250000 w=150 

set +x

echo ""
echo "Done tests! Compare your generated files with the files in the expected_outputs folder to ensure the tests were successful."

echo "Final scaffold files found in: test_scaffolds1.k24.w100.tigmint-ntLink.longstitch-scaffolds.fa test_scaffolds2.k32.w150.tigmint-ntLink-arks.longstitch-scaffolds.fa"
echo "Tip: compare the fasta files using abyss-fac. Some files might appear slightly different using command line tools like 'cmp' due to sequences being output on different strands."

