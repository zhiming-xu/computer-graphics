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
	lodepng::decode(data, width, height, filename);
}

void Image::write(std::string filename) const {
	// FIXME
	lodepng::encode(filename, data, width, height);
}

uint8_t* Image::at(int x, int y) {
	// FIXME
	return &data[4*(y*width+x)];
}

Image Image::operator*(const Filter& filter) {
	// FIXME
	for(int i=0;i<this->width;++i)
		for(int j=0;j<this->height;++j)
		{
			int result=0;
			if(i+1<width)
				result+=filter.at(i+1, j)**(this->at(i, j));
			if(i-1>0)
				result+=filter.at(i-1, j)**(this->at(i, j));
			if(j+1<height)
				result+=filter.at(i, j+1)**(this->at(i, j));
			if(j-1<height)
				result+=filter.at(i, j-1)**(this->at(i, j));
			*(this->at(i, j))=result;
		}
}