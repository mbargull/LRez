#include "subcommands/queryBam.h"
#include "subcommands/help.h"
#include "alignmentsRetrieval.h"
#include "indexManagementBam.h"
#include <getopt.h>

void subcommandQueryBam(int argc, char* argv[]) {
	string bamFile;
	string indexFile;
	string query;
	string list;
	string outputFile;
	BarcodesOffsetsIndex BarcodesOffsetsIndex;
	BamReader reader;
	ofstream out;

	const struct option longopts[] = {
		{"bam",				required_argument,	0, 'b'},
		{"index",			required_argument,	0, 'i'},
		{"query",			required_argument,	0, 'q'},
		{"list",			required_argument,	0, 'l'},
		{"output",			required_argument,	0, 'o'},
		{"userx",			no_argument,		0, 'u'},
		{0, 0, 0, 0},
	};
	int index;
	int iarg = 0;

	iarg = getopt_long(argc, argv, "b:i:q:l:o:u", longopts, &index);
	if (iarg == -1) {
		subcommandHelp("query bam");
	}
	while (iarg != -1) {
		switch (iarg) {
			case 'b':
				bamFile = optarg;
				break;
			case 'i':
				indexFile = optarg;
				break;
			case 'q':
				query = optarg;
				break;
			case 'l':
				list = optarg;
				break;
			case 'o':
				outputFile = optarg;
				break;
			case 'u':
				CONSIDER_RX = true;
				break;
			default:
				subcommandHelp("query bam");
				break;
		}
		iarg = getopt_long(argc, argv, "b:i:q:l:o:u", longopts, &index);
	}

	if (bamFile.empty()) {
		fprintf(stderr, "Please specify a BAM file with option --bam/-b.\n");
		exit(EXIT_FAILURE);
	}
	if (indexFile.empty()) {
		fprintf(stderr, "Please specify an index file with option --index/-i.\n");
		exit(EXIT_FAILURE);
	}
	if (query.empty() and list.empty()) {
		fprintf(stderr, "Please specify a query barcode with option --query/-q or a list of barcodes with option --list/-l.\n");
		exit(EXIT_FAILURE);
	}
	if (!query.empty() and !list.empty()) {
		fprintf(stderr, "Options --query/-q and --list/-l cannot be set at the same time.\n");
		exit(EXIT_FAILURE);
	}


	BarcodesOffsetsIndex = loadBarcodesOffsetsIndex(indexFile);
	vector<BamAlignment> alignments;
	if (!query.empty()) {
		alignments = retrieveAlignmentsWithBarcode(bamFile, BarcodesOffsetsIndex, query);
	} else {
		alignments = retrieveAlignmentsWithBarcodes(bamFile, BarcodesOffsetsIndex, list);
	}

	if (!outputFile.empty()) {
		out.open(outputFile, ios::out);
		if (!out.is_open()) {
			fprintf(stderr, "Unable to open file %s.", outputFile.c_str());
			exit(EXIT_FAILURE);
		}
		for (BamAlignment al : alignments) {
			out << convertToSam(al, reader.GetReferenceData()) << endl;
		}
		out.close();
	} else {
		for (BamAlignment al : alignments) {
			cout << convertToSam(al, reader.GetReferenceData()) << endl;
		}
	}
}