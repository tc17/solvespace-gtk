
#ifndef __SOLVESPACE_H
#define __SOLVESPACE_H

// Debugging functions
#define oops() do { dbp("oops at line %d, file %s", __LINE__, __FILE__); \
                                                        exit(-1); } while(0)
#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif
#ifndef max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif

void dbp(char *str, ...);
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "dsc.h"
#include "ui.h"
#include "sketch.h"

void Invalidate(void);



#define arraylen(x) (sizeof((x))/sizeof((x)[0]))
#define PI (3.1415926535897931)
void MakeMatrix(double *mat, double a11, double a12, double a13, double a14,
                             double a21, double a22, double a23, double a24,
                             double a31, double a32, double a33, double a34,
                             double a41, double a42, double a43, double a44);


class SolveSpace {
public:
    TextWindow                  TW;
    GraphicsWindow              GW;

    IdList<Request,hRequest>    req;
    IdList<Entity,hEntity>      entity;
    IdList<Point,hPoint>        point;
    IdList<Param,hParam>        param;

    void Init(void);
};

extern SolveSpace SS;

#endif