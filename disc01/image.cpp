#include "image.h"
#include "lodepng.h"


Image::Image() {
	width = height = 0;
}

Image::Image(const std::string& filename) {
	read(filename);
}

Image::Image(const unsigned& w, const unsigned& h)
: width(w), height(h), data(4 * w * h) {

}

void Image::read(std::string filename) {
	// FIXME
	data.clear();
	lodepng::decode(data, width, height, filename);
}

void Image::write(std::string filename) const {
	// FIXME
	lodepng::encode(filename, data, width, height);
}

uint8_t* Image::at(int x, int y) {
	// FIXME
	return &data[0] + 4*(y*width+x);
}

Image Image::operator*(const Filter& filter) {
	// FIXME
	Image ret=Image(this->width, this->height);
	// std::cout<<"output image created"<<std::endl;
	for(int offset=0;offset<4;++offset)
	{
		for (int i = 0; i < this->width; ++i)
		{
			for (int j = 0; j < this->height; ++j)
			{
				// printf("%d\t", *this->at(i,j));
				int w = filter.width, h = filter.height;
				int shift_i = 0, shift_j = 0;
				float sum = 0;
				for (int i_in = 0; i_in < w; ++i_in)
				{
					for (int j_in = 0; j_in < h; ++j_in)
					{
						// std::cout<<"before this->at"<<std::endl;
						shift_i = i - w / 2 + i_in;
						shift_j = j - h / 2 + j_in;
						if (shift_i >= 0 && shift_i < width && shift_j >= 0 && shift_j < height)
						{
							int r = *(this->at(shift_i, shift_j) + offset);
							sum += filter.at(i_in, j_in) * r;
						}
					}
				}
				ret.at(i, j)[offset] = (uint8_t)sum;
			}
		}
	}
	return ret;
}