#include <iostream>
#include <fstream>
#include <complex>

using namespace std;

#define MAX_ITER 100

float width = 1200;
float height = 1200;


int value(int x, int y){
    complex<float> point((float)x/width - 1.5, (float)y/height - 0.5);

    complex<float> z(0, 0);
    unsigned int iter = 0;
    while(abs(z) < 2 && iter <= MAX_ITER){
        z = z * z + point;
        ++iter;
    }
    if(iter < MAX_ITER){
        return (255 * iter)/(MAX_ITER - 1);
    }
    else{
        return 0;
    }
}

int main(){
    ofstream myImage("mandelbrot.ppm");
    if(myImage.is_open()){
        myImage << "P3\n" << width << " " << height << " 255\n";
        for(int i = 0; i < width; ++i){
            for(int j = 0; j < height; ++j){
                int val = value(i, j);
                myImage << val << ' ' << 0 << ' ' << 0 << "\n";
            }
        }
        myImage.close();
    }
    else{
        cout << "Could not open file\n";
    }
    return 0;
}