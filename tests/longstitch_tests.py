"""Tests for ntJoin"""

import shlex
import subprocess
import pandas as pd

def launch_longstitch(cmd):
    "Launch Longstitch with specified command"
    cmd_shlex = shlex.split(cmd)
    return_code = subprocess.call(cmd_shlex)
    assert return_code == 0


def run_longstitch(target, draft, reads, genome_size, k=32, window=1000):
    " Run LongStitch with specified parameters"
    cmd = "longstitch -B {target} draft={draft} reads={reads} G={G} " \
          "k_ntLink={k} w={w}".format(target=target, draft=draft,
                                      reads=reads, k=k, w=window,
                                      G=genome_size)
    launch_longstitch(cmd)

def run_abyssfac(scaffolds):
    "Run abyss-fac on scaffolds"
    cmd = "abyss-fac {}".format(scaffolds)
    cmd_shlex = shlex.split(cmd)
    with open(scaffolds + ".abyssfac.tsv", 'w') as outfile:
        return_code = subprocess.call(cmd_shlex, stdout=outfile)
    assert return_code == 0

def check_stats(abyssfac_filename):
    "Check stats of longstitch scaffolds"
    reference_stats = pd.read_csv("expected_outputs/{}".format(abyssfac_filename), sep="\t")
    ci_stats = pd.read_csv(abyssfac_filename, sep="\t")

    assert int(reference_stats["N50"]) == int(ci_stats["N50"])
    assert int(reference_stats["n"]) == int(ci_stats["n"])

def test_default():
    "Testing default target, specifying k"
    target, draft, reads, G, k = "run", "test_scaffolds1", "test_reads1", 482700, 24
    run_longstitch(target, draft, reads, G, k=k)
    scaffolds_filename = "{draft}.k{k}.w{w}.tigmint-ntLink.longstitch-scaffolds.fa".format(draft=draft,
                                                                                           k=k, w=100)
    scaffolds_abyssfac_filename = scaffolds_filename + ".abyssfac.tsv"
    run_abyssfac(scaffolds_filename)
    check_stats(scaffolds_abyssfac_filename)

def test_arks():
    "Testing additional ARKS-long step, specifying w"
    target, draft, reads, G, w = "tigmint-ntLink-arks", "test_scaffolds2", "test_reads2", 250000, 150
    run_longstitch(target, draft, reads, G, window=w)

    scaffolds_filename = "{draft}.k{k}.w{w}.tigmint-ntLink.longstitch-scaffolds.fa".format(draft=draft,
                                                                                           k=32, w=w)
    scaffolds_abyssfac_filename = scaffolds_filename + ".abyssfac.tsv"
    run_abyssfac(scaffolds_filename)
    check_stats(scaffolds_abyssfac_filename)

    scaffolds_filename = "{draft}.k{k}.w{w}.tigmint-ntLink-arks.longstitch-scaffolds.fa".format(draft=draft,
                                                                                                k=32, w=w)
    scaffolds_abyssfac_filename = scaffolds_filename + ".abyssfac.tsv"
    run_abyssfac(scaffolds_filename)
    check_stats(scaffolds_abyssfac_filename)
