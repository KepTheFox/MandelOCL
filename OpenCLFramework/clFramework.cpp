#include "clFramework.h"

/*Buffer Functions*/




/*Kernel Functions*/


/*device functions*/
void clDevice::initDevice(cl_device_id did){
    cl_int err;
    this->id = did;
    err |= clGetDeviceInfo(this->id, CL_DEVICE_AVAILABLE, sizeof(this->deviceAvailable), (void *)&(this->deviceAvailable), NULL);
    err |= clGetDeviceInfo(this->id, CL_DEVICE_COMPILER_AVAILABLE, sizeof(this->deviceCompilerAvailable), (void *)&(this->deviceCompilerAvailable), NULL);
    err |= clGetDeviceInfo(this->id, CL_DEVICE_EXTENSIONS, sizeof(this->deviceExtensions), (void *)&(this->deviceExtensions), NULL);
    err |= clGetDeviceInfo(this->id, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(this->deviceGlobalMemSize), (void *)&(this->deviceGlobalMemSize), NULL);
    err |= clGetDeviceInfo(this->id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(this->deviceWorkgroupSize), (void *)&(this->deviceWorkgroupSize), NULL);
    err |= clGetDeviceInfo(this->id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(this->deviceMaxComputeUnits), (void *)&(this->deviceMaxComputeUnits), NULL);
    err |= clGetDeviceInfo(this->id, CL_DEVICE_NAME, sizeof(this->deviceName), (void *)&(this->deviceName), NULL);
    err |= clGetDeviceInfo(this->id, CL_DEVICE_VENDOR, sizeof(this->deviceVendor), (void *)&(this->deviceVendor), NULL);

    /* Create OpenCL context */
    this->context = clCreateContext(NULL, 1, &(this->id), NULL, NULL, &err);
    
    /* Create Command Queue */
    this->commandQueue = clCreateCommandQueue(this->context, this->id, 0, &err);
}

void clDevice::printDeviceInfo(){
    printf("-------------------------------------\n");
    printf("DEVICE NAME:\t\t%s\n", this->deviceName);
    printf("DEVICE VENDOR: \t\t%s\n", this->deviceVendor);
    printf("MAX WORKGROUP SIZE:\t%ld\n", this->deviceWorkgroupSize);
    printf("MAX COMPUTE UNITS:\t%d\n", this->deviceMaxComputeUnits);
    printf("MAX DEVICE THREADS:\t%d\n", this->deviceMaxComputeUnits * 64);
    printf("GLOBAL MEM SIZE:\t%ldMiB\n", this->deviceGlobalMemSize/(1024*1024));
    printf("DEVICE AVAILABLE:\t%s\n", this->deviceAvailable ? "true" : "false");
    printf("-------------------------------------\n");
}

void clDevice::loadKernelsFromFile(char *filename){
    cl_int err;
    FILE *fp;
    int MAX_SOURCE_SIZE = 1000000;
    fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernels.\n");
        exit(1);
    }
    char *source_str = new char[MAX_SOURCE_SIZE];
    int source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    // printf("Source size: %d\n", source_size);

    // printf("SOURCE:\n%s\n", source_str);

    this->program = clCreateProgramWithSource(this->context, 1, (const char **)&source_str, NULL, NULL);
        
    /* Build Kernel Program */
    err |= clBuildProgram(this->program, 1, &(this->id), NULL, NULL, NULL);

    delete[] source_str;
}

int clDevice::createBuffer(char *name, size_t size){
    cl_int err;
    this->buffers.insert(std::pair<char *, cl_mem>(name, clCreateBuffer(this->context, CL_MEM_READ_WRITE, size, NULL, &err)));
    return (int)err;
}

cl_mem *clDevice::getBuffer(char *name){
    std::map<char *, cl_mem>::iterator it = this->buffers.find(name);
    if(it != this->buffers.end()){
        return &(it->second);
    }
    else{
        return NULL;
    }
}



int clDevice::createKernel(char *name){
    cl_int err;
    this->kernels.insert(std::pair<char *, cl_kernel> (name, clCreateKernel(this->program, name, &err)));
    return (int)err;
}

cl_kernel *clDevice::getKernel(char *name){
    std::map<char *, cl_kernel>::iterator it = this->kernels.find(name);
    if(it != this->kernels.end()){
        return &(it->second);
    }
    else{
        return NULL;
    }
}

int clDevice::setKernelArg(char *kernelName, char *bufferName, int argNum){
    cl_int err;
    cl_kernel *kern = this->getKernel(kernelName);
    err = clSetKernelArg(*kern, argNum, sizeof(cl_mem), (void*)this->getBuffer(bufferName));
    return (int)err;
}

int clDevice::enqueueWriteBuffer(char *name, size_t size, void *data, bool blocking){
    cl_int err;
    err = clEnqueueWriteBuffer(this->commandQueue, *(this->getBuffer(name)), blocking, 0, size,  data, 0, NULL, NULL);
    return err;
}

int clDevice::enqueueReadBuffer(char *name, size_t size, void *data, bool blocking){
    cl_int err;
    err = clEnqueueReadBuffer(this->commandQueue, *(this->getBuffer(name)), blocking, 0, size,  data, 0, NULL, NULL);
    return err;
}

int clDevice::enqueueNDRangeKernel(char *name, size_t *global, size_t *local){
    cl_int err;
    err = clEnqueueNDRangeKernel(this->commandQueue, *(this->getKernel(name)), 1, 0, global, local, 0, NULL, &(this->ev));
    return err;
}





/*platform functions*/
clPlatform::clPlatform(){
}

void clPlatform::initPlatform(cl_platform_id pid){
    /*Init Platform Here*/
    cl_int err;
    this->id = pid;
    err |= clGetPlatformInfo(this->id, CL_PLATFORM_PROFILE, sizeof(this->platformProfile), this->platformProfile, NULL);
    err |= clGetPlatformInfo(this->id, CL_PLATFORM_VERSION, sizeof(this->platformVersion), this->platformVersion, NULL);
    err |= clGetPlatformInfo(this->id, CL_PLATFORM_NAME, sizeof(this->platformName), this->platformName, NULL);
    err |= clGetPlatformInfo(this->id, CL_PLATFORM_VENDOR, sizeof(this->platformVendor), this->platformVendor, NULL);
    err |= clGetPlatformInfo(this->id, CL_PLATFORM_EXTENSIONS, sizeof(this->platformExtensions), this->platformExtensions, NULL);
    this->getDevices();
}

void clPlatform::printPlatformInfo(){
    printf("========= PLATFORM =========\n");
    printf("-------------------------------------\n");
    printf("PLATFORM PROFILE:\t%s\n", this->platformProfile);
    printf("PLATFORM VERSION:\t%s\n", this->platformVersion);
    printf("PLATFORM NAME:\t\t%s\n", this->platformName);
    printf("PLATFORM VENDOR:\t%s\n", this->platformVendor);
    printf("PLATFORM EXTENSIONS:\t%s\n", this->platformExtensions);
    printf("-------------------------------------\n");
    printf("\n");
    printf("======= DEVICE(S): %d =======\n", this->numDevices);
    for(int i = 0; i < this->numDevices; ++i){
        this->clDevices[i].printDeviceInfo();
    }
}

void clPlatform::getDevices(){
    cl_int err;

    err |= clGetDeviceIDs(this->id, CL_DEVICE_TYPE_ALL, 0, NULL, &(this->numDevices));
    // printf("Found %d platform(s)\n", this->numPlatforms);

    cl_device_id *tmp_cl_devices = new cl_device_id[this->numDevices];
    err |= clGetDeviceIDs(this->id, CL_DEVICE_TYPE_ALL, this->numDevices, tmp_cl_devices, NULL);

    this->clDevices = new clDevice[this->numDevices];

    for(int i = 0; i < this->numDevices; ++i){
        this->clDevices[i].initDevice(tmp_cl_devices[i]);
    }
}

void clPlatform::loadKernelsFromFile(char *filename){
    for(int i = 0; i < this->numDevices; ++i){
        this->clDevices[i].loadKernelsFromFile(filename);
    }
}

clDevice *clPlatform::getFirstAvailableDevice(){
    for(int i = 0; i < this->numDevices; ++i){
        if(this->clDevices[i].checkAvailable()){
            return &(this->clDevices[i]);
        }
    }
}




/*info functions*/
clInfo::clInfo(){
    cl_int err;

    err |= clGetPlatformIDs(0, NULL, &(this->numPlatforms));
    // printf("Found %d platform(s)\n", this->numPlatforms);

    cl_platform_id *tmp_cl_platforms = new cl_platform_id[this->numPlatforms];
    err |= clGetPlatformIDs(this->numPlatforms, tmp_cl_platforms, NULL);

    this->clPlatforms = new clPlatform[this->numPlatforms];

    for(int i = 0; i < this->numPlatforms; ++i){
        this->clPlatforms[i].initPlatform(tmp_cl_platforms[i]);
    }
}

void clInfo::printInfo(){
    for(int i = 0; i < this->numPlatforms; ++i){
        this->clPlatforms[i].printPlatformInfo();
    }
}

void clInfo::loadKernelsFromFile(char *filename){
    for(int i = 0; i < this->numPlatforms; ++i){
        this->clPlatforms[i].loadKernelsFromFile(filename);
    }
}

clDevice *clInfo::getFirstAvailableDevice(){
    for(int i = 0; i < this->numPlatforms; ++i){
        clDevice *foundDev = this->clPlatforms[i].getFirstAvailableDevice();
        if(foundDev){
            return foundDev;
        }
    }
    return NULL;
}
