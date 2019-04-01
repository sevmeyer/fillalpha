#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

#include "minarg/minarg.hpp"

#include "Image.hpp"


constexpr char version[] = "1.0.0";


int main(int argc, char* argv[])
{
	bool isLinear{false};
	unsigned char threshold{0};
	std::string source{};
	std::string target{};

	minarg::Parser parser{
		"This tool is intended for images with an alpha channel.\n"
		"To reduce color bleeding, it grows visible colors into\n"
		"areas where the alpha value is below a given threshold.\n"
		"Reads and writes ordinary PNG images, metadata is ignored."};

	parser.addOption(isLinear, 'l', "linear",
		"Only grow visible colors horizontally,\n"
		"for the most repetitive byte patterns");

	parser.addOption(threshold, 't', "threshold", "ALPHA",
		"Alpha values up to this threshold\n"
		"are considered invisible");

	parser.addSignal('h', "help", "Print help and exit");
	parser.addSignal('v', "version", "Print version and exit");

	parser.addOperand(source, "SOURCE", "Source PNG image", true);
	parser.addOperand(target, "TARGET", "Target PNG image, overwrites existing", true);

	try
	{
		parser.parse(argc, argv);
		Image image{source};

		if (isLinear)
			image.fillLinear(threshold);
		else
			image.fillRadial(threshold);

		image.write(target);
	}
	catch (const minarg::Signal& s)
	{
		if (s.shortName == 'v')
			std::cout << version << std::endl;
		else
			std::cout << parser;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
