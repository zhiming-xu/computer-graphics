#include "filter.h"


Filter::Filter() {
	width = height = 1;
	kernel = {1.f};
}

Filter::Filter(const std::string& filename) {
	read(filename);
}

void Filter::read(std::string filename) {
    float tmp;
    std::ifstream rd(filename);
	rd>>this->width>>this->height;
	for(int i=0;i<this->height;++i)
	    for(int j=0;j<this->width;++j) {
			rd >> tmp;
			this->kernel.push_back(tmp);
		}
	// FIXME
}

void Filter::normalize() {
	// FIXME
	float sum=0;
	for(auto i=kernel.begin();i!=kernel.end();++i)
		sum+=*i;
	for(auto i=kernel.begin();i!=kernel.end();++i)
		*i=*i/sum;
}

const float& Filter::at(int x, int y) const {
	// FIXME
	return kernel[y*width+x];
}

