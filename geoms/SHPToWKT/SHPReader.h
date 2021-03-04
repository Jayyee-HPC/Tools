#ifndef __SHP_READER_H_INCLUDED__
#define __SHP_READER_H_INCLUDED__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <list>
#include <vector>

#include <geos/geom/Geometry.h>
#include <geos/geom/LinearRing.h>
#include <geos/io/WKTReader.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateArraySequence.h>

#include "spatialPartition/mpitype.h"

using namespace geos::geom;
using namespace std;

struct Rectangle
{
    double boundary[4]; /* xmin,ymin,...,xmax,ymax,... */
};

/* MBR of a polygon */
typedef struct PolyRectangle
{
    int poly_id;
    int cellid;        // cell id because a polygon goes to different cells, filled after
                       // gridding
    struct Rectangle mbr;   /* Rect is defined as a structure with 4
                          doubles: xmin,ymin,xmax,ymax */
} PolyRectangle;

/* bBox holds MBRs of base/overlay polygons for a given cell */
typedef struct bdBox
{
 // count is for rects array size
 int count;     // should be initialized
 int allocated; // for realloc
 int processorIncharge;  // tells me which processor should handle this cell
 struct PolyRectangle *rects;   
} bdBox;

#ifdef __cplusplus
extern "C" {
#endif

#include "shapefil.h"

class SHPReader{
	private:
	void destoryObjects(SHPObject **psShape,int num);

	void printObjects(SHPObject	**psShape,int num);

	void convert(double *rect1, double *rect2, double *rect3, double *rect4,SHPObject **psShape_base, int *num_base, int * prefix_base,bdBox *baseBoxes,int cellsPerProcess,int sum_mbrs_overlay, SHPHandle hSHP_base,double * minX,double *minY);

	int SHPReadMBR( SHPHandle hSHP, int startIndex, int endIndex, PolyRectangle ** mbrs);
	string shp2Str(SHPObject	*shpObj);
	
	void populateLayer(SHPObject **psShape,int num, list<Geometry*> *layer);
	
	public:
	int readShapefile(char* filepath, list<Geometry*> *lGeoms);
};



    
#ifdef __cplusplus
}
#endif      
                  
#endif          
