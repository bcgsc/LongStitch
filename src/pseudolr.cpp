#include "btllib/seq_reader.hpp"
#include "btllib/status.hpp"
#include "btllib/util.hpp"

#include <cassert>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <getopt.h>
#include <iostream>
#include <limits>
#include <math.h>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

const static std::string PROGNAME = "pseudolr";
const static std::string VERSION = "v1.0";

static void
print_error_msg(const std::string& msg)
{
	std::cerr << PROGNAME << ' ' << VERSION << ": " << msg << std::endl;
}

static void
print_usage()
{
	std::cerr << "Usage: " << PROGNAME
	          << " -l L -g G [--fast -s -d -p P -c C] FILE"
	             "[-o FILE] FILE...\n\n"
	             "  -l L        Use L as simulated read length size.\n"
	             "  -g G        Use G as genome size for span estimation.\n"
	             "  --fasta     Output in fasta format.\n"
	             "  -f FILE     Write estimated parameter to FILE, default is config.\n"
	             "  -s          Output estimate span parameter.\n"
	             "  -c C        Use C coverage to estimate span parameter. Default is 0.25.\n"
	             "  -d          Output estimate dist parameter.\n"
	             "  -p P        Use P percentile to estimate dist parameter. Default is 50.\n"
	             "  -v          Show verbose output.\n"
	             "  --help      Display this help and exit.\n"
	             "  --version   Display version and exit.\n"
	             "  FILE        Space separated list of FASTA/Q files."
	          << std::endl;
}

int
main(int argc, char* argv[])
{
	int c;
	int optindex = 0;
	int help = 0, version = 0;
	bool auto_span = false, auto_dist = false;
	size_t l = 0, g = 0;
	bool g_set = false;
	bool l_set = false;
	double cov_to_span = 0.25;      // Optimal tigmint-long span is 1/4 sequence coverage
	double dist_read_perc = 50;     // Optimal tigmint-long dist is read length p50
	size_t dist_lower_bound = 1000; // Lower bound for dist
	std::vector<size_t> read_lengths;
	size_t total_bases = 0;
	int with_fasta = 0;
	std::string outfile("-");
	std::string configFile("config");
	bool failed = false;
	static const struct option longopts[] = { { "fasta", no_argument, &with_fasta, 1 },
		                                      { "help", no_argument, &help, 1 },
		                                      { "version", no_argument, &version, 1 },
		                                      { nullptr, 0, nullptr, 0 } };
	while ((c = getopt_long(argc, argv, "l:g:o:c:p:sdf:", longopts, &optindex)) != -1) {
		switch (c) {
		case 0:
			break;
		case 'l':
			l_set = true;
			l = std::stoul(optarg);
			break;
		case 'g':
			g_set = true;
			g = std::stoul(optarg);
			break;
		case 'p':
			dist_read_perc = std::stod(optarg);
			break;
		case 'c':
			cov_to_span = std::stod(optarg);
			break;
		case 'o':
			outfile = optarg;
			break;
		case 'f':
			configFile = optarg;
			break;
		case 's':
			auto_span = true;
			break;
		case 'd':
			auto_dist = true;
			break;
		default:
			std::exit(EXIT_FAILURE);
		}
	}

	std::vector<std::string> infiles(&argv[optind], &argv[argc]);
	if (argc < 2) {
		print_usage();
		std::exit(EXIT_FAILURE);
	}
	if (help != 0) {
		print_usage();
		std::exit(EXIT_SUCCESS);
	} else if (version != 0) {
		std::cerr << PROGNAME << ' ' << VERSION << std::endl;
		std::exit(EXIT_SUCCESS);
	}
	if (!l_set) {
		print_error_msg("missing option -- 'l'");
		failed = true;
	} else if (l == 0) {
		print_error_msg("option has incorrect value -- 'l'");
		failed = true;
	}
	if (!g_set) {
		print_error_msg("missing option -- 'g'");
		failed = true;
	} else if (g == 0) {
		print_error_msg("option has incorrect value -- 'g'");
		failed = true;
	}
	if (infiles.empty()) {
		print_error_msg("missing file operand");
		failed = true;
	}
	if (failed) {
		std::cerr << "Try '" << PROGNAME << " --help' for more information.\n";
		std::exit(EXIT_FAILURE);
	}

	char header_symbol = '@';

	if (with_fasta) {
		header_symbol = '>';
	}

	for (auto& infile : infiles) {
		unsigned flags = 0;
		flags |= btllib::SeqReader::Flag::NO_FOLD_CASE; // skip time intensive checking
		flags |= btllib::SeqReader::Flag::NO_TRIM_MASKED;
		btllib::SeqReader reader(infile, flags, 6, 4, 1);
		btllib::SeqReader::Record record;
		while ((record = reader.read())) {
			size_t step = l * 2;
			std::string& seq = record.seq;
			std::string& qual = record.qual;
			size_t seq_size = seq.size();
			size_t qual_size = qual.size();
			if (step > seq_size) {
				continue;
			}
			int read_num = 1;
			for (size_t i = 0; i <= seq_size - step; i = i + step) {
				std::cout << header_symbol << record.name << "_f" << read_num
				          << " BX:Z:" << record.num + 1 << '\n';
				std::cout << seq.substr(i, l) << '\n';
				if (!with_fasta) {
					std::cout << "+\n";
					if (qual_size == 0) {
						std::cout << std::string(l, '#') << '\n';
					} else {
						std::cout << qual.substr(i, l) << '\n';
					}
				}
				std::string reverse_linked_read = seq.substr(i + l, l);
				btllib::reverse_complement(reverse_linked_read);
				std::cout << header_symbol << record.name << "_f" << read_num
				          << " BX:Z:" << record.num + 1 << '\n';
				std::cout << reverse_linked_read << '\n';
				if (!with_fasta) {
					std::cout << "+\n";
					if (qual_size == 0) {
						std::cout << std::string(l, '#') << '\n';
					} else {
						std::string reverse_qual = qual.substr(i + l, l);
						std::reverse(reverse_qual.begin(), reverse_qual.end());
						std::cout << reverse_qual << '\n';
					}
				}
				++read_num;
			}

			size_t remainder = seq_size % step;
			if (remainder != 0) {
				size_t curr_i = seq_size - remainder;
				std::cout << header_symbol << record.name << "_f" << read_num
				          << " BX:Z:" << record.num + 1 << '\n';
				std::string forward_linked_read = seq.substr(curr_i, l);
				std::cout << forward_linked_read << '\n';
				if (!with_fasta) {
					std::cout << "+\n";
					if (qual_size == 0) {
						std::cout << std::string(forward_linked_read.size(), '#') << '\n';
					} else {
						std::cout << qual.substr(curr_i, l) << '\n';
					}
				}
				std::string reverse_linked_read =
				    seq.substr(seq_size - forward_linked_read.size(), forward_linked_read.size());
				btllib::reverse_complement(reverse_linked_read);
				std::cout << header_symbol << record.name << "_f" << read_num
				          << " BX:Z:" << record.num + 1 << '\n';
				std::cout << reverse_linked_read << '\n';
				if (!with_fasta) {
					std::cout << "+\n";
					if (qual_size == 0) {
						std::cout << std::string(forward_linked_read.size(), '#') << '\n';
					} else {
						std::string reverse_qual = qual.substr(
						    seq_size - forward_linked_read.size(), forward_linked_read.size());
						std::reverse(reverse_qual.begin(), reverse_qual.end());
						std::cout << reverse_qual << '\n';
					}
				}
			}

			std::cout << std::flush;

			if (auto_dist) {
				if (seq_size > dist_lower_bound) {
					read_lengths.push_back(seq_size);
				}
			}
			if (auto_span) {
				total_bases += seq_size;
			}
		}
	}

	if (auto_span || auto_dist) {
		std::ofstream ofs(configFile, std::ofstream::out);
		if (auto_span) {
			ofs << "span\t" << (size_t)(total_bases / g * cov_to_span) << "\n";
		}

		if (auto_dist) {
			if (read_lengths.size() == 0) {
				std::cerr << "Psuedolr: unable to estimate dist parameter due to no valid lengths"
				          << std::endl;
			} else {
				size_t dist_estimate;
				std::sort(read_lengths.begin(), read_lengths.end());
				double index = (dist_read_perc / 100) * read_lengths.size();
				size_t size_t_index = (size_t)floor(index);
				if (floor(index) == index) {
					dist_estimate =
					    (read_lengths[size_t_index - 1] + read_lengths[size_t_index]) / 2;

				} else {
					dist_estimate = read_lengths[size_t_index];
				}
				ofs << "read_p" << dist_read_perc << "\t" << dist_estimate << "\n";
			}
		}

		ofs.close();
	}

	return 0;
}
