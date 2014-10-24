#include "readData.h"
#include "arrayOps.h"
#include <assert.h>
#include <time.h>

/*Removes the elements of H which are not in the verbose halo format
 *and retains the needed ones in V.*/
inline void convertReadHalo(ReadHalo *H, VerboseHalo *V) {
    unsigned int i;

    switch(DF_CODE) {
    case DF_A200:
        V->mass = H->A200.mass;
        V->count = H->A200.count;
        V->radius = H->A200.radius;
        break;
    case DF_C200:
        V->mass = H->C200.mass;
        V->count = H->C200.count;
        V->radius = H->C200.radius;
        break;
    case DF_C500:
        V->mass = H->C500.mass;
        V->count = H->C500.count;
        V->radius = H->C500.radius;
        break;
    }
    V->stars = 0.0;
    V->sfr = 0.0;
    V->gas = 0.0;
    V->mDot = 0.0;
    V->daughterMass = 0;
    V->daughterContrib = 0;
    V->daughterMT = 0;
    V->daughterST = 0;
    V->maxI = 0;
    V->mainDaughter = -1;

    for(i = 0; i < PARENT_COUNT; i++) {
        V->countContrib[i] = H->countContrib[i];
        V->mtWeight[i] = H->mtWeight[i];
        V->stWeight[i] = H->stWeight[i];
        /*FORTRAN is the devil, FORTRAN is the devil.*/
        V->parents[i] = H->parents[i] - 1;
    }
    for(i = 0; i < 3; i++) V->position[i] = H->position[i];
}

void removeExcess(ReadHalo *H, VerboseHalo *VH, int haloCount) {
    int i;
    for(i = 0; i < haloCount; i++) convertReadHalo(H + i, VH + i);
    return;
}

/*readOutput : (string, int*) -> Halo*
 *
 *[readOutput(fileName, sizePtr)] reads in a binary file of properly
 *formatted halos are returns a corresponding Halo array. The contents
 *of sizePtr will change to the number of Halos read in.
 *
 *Terminates upon IO error.*/
VerboseHalo *readInput(char *fileName, int *sizePtr) {
    FILE *fp;
    long long haloCount;
    ReadHalo *H;
    VerboseHalo *VH;
    int toRead, readSoFar;
    
    if(!(fp = fopen(fileName, "r"))) {
        fprintf(stderr, "Cannot open %s\n", fileName);
        exit(1);
    }
    
    if(!fread(&haloCount, sizeof(long long), 1, fp)) {
        fprintf(stderr, "Encountered problem reading file size of %s\n",
                fileName);
        exit(1);
    }
    /*malloc all the VerboseHalos first.*/
    VH = malloc(sizeof(VerboseHalo) * haloCount);
    if(!VH) {
        fprintf(stderr, "Could not malloc VH\n");
        exit(1);
    }
    
    if(BREAK_UP_READ) {
        /*This should save memory.*/
        readSoFar = 0;
        H = (ReadHalo*) malloc(sizeof(ReadHalo) * BREAK_UP_SIZE);
        if(!H) {
            fprintf(stderr, "readHalo could not malloc H\n");
            exit(1);
        }
        /*Keep reading pieces of the binary file.*/
        while(readSoFar < haloCount) {
            if(haloCount - readSoFar < BREAK_UP_SIZE) {
                toRead = haloCount - readSoFar;
            } else {
                toRead = BREAK_UP_SIZE;
            }
            if((unsigned int)haloCount < 
               fread(H, sizeof(ReadHalo), toRead, fp)) {
                fprintf(stderr, "Could not read data from %s\n",
                        fileName);
                exit(1);
            }   
            removeExcess(H, VH + readSoFar, toRead);
            readSoFar += toRead;
        }
        
        
    } else {
        H = (ReadHalo*) malloc(sizeof(ReadHalo) * haloCount);
        if(!H) {
            fprintf(stderr, "readHalo could not malloc H\n");
            exit(1);
        }
        
        if((unsigned int)haloCount < 
           fread(H, sizeof(ReadHalo), haloCount, fp)) {
            fprintf(stderr, "Encountered problem reading data of %s\n",
                    fileName);
            exit(1);
        }
        
        removeExcess(H, VH, haloCount);
    }
    
    free(H);
    fclose(fp);
    *sizePtr = haloCount;
    return VH;
}

double *readCube(char* fileName) {
    FILE *fp;
    float *cube;
    double *dCube;
    unsigned int cellCount, i;
    
    if(!(fp = fopen(fileName, "r"))) {
        fprintf(stderr, "Cannot open %s\n", fileName);
        exit(1);
    }
    
    cellCount = NG * NG * NG;
    cube = (float*) malloc(sizeof(float) * cellCount);
    dCube = (double*) malloc(sizeof(double) * cellCount);
    if(!(cube && dCube)) {
        fprintf(stderr, "readCube could not malloc cubes.\n");
    }
    
    if(cellCount < fread(cube, sizeof(float), cellCount, fp)) {
        fprintf(stderr, "Encountered problem reading data of %s.\n",
                fileName);
        exit(1);
    }
    /*Our array ops require doubles, not floats. Also, I feel dirty
     *using an unsigned int as an iterator.*/
    for(i = 0; i < cellCount; i++) dCube[i] = cube[i];
    free(cube);
    fclose(fp);
    return dCube;
}

/*getNumFiles : string -> int
 *
 *[getNumFiles(pwd)] takes the path of the target directory and
 *returns the number of files/directories in it, not including
 *pwd/. and pwd/..
 *
 *Terminates upon IO error.*/
int getNumFiles(char *pwd) {
    DIR *dp;
    int n;

    if(!(dp = opendir(pwd))) {
        fprintf(stderr, "Encountered problem opening %s.\n", pwd);
    }
    
    n = 0;
    while(readdir(dp)) n++;
    
    if(closedir(dp)) {
        fprintf(stderr, "Encountered problem closing %s.\n", pwd);
    }
    
    /*ignores . and ..*/
    return n - 2;
}

/*Takes a file name and returns the redshift that it corresponds to.*/
double getRedshift(char *fileName) {
    double z;
    int len, zLen;
    char zString[FILENAME_MAX];
    
    len = strlen(fileName);
    zLen = 7;
    
    strcpy(zString, fileName + (len - zLen));
    sscanf(zString, "%lf", &z);
    return z;
}

/*Compares the redshift of two file names.*/
static int strpcmp(const void *p1, const void *p2){
    float n1, n2;
    /* The actual arguments to this function are "pointers to
       pointers to char", but strcmp() arguments are "pointers
       to char", hence the following cast plus dereference */
    n1 = getRedshift(*(char *const*)p1);
    n2 = getRedshift(*(char *const*)p2);
    
    if(n1 > n2) {
        return 1;
    } else if(n1 < n2) {
        return -1;
    }
    return 0;
}


/*getAllFiles : (string, int) -> string*
 *[getAllFiles(pwd, fileCount)] returns an array of strings corresponding
 *to the first fileCount files in pwd. This does not include pwd/. and
 *pwd/..
 *
 *Terminates on IO errors and explodes spectacularly if fileCount is too
 *large.*/
char **getAllFiles(char *pwd, int fileCount) {
    DIR *dp;
    struct dirent *entry;
    char **fileNames;
    int i;
    
    if(!(dp = opendir(pwd))) {
        fprintf(stderr, "Encountered problem opening %s.\n", pwd);
    }
    
    fileNames = malloc(sizeof(char*) * fileCount);
    if(! fileNames) {
        fprintf(stderr, "malloc has failed. You sorry son of a bitch.\n");
        exit(1);
    }
    /*readdir will also read in . and ..*/
    for(i = 0; i < 2; i++) readdir(dp);
    
    for(i = 0; i < fileCount; i++) {
        entry = readdir(dp);
        fileNames[i] = malloc(sizeof(char) * FILENAME_MAX);
        if(! fileNames[i]) {
            fprintf(stderr, "fileNames[%d] could not be malloced\n", i);
        }
        sprintf(fileNames[i], "%s%s", pwd, entry->d_name);
    }
    
    if(closedir(dp)) {
        fprintf(stderr, "Encountered problem closing %s.\n", pwd);
    }
    
    qsort(fileNames, fileCount, sizeof(char*), strpcmp);
    return fileNames;
}



/*prints a file  in dirName with the name z which is formatted as:
 *binsize\n
 *massBins[0] massBins[1] ...\n*/
void writeResults(double *massBins, int size, double binSize, 
                  double z, char *dirName) {
    FILE *fp;
    char fileName[FILENAME_MAX];
    int i;
    
    sprintf(fileName, "%s/%lf", dirName, z);
    fp = fopen(fileName, "w");
    
    fprintf(fp, "%lf\n", binSize);
    
    if(!massBins) size = 0;
    
    if(size) fprintf(fp, "%lg", massBins[0]);
    for(i = 1; i < size; i++) fprintf(fp, " %lg", massBins[i]);
	
    fprintf(fp, "\n");
    fclose(fp);
}

void twoVarWrite(double *var1Bins, double *var2Bins, int size, 
                 double binSize, double z, char *dirName) {
    FILE *fp;
    char fileName[FILENAME_MAX];
    int i;
    
    sprintf(fileName, "%s/%lf", dirName, z);
    fp = fopen(fileName, "w");
    
    fprintf(fp, "%lf\n", binSize);
    
    if(!(var1Bins && var2Bins)) {
        fprintf(stderr, "Don't give your IO functions NULL pointers.\n");
        exit(1);
    }
    
    if(size) fprintf(fp, "%lg %lg", var1Bins[0], var2Bins[0]);
    for(i = 1; i < size; i++) {
        fprintf(fp, "\n%lg %lg", var1Bins[i], var2Bins[i]);
    }
	
    fprintf(fp, "\n");
    fclose(fp);
}

/*readMDotCol:
 *
 *Given a file named mdot_lum.z=`z`, which contains a lineCount and three
 *colums for \dot{M}, lum, and magnitude, reads the mDot column and 
 *returns it.*/
double *readMDotCol(char *dirName, double z, int *sizePtr) {
    char fileName[FILENAME_MAX];
    sprintf(fileName, "%s/mdot_lum.z=0%.4f", dirName, z);
    return readAsciiColumn(fileName, 0, 3, sizePtr);
}

double *readLumCol(char *dirName, double z, int *sizePtr) {
    char fileName[FILENAME_MAX];
    sprintf(fileName, "%s/mdot_lum.z=0%.4f", dirName, z);
    return readAsciiColumn(fileName, 1, 3, sizePtr);
}

/*extendCopy:
 *
 *Crappy utility function that copies the elements of source to
 *destination. If dest is larger than sources, it fills the extra
 *spaces with bc.
 *
 *If source is larger than dest, it probably seg faults. Don't do that.*/
void extendCopy(double *source, int lenS, 
		double *dest, int lenD, double bc) {
    int i;
    for(i = 0; i < lenD; i++) {
        if(i < lenS) {
            dest[i] = source[i];
        } else {
            dest[i] = bc;
        }
    }
}

/*readIG:
 *
 *Reads in all the files containing the abundance matched data and
 *writes it to an IG. zs is assumed ot be sorted.*/
InterpGrid *readIG(char *dirName, int zCount, double zs[zCount]) {
    InterpGrid *IG;
    int i, maxLen;
    /*A statically allocated array of dynamically allocated arrays:*/
    double *lums[zCount];
    double *maxMDot, *tmpMDot, *flatLum;
    double bc;
    int lens[zCount];
    
    /*Using the readColumn function, extract the raw data and figure
     *out what the length of maximum know mDot array is.*/
    maxLen = 0;
    maxMDot = NULL;
    for(i = 0; i < zCount; i++) {
        lums[i] = readLumCol(dirName, zs[i], lens + i);
        tmpMDot = readMDotCol(dirName, zs[i], lens + i);
        if(lens[i] > maxLen) {
            maxLen = lens[i];
            free(maxMDot);
            maxMDot = tmpMDot;
        } else {
            free(tmpMDot);
        }
    }
    
    /*flatLum is a 2d array z |-> row, mDot -> col.*/
    flatLum = (double*) malloc(sizeof(double) * maxLen * zCount);
    if(! flatLum) {
        fprintf(stderr, "malloc has failed in readIG, oh noes!\n");
        exit(1);
    }
    /*The data is currently not rectangular. It is made to fit in
     *flatLum by extend-copying the arrays over with the base case
     *being their last element.*/
    for(i = 0; i < zCount; i++) {
        bc = lums[i][lens[i] - 1];
        extendCopy(lums[i], lens[i], flatLum + i*maxLen, maxLen, bc);
        free(lums[i]);
    }
    /*Now I have you, my pretty. The returned IG will have z for a y
     *value and \dot{M} as an x value.*/
    IG = interpInit(zCount, maxLen, maxMDot, zs, flatLum);
    free(maxMDot);
    free(flatLum);
    return IG;
}
