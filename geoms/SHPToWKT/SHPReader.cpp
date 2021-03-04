#include "SHPReader.h"

int SHPReader :: SHPReadMBR( SHPHandle psSHP, int startIndex, int endIndex, PolyRectangle ** mbrs){
	PolyRectangle * bounding_boxes;
	int num=(endIndex-startIndex+1);
	bounding_boxes=(PolyRectangle *) malloc(num*sizeof(PolyRectangle));
	int i,j;

/* -------------------------------------------------------------------- */
/*      Read the record.                                                */
/* -------------------------------------------------------------------- */
    for(i=startIndex;i<=endIndex;i++){
		if( psSHP->sHooks.FSeek( psSHP->fpSHP, psSHP->panRecOffset[i]+12, 0 ) != 0 )
		{
			/*
			 * TODO - mloskot: Consider detailed diagnostics of shape file,
			 * for example to detect if file is truncated.
			 */
			char str[128];
			sprintf( str,
					 "Error in fseek() reading object from .shp file at offset %u",
					 psSHP->panRecOffset[i]+12);

			psSHP->sHooks.Error( str );
			return -1;
		}
		for(j=0;j<4;j++){
			if( psSHP->sHooks.FRead( &(bounding_boxes[i-startIndex].mbr.boundary[j]), sizeof(double), 1, psSHP->fpSHP ) != 1 )
			{
				/*
				 * TODO - mloskot: Consider detailed diagnostics of shape file,
				 * for example to detect if file is truncated.
				 */
				char str[128];
				sprintf( str,
						 "Error in fread() reading object of size %lu at offset %u from .shp file",
						 4*sizeof(double), psSHP->panRecOffset[i]+12 );

				psSHP->sHooks.Error( str );
				return -1;
			}			
		}
		bounding_boxes[i-startIndex].poly_id=i;
    }
    (*mbrs)=bounding_boxes;
    return 0;
}



void SHPReader :: convert(double * rect1, double * rect2, double * rect3, double * rect4,SHPObject **psShape_base, 
int *num_base, int * prefix_base,bdBox *baseBoxes,int cellsPerProcess,int sum_mbrs_overlay,
SHPHandle hSHP_base,double * minX, double *minY)
{
	int i;

	int prefix=0;
	
	for(i =0 ; i< baseBoxes[0].count;i++)
	{     
	  // printf("%d ", i);				
		rect1[i+prefix]=baseBoxes[0].rects[i].mbr.boundary[0];
		rect2[i+prefix]=baseBoxes[0].rects[i].mbr.boundary[1];
		rect3[i+prefix]=baseBoxes[0].rects[i].mbr.boundary[2];
		rect4[i+prefix]=baseBoxes[0].rects[i].mbr.boundary[3];
		if(rect1[i+prefix]<(*minX))
		{
			(*minX)=rect1[i+prefix];
		}
		if(rect2[i+prefix]<(*minY))
		{
			(*minY)=rect2[i+prefix];
		}
		psShape_base[i+prefix] = SHPReadObject( hSHP_base, baseBoxes[0].rects[i].poly_id);
		if( psShape_base[i+prefix] == NULL )
		{
			fprintf( stderr,"Unable to read shape %d, terminating object reading.\n",baseBoxes[0].rects[i].poly_id );
			exit(1);
		}
		num_base[i+prefix]=psShape_base[i+prefix]->nVertices;
		if((i+prefix)==0)
		{
			prefix_base[i+prefix]=0;
		}	
		else
		{
			prefix_base[i+prefix]=prefix_base[i+prefix-1]+num_base[i+prefix-1];
		}
	}



	prefix+=baseBoxes[0].count;
}

void SHPReader:: populateLayer(SHPObject **psShape,int num, list<Geometry*> *layer){
	int i;
	int nu[15] = {0}; 
//printf("%d \n",num);
	//DO NOT FREE in case of furture use
	geos::geom::GeometryFactory* factory_= new geos::geom::GeometryFactory();
        
	for( i = 0; i < num; i++ ) {
    int  j;
          // Create sequence of coordiantes
    CoordinateArraySequence* coords = new geos::geom::CoordinateArraySequence(psShape[i]->nVertices);

          //cout<<i<<" "<<psShape[i]->nVertices<<" ";
    for( j = 0; j < psShape[i]->nVertices; j++ )
		{
      coords->setAt(Coordinate(psShape[i]->padfX[j], psShape[i]->padfY[j]), j);
            //  cout<<psShape[i]->padfX[j]<< ", "<<psShape[i]->padfY[j]<<endl;
    }
		//cout<<coords->front().toString () <<endl;
		//cout<<coords->back().toString () <<endl;
          //cout<<endl;
    //coords->setAt(Coordinate(psShape[i]->padfX[0], psShape[i]->padfY[0]), j);
          //cout<<psShape[i]->padfX[j]<< ", "<<psShape[i]->padfY[j]<<endl;
        
          // Create exterior ring
		switch(psShape[i]->nSHPType){
			case 0:
				nu[0]++;
				break;
			case 1:
				nu[1]++;
				break;		
			case 3:
				nu[2]++;
				break;
			case 5:
				nu[3]++;
				break;
			case 8:
				nu[4]++;
				break;
			case 11:
				nu[5]++;
				break;
			case 13:
				nu[6]++;
				break;
			case 15:
				nu[7]++;
				break;	
			case 18:
				nu[8]++;
				break;
			case 21:
				nu[9]++;
				break;
			case 23:
				nu[10]++;
				break;
			case 25:
				nu[11]++;
				break;
			case 28:
				nu[12]++;
				break;
			case 31:
				nu[13]++;
				break;				
			default:
				nu[14]++;
				break;
		}
		if(coords->front().equals(coords->back())){			
			LinearRing* exterior1 = factory_->createLinearRing(coords);
			//LineString* exterior1 = factory_->createLineString(coords);
			//Geometry* poly = factory_->createGeometry(exterior1);
            Polygon* poly = factory_->createPolygon(exterior1, NULL);
            //if(poly->isValid())
			layer->push_back((Geometry*)poly);
        }
		/*}else{
			//coords->add(Coordinate(psShape[i]->padfX[0], psShape[i]->padfY[0]));
			unsigned int i;
			CoordinateArraySequence *tmpCoor = new CoordinateArraySequence();
			for(i = 0; i < coords->getSize(); i++){
				tmpCoor->add(coords->getAt(i));
				if(tmpCoor->getSize() > 3 && coords->front().equals(coords->getAt(i))){					
					break;
				}	else if(coords->front().equals(coords->getAt(i)))	{
					tmpCoor->clear();
				}		
			}
			
			
			//unsigned int start = i+1;
			//unsigned int end = start+1;
			//CoordinateArraySequence *tmpCoorHole = new CoordinateArraySequence();
			//vector< Geometry * > *holes = new vector< Geometry * >;
			for(unsigned int flg = start; flg < coords->getSize(); flg++){
				tmpCoorHole->add(coords->getAt(flg));
				if(start != flg && coords->getAt(start).equals(coords->getAt(flg))){
					if(tmpCoorHole->getSize() > 3){
						LinearRing* holeGeom = factory_->createLinearRing(tmpCoorHole);
						Geometry* hole = factory_->createGeometry(holeGeom);
						holes->push_back(hole);
						tmpCoorHole->clear();
						start=flg+1;
					}
					

				}
				
			}	*/
			
		/*	if(coords->getSize() == tmpCoor->getSize()){
				LineString* exterior1 = factory_->createLineString(coords);
			Geometry* poly = factory_->createGeometry(exterior1);
				cout<<poly->toString()<<endl;
			continue;	
			}
			if(!tmpCoor->front().equals(tmpCoor->back())){
				//tmpCoor->add(coords->front());
				//cout<<tmpCoor->toString()<<endl;
				continue;
			}
			//tmpCoor->add(coords->front());
			//cout<<coords->getSize()<<"::"<<tmpCoor->getSize()<<endl;
			try{
			LinearRing* exterior1 = factory_->createLinearRing(tmpCoor);
			Geometry* poly = factory_->createGeometry(exterior1);
			//Polygon* poly = factory_->createPolygon(exterior1, holes);
			//cout<<poly->toString()<<endl;
			layer->push_back((Geometry*)poly);
			}catch(exception &e){
		//throw;
		}
		}*/
		//Polygon* exterior = factory_.createPolygon (exterior1, NULL);
		
          //tut::ensure("simple ext", exterior->isSimple() );

		//Geometry* poly =  factory_.createEmptyGeometry ();
		//Geometry* poly1 =  poly->Union(exterior);
		//Polygon* poly = factory_.createPolygon(exterior);
 // std::cout << "poly empty: " << poly->isEmpty()
  //<< ", simple: " << poly->isSimple()
  //<< ", points: " << poly->getNumPoints()
  //<< std::endl;
    
		//cout<<((Geometry*)exterior)->toString()<<endl;
  }
  //cout<<"populate fn "<<layer->size()<<endl;
	
	//for(int k = 0; k < 15; k++){
	//	cout<<nu[k]<<endl;
	//}
}

string SHPReader :: shp2Str(SHPObject	*shpObj){
	string tmpStr;
	string type; 
	int size = shpObj->nVertices;
	switch(shpObj->nSHPType){
		case SHPT_POINT:
			type = "POINT";
			break;
		case SHPT_POLYGON:
			type = "POLYGON";
			break;		
		case SHPT_ARC:
			type = "LINESTRING";
			break;		
		default:
			printf("Unsupport format of geometry.\n");
			break;
	}
	
	if(type.empty()){
		return tmpStr;
	}else{
		tmpStr.append(type);
		tmpStr.append("\t((");
		for(int i = 0; i < size; i++){
			tmpStr.append(to_string(shpObj->padfX[i]));
			tmpStr.append(" ");
			tmpStr.append(to_string(shpObj->padfY[i]));
			if(i == size - 1 ){
				tmpStr.append("))");
			}else{
				tmpStr.append(",");
			}
		}	
	}
	return tmpStr;
}


void SHPReader :: destoryObjects(SHPObject **psShape,int num){
	int i;
	for(i=0;i<num;i++){
	    SHPDestroyObject(psShape[i]);
	}
}


int SHPReader :: readShapefile(char* filepath, list<Geometry*> *lGeoms)
{
	int i;

	SHPHandle	shpHandle;
	shpHandle = SHPOpen(filepath,"r");
	PolyRectangle *mbrs_base;
	
	int startIndex_base=0;
  int endIndex_base=shpHandle->nRecords-1;
    
  int count_base=endIndex_base-startIndex_base+1;
    
	SHPReadMBR(shpHandle, startIndex_base, endIndex_base, &mbrs_base);

	bdBox *baseBoxes;

	baseBoxes=(bdBox *)malloc(sizeof(bdBox));

	baseBoxes->count=count_base;
	baseBoxes->rects=mbrs_base;
	
	int sum_mbrs_base=baseBoxes[0].count ;

	double minX_base,minY_base;
	minX_base=10000000;
	minY_base=10000000;
	SHPObject	**psShape_base;
	psShape_base=(SHPObject **)malloc((sum_mbrs_base)*sizeof(SHPObject*));
	int *num_base;
	int * prefix_base;

	num_base=(int *)malloc((sum_mbrs_base)*sizeof(int));
	prefix_base=(int *)malloc((sum_mbrs_base)*sizeof(int));
	
	double 	*rect1=(double *)malloc(sum_mbrs_base*sizeof(double));
	double	*rect2=(double *)malloc(sum_mbrs_base*sizeof(double));
	double	*rect3=(double *)malloc(sum_mbrs_base*sizeof(double));
	double	*rect4=(double *)malloc(sum_mbrs_base*sizeof(double));	
	
	convert(rect1,rect2,rect3,rect4,psShape_base,num_base,prefix_base,baseBoxes,0,
	sum_mbrs_base,shpHandle,&minX_base,&minY_base);

	geos::io::WKTReader wktreader;

	//populateLayer(psShape_base,sum_mbrs_base,lGeoms);
 	for(i =0 ; i< sum_mbrs_base;i++){
		string tmpStr = shp2Str(psShape_base[i]);
		try{
			Geometry* tmpGeo = NULL;
			tmpGeo = wktreader.read(tmpStr);
			ExtraInfo *extraInfo = new ExtraInfo();
			extraInfo->vertexStr = tmpStr;
			tmpGeo->setUserData(extraInfo);
			if(tmpGeo->isValid())
				lGeoms->push_back(tmpGeo);
		}catch(exception &e){
			//throw;
		}
	} 

	//printf("sum_mbrs_base = %d sum_mbrs_overlay = %d \n", sum_mbrs_base,sum_mbrs_overlay);
	
	destoryObjects(psShape_base,sum_mbrs_base);

	SHPClose(shpHandle);	

	return 0;
}
