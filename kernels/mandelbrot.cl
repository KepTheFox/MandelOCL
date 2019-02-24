
typedef float2 cfloat;

inline float real(cfloat a){
    return a.x;
}

inline float imag(cfloat a){
    return a.y;
}

inline float cmod(cfloat a){
    return (sqrt(a.x * a.x + a.y * a.y));
}

inline cfloat cmult(cfloat a, cfloat b){
    return (cfloat)(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

inline cfloat cadd(cfloat a, cfloat b){
    return (cfloat)(a.x + b.x, a.y + b.y);
}

__kernel void value(__global int *ret, __constant int* WINDOW, __constant double* FRAME, double ZOOM, int MAX_ITER){
    uint g = (uint) get_global_id(0);

    int x = g % WINDOW[1];
    int y = g / WINDOW[1];
    double FRAME_WIDTH = FRAME[1] - FRAME[0];
    double FRAME_HEIGHT = FRAME[3] - FRAME[2];
    
    cfloat point = (cfloat)((float)x/WINDOW[1] * FRAME_WIDTH + FRAME[0], (float)y/WINDOW[3] * FRAME_HEIGHT + FRAME[2]);
    cfloat z = (cfloat)(0, 0);
    int i = 0;
    while((cmod(z) < 2) && (i < MAX_ITER)){
        z = cadd(cmult(z, z), point);
        ++i;
    }

    if(i < MAX_ITER){
        ret[g] = (255 * i)/(MAX_ITER - 1);
    }
    else{
        ret[g] = 0;
    }
    // ret[g] = cmod(z)*1000;
};