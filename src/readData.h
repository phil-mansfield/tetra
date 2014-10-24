#ifndef CACHE_H
#define CACHE_H

#include "simParameters.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <string.h>
#include "readData.h"
#include "bilinear.h"
#include "uba.h"

#define DF_A200 0
#define DF_C200 1
#define DF_C500 2

#define DF_CODE DF_A200

typedef struct {
    double mtWeight[PARENT_COUNT];
    double stWeight[PARENT_COUNT];
    int parents[PARENT_COUNT];
    int countContrib[PARENT_COUNT];
    double position[3];
    double mass;
    double radius;
    double stars;
    double gas;
    double sfr;
    double mDot;
    double daughterMass;
    double daughterST;
    double daughterMT;
    int daughterContrib;
    int count;
    int mainDaughter;
    int maxI;
} VerboseHalo;

typedef struct {
    double position[3];
    double mass;
    int parents[PARENT_COUNT];
    int countContrib[PARENT_COUNT];
} BriefHalo;

typedef struct {
    int count;
    float mass;
    float radius;
} DensityFormat;

typedef struct {
    float position[3];
    float velocity[3];
    DensityFormat A200;
    DensityFormat C200;
    DensityFormat C500;
    int parents[PARENT_COUNT];
    int countContrib[PARENT_COUNT];
    float mtWeight[PARENT_COUNT];
    float stWeight[PARENT_COUNT];
} ReadHalo;

VerboseHalo *readInput(char *fileName, int* sizePtr);
BriefHalo *readInputBrief(char *fileName, int* sizePtr);

double *readCube(char *fileName);

int getNumFiles(char *pwd);
char **getAllFiles(char *pwd, int fileCount);
double getRedshift(char *fileName);
void writeResults(double *massBins, int size, double binSize,
                  double z, char *dirName);
void twoVarWrite(double *var1Bins, double *var2Bins, int size, 
		 double binSize, double z, char *dirName);
void newFormatWrite(char *dirName, char *format, double z,
		    double *var1, double *var2, int size);
void newFormatSigmaWrite(char *dirName, char *format, double z,
			 double *var, int size, double simgas[size][5]);
InterpGrid *readIG(char *dirName, int zCount, double zs[zCount]);

void haloFree(VerboseHalo *H);

#endif
