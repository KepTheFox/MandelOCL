#include <iostream>
#include <fstream>
#include <complex>
#include <unistd.h>
#include "clFramework.h"
#include "mandelbrotUtils.h"

using namespace std;

// #define MAX_ITER 100

cl_int WIDTH = 1200;
cl_int HEIGHT = 1200;
cl_double ZOOM = 1;
cl_int MAX_ITER = 100;
int size = (int)WIDTH * (int)HEIGHT;

double X = -0.55;
double Y = -0.595;
double startWidth = 5;



int value(int x, int y){
    complex<float> point((float)x/WIDTH - 1.5, (float)y/HEIGHT - 0.5);

    complex<float> z(0, 0);
    unsigned int iter = 0;
    while(abs(z) < 2 && iter < MAX_ITER){
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

int runGPU(clInfo *cli, Window<int> scr, Window<double> fract){

    clDevice device = cli->clPlatforms[0].clDevices[0];

    
}

int main(){
    cl_int err;
    clInfo *cli = new clInfo;
    cli->loadKernelsFromFile("kernels/mandelbrot.cl");

    clDevice device = cli->clPlatforms[0].clDevices[0];

    cl_mem retBuffer = clCreateBuffer(device.context, CL_MEM_READ_WRITE, size * sizeof(int), NULL, &err);
    cl_mem _WINDOW = clCreateBuffer(device.context, CL_MEM_READ_WRITE, 4 * sizeof(int), NULL, &err);
    cl_mem _FRAME = clCreateBuffer(device.context, CL_MEM_READ_WRITE, 4 * sizeof(double), NULL, &err);

    cl_kernel valueKern = clCreateKernel(device.program, "value", &err);
    err = clSetKernelArg(valueKern, 0, sizeof(cl_mem), (void*)&retBuffer);
    err = clSetKernelArg(valueKern, 1, sizeof(cl_mem), (void*)&_WINDOW);
    err = clSetKernelArg(valueKern, 2, sizeof(cl_mem), (void*)&_FRAME);
    err = clSetKernelArg(valueKern, 3, sizeof(cl_double), (void*)&ZOOM);

    Window<int> scr(0, 1200, 0, 1200);
    // Window<double> fract(-2.2, 1.2, -1.7, 1.7);
    Window<double> fract(-1, 1, -1, 1);
    
    fract.move(X, Y);
    fract.zoom(ZOOM);
    while(fract.getZoom() < 65){
        fract.zoom(2);
        // int err = runGPU(cli, scr, fract);
        // std::cout << "ZOOM LEVEL: " << fract.getZoom() << '\n';
        printf("ZOOM LEVEL: %f\n", fract.getZoom());


        int WINDOW[] = {0, scr.xMax(), 0, scr.yMax()};
        double FRAME[] = {fract.xMin(), fract.xMax(), fract.yMin(), fract.yMax()};
            
        
        /*START GPU CODE*/
        clEnqueueWriteBuffer(device.commandQueue, _WINDOW, CL_TRUE, 0, 4 * sizeof(int),  WINDOW, 0, NULL, NULL);
        clEnqueueWriteBuffer(device.commandQueue, _FRAME, CL_TRUE, 0, 4 * sizeof(double),  FRAME, 0, NULL, NULL);
        err = clSetKernelArg(valueKern, 4, sizeof(cl_int), (void*)&MAX_ITER);

        ofstream myImage("mandelbrotGPU.ppm");

        int *retMem = new int[size];

        size_t global[1], local[1];
        global[0] = size;
        local[0] = 64;
        cl_event ev;

        if(myImage.is_open()){
            myImage << "P3\n" << (int)WIDTH << " " << (int)HEIGHT << " 255\n";

            /* Launch runs on GPU */
            err = clEnqueueNDRangeKernel(device.commandQueue, valueKern, 1, 0, global, local, 0, NULL, &ev);
            if (err != CL_SUCCESS)
                return err;
            clWaitForEvents(1, &ev);

            clFlush(device.commandQueue);
            clFinish(device.commandQueue);

            /* Copy results from the memory buffer */
            err = clEnqueueReadBuffer(device.commandQueue, retBuffer, CL_TRUE, 0, size * sizeof(int), retMem, 0, NULL, NULL);
            for(int i = 0; i < size; ++i){
                myImage << 0 << ' ' << retMem[i]/2 << ' ' << retMem[i] << "\n";
            }
            myImage.close();
        }
        else{
            cout << "Could not open file\n";
        }
        /*END GPU CODE*/

        MAX_ITER += 30;
        sleep(3);
    }
    return 0;
}