#include "Image.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#define STBI_ONLY_PNG
#include "stb_image/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write/stb_image_write.h"


// Constructors

Image::Image(const std::string& filename)
{
	data_ = stbi_load(filename.c_str(), &width_, &height_, &channels_, 0);

	if (data_ == nullptr)
		throw std::runtime_error{stbi_failure_reason()};
}


// Resource management

Image::~Image() noexcept
{
	stbi_image_free(data_);
}


// Functions

void Image::write(const std::string& filename) const
{
	int success{0};

	if (data_ != nullptr)
		success = stbi_write_png(filename.c_str(), width_, height_, channels_, data_, 0);

	if (success == 0)
		throw std::runtime_error{"Could not write " + filename};
}


void Image::fillLinear(unsigned char threshold)
{
	if (channels_ != 2 && channels_ != 4)
		return;

	const int pixelCount{width_ * height_};
	const int alphaOffset{channels_ - 1};

	unsigned char* fillPtr{nullptr};
	int distance{0};

	for (int pixel{0}; pixel < pixelCount; ++pixel)
	{
		unsigned char* pixelPtr{data_ + pixel*channels_};

		if (pixelPtr[alphaOffset] > threshold)
		{
			fillPtr = pixelPtr;

			// Fill backward
			if (distance > 0)
			{
				const int x{pixel % width_};
				int backtrack{distance};

				// Minimize backtracking distance
				if (backtrack < pixel)
				{
					if (backtrack > x)
						backtrack = x; // Up to left edge
					else
						backtrack /= 2; // Halfway to previous visible pixel
				}

				for (int b{pixel-backtrack}; b < pixel; ++b)
					std::copy(fillPtr, fillPtr + alphaOffset, data_ + b*channels_);
			}

			distance = 0;
		}
		else
		{
			// Fill forward
			if (fillPtr != nullptr)
				std::copy(fillPtr, fillPtr + alphaOffset, pixelPtr);
			++distance;
		}
	}
}


void Image::fillRadial(unsigned char threshold)
{
	if (channels_ != 2 && channels_ != 4)
		return;

	static const int directDeltas[][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}};
	static const int diagonalDeltas[][2] = {{1,1}, {-1,-1}, {-1,1}, {1,-1}};

	const int pixelCount{width_ * height_};
	const int alphaOffset{channels_ - 1};

	// Buffer for smallest distance to visible pixel
	constexpr int infinity{std::numeric_limits<int>::max()};
	std::vector<int> distance(pixelCount, infinity);

	// List of pixels bordering established colors
	std::vector<int> border{};
	std::vector<int> borderNext{};

	// Prepare distance buffer and border
	for (int pixel{0}; pixel < pixelCount; ++pixel)
	{
		if (data_[pixel*channels_ + alphaOffset] > threshold)
		{
			// Visible pixel remains unchanged
			distance[pixel] = 0;

			// Add invisible neighbors to border
			const int x{pixel % width_};
			const int y{pixel / width_};
			for (auto& delta : directDeltas)
			{
				const int s{x + delta[0]};
				const int t{y + delta[1]};
				if (s >= 0 && s < width_ && t >= 0 && t < height_)
				{
					const int neighbor{s + t*width_};
					if (data_[neighbor*channels_ + alphaOffset] <= threshold &&
					    distance[neighbor] == infinity)
					{
						distance[neighbor] = 1;
						border.push_back(neighbor);
					}
				}
			}
		}
	}

	// Grow border outward
	for (int radius{1}; !border.empty(); ++radius)
	{
		for (int pixel : border)
		{
			const int x{pixel % width_};
			const int y{pixel / width_};
			unsigned char* fillPtr{nullptr};

			// Diagonal neighbors
			for (auto& delta : diagonalDeltas)
			{
				const int s{x + delta[0]};
				const int t{y + delta[1]};
				if (s >= 0 && s < width_ && t >= 0 && t < height_)
				{
					const int neighbor{s + t*width_};
					if (distance[neighbor] < radius-1)
					{
						// If a diagonal neighbor was established at
						// least two iterations ago, it is closer to
						// a visible pixel than any direct neighbor
						fillPtr = data_ + neighbor*channels_;
						break;
					}
				}
			}

			// Direct neighbors
			for (auto& delta : directDeltas)
			{
				const int s{x + delta[0]};
				const int t{y + delta[1]};
				if (s >= 0 && s < width_ && t >= 0 && t < height_)
				{
					const int neighbor{s + t*width_};
					if (distance[neighbor] < radius && fillPtr == nullptr)
					{
						// Use color of established neighbor
						fillPtr = data_ + neighbor*channels_;
					}
					else if (distance[neighbor] == infinity)
					{
						// Process invisible neighbor in next iteration
						distance[neighbor] = radius + 1;
						borderNext.push_back(neighbor);
					}
				}
			}

			std::copy(fillPtr, fillPtr + alphaOffset, data_ + pixel*channels_);
		}

		border.swap(borderNext);
		borderNext.clear();
	}
}
