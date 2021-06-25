#!/bin/bash

set -eux -o pipefail

echo "Testing LongStitch installation..."
echo "Ensure all dependencies are in your PATH"

echo "Running first test - default LongStitch steps..."
longstitch -B run draft=test_scaffolds1 reads=test_reads1 G=520500 k_ntLink=24 

echo "Running second test - with optional arks-long step..."
longstitch -B tigmint-ntLink-arks draft=test_scaffolds2 reads=test_reads2 G=255000 w=150 

set +x

echo ""
echo "Done tests! Compare your generated files with the files in the expected_outputs folder to ensure the tests were successful."

echo "Final scaffold files found in: test_scaffolds1.tigmint-ntLink.longstitch-scaffolds.fa and test_scaffolds2.tigmint-ntLink-arks.longstitch-scaffolds.fa"

echo "Tip: compare the fasta files using abyss-fac. Some files might appear slightly different using command line tools like 'cmp' due to sequences being output on different strands."

