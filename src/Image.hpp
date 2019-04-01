#pragma once

#include <string>


// This class uses int for image dimensions and byte count,
// to match stb_image. While size_t would be more correct,
// it turns into a casting mess for signed 2D coordinates.
// Therefore, on systems with the usual 32bit integers,
// images are limited to about 536MP (2GiB).


class Image
{
	public:

		// Constructors

		explicit Image(const std::string& filename);

		// Resource management

		~Image() noexcept;
		Image(const Image& other) = delete;
		Image& operator=(const Image& other) = delete;

		// Functions

		void write(const std::string& filename) const;
		void fillLinear(unsigned char threshold);
		void fillRadial(unsigned char threshold);

	private:

		int width_{0};
		int height_{0};
		int channels_{0};

		unsigned char* data_{nullptr};
};
