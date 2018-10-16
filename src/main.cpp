#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <getopt.h>

/* Flag set by '--verbose' */
static int verbose_flag;

#include <memory>
#include <boost/filesystem.hpp>
#include "../include/ScanImageTiff.h"
#include "../include/write_tiff.h"

void printhelp() {
	std::cout << "\nA command-line utility for splitting tiff files recorded with ScanImage.\n";
	std::cout << "Can handle files written in the bigTIFF format and saves all header information.\n";
	std::cout << "Usage:\n";
	std::cout << "\t-f :  required - the input tiff file to split\n";
	std::cout << "\t-c :  chunks - the number of frames (default 5000) in each part of the split files\n";
	std::cout << "\t-s :  the output file base name\n";
	std::cout << "\t-h :  prints this message\n";
	std::cout << "\n\tExample:\n";
	std::cout << "\n\tTiffSplitter -f /home/robin/my_big_file.tif -c 10000 -s /home/robin/my_smaller_tiffs\n";
	std::cout << "\n\tThis will take the my_big_file.tif and split it into some number of other files called:\n";
	std::cout << "\t\tmy_smaller_tiffs_part0.tif\n";
	std::cout << "\t\tmy_smaller_tiffs_part1.tif\n";
	std::cout << "\t\tmy_smaller_tiffs_part2.tif\n";
	std::cout << "\t\tetc...\n";
	std::cout << "\n\tEach file will consist of 10000 frames, possibly except the last one which contains the remainder.\n";
	std::cout << "\n\tIf no output file name is supplied then the output files are named after the input.\n";
	std::cout << "\tIn the above example they would look like:\n";
	std::cout << "\t\tmy_big_file_part0.tif\n";
	std::cout << "\t\tmy_big_file_part1.tif\n";
	std::cout << "\t\tmy_big_file_part2.tif\n";
	std::cout << "\t\tetc...\n\n";
	exit(0);
}

int main(int argc, char **argv)
{
	std::string inputfile;
	std::string outputfile_base;
	int chunk_size = 5000;

	int c;

	while (1) {
		static struct option long_options[] = {
			/* These options set a flag */
			{"verbose", no_argument, &verbose_flag, 1},
			{"brief", no_argument, &verbose_flag, 0},
			/* These options don't set a flag
			They are distinguished by their indices*/
			{"help", no_argument, 0, 'h'},
			{"file", required_argument, 0, 'f'},
			{"chunks", no_argument, 0, 'c'},
			{"savefile", required_argument, 0, 's'},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here */
		int option_index = 0;
		c = getopt_long(argc, argv, "hf:c:s:", long_options, &option_index);
		/* Detect the end of the options */
		if ( c == -1 )
			break;
		switch (c) {
			case 0:
				if ( long_options[option_index].flag != 0 )
					break;
				break;
			case 'h':
				printhelp();
				exit(0);
			case 'f':
				inputfile = std::string(optarg);
				break;
			case 'c':
				chunk_size = atoi(optarg);
				break;
			case 's':
				outputfile_base = std::string(optarg);
				break;
			default:
				abort();
		}
	}
	/*
	Instead of reporting '--verbose' and '--brief' as
	they are encountered, we report the finals status
	resulting from them
	*/
	// if ( verbose_flag )
	// 	std::cout << "verbose_flag is set\n";
	/*
	Print any remaining command line args (not options)
	*/
	// if ( optind < argc ) {
	// 	std::cout << "non-option ARGV elements:\n";
	// 	while ( optind < argc )
	// 		std::cout << argv[optind++] << std::endl;
	// }
	// Check the input file exists
	if ( ! boost::filesystem::exists(inputfile) ) {
		std::cout << "Input tiff file does not exist, so exiting\n";
		exit(1);
	}
	// Create an outfile base name if one hasn't been supplied
	if ( outputfile_base.empty() ) {
		std::cout << "Output file is empty so naming files after input file like" << std::endl;
		std::vector<std::string> base;
		split(inputfile, '.', base);
		std::cout << (base[0] + "_part0.tiff") << std::endl;
		outputfile_base = base[0];
	}
	// Create a file reader and count the number of directories (frames) in the tiff file
	// NB the counting could be skipped
	std::unique_ptr<SITiffReader> reader = std::make_unique<SITiffReader>(inputfile);
	if ( ! reader->open() ) {
		std::cout << "Could not open tiff file, so exiting\n";
		exit(1);
	}
	int count = 0;
	/*
	libtiff has a maximum limit of 65535 frames per tiff file and throws an error if the number
	of frames is equal to or greater than this number - need to account for that here and sensibly
	traverse the file and save it...
	*/
	std::cout << "Counting directories in this tiff file (may take a while)..." << std::endl;
	reader->countDirectories(count);
	if ( count >= 65535) {
		std::cout << "\nWARNING: This file contains more than 65535 frames.\n";
		std::cout << "This means I can only save the first 65535 frames," << std::endl;
		std::cout << "i.e. the total number of frames from all chunks will equal 65535" << std::endl;
	}
	else
		std::cout << "There are " << count << " frames in this tiff file" << std::endl;
	std::string software_tag;
	std::string image_tag;
	cv::TiffWriter writer;
	cv::Mat frame;
	int tiff_part_num = 0;
	for (int i = 0; i < count; ++i) {
		if ( (i % chunk_size) == 0 ) {
			std::string fname = outputfile_base + "_part" + std::to_string(tiff_part_num++) + ".tif";
			std::cout << "Writing to " << fname << std::endl;
			if ( writer.isOpened() )
				writer.close();
			writer.open(fname);
		}
		software_tag = reader->getSWTag(i);
		image_tag = reader->getImDescTag(i);
		frame = reader->readframe(i);
		writer.writeSIHdr(software_tag, image_tag);
		writer << frame;
	}
	exit(0);
}