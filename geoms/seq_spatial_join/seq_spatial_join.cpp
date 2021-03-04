#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <sstream>
#include <chrono> 

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <geos/geom/Geometry.h>
#include <geos/geom/Envelope.h>
#include <geos/geom/Point.h>
#include <geos/geom/prep/PreparedGeometryFactory.h>
#include <geos/geom/prep/PreparedGeometry.h>
#include <geos/io/WKTReader.h>
#include <geos/index/strtree/STRtree.h>

// g++ -fopenmp -L/home//.local/lib -lgeos-3.8.1 -I/usr/local/include -I/home//.local/include -std=c++17 -Wfatal-errors -o prog seq_spatial_join.cpp
// ./prog /WKT/file/path/1 /WKT/file/path/2 

void readGeoms(std::string file_path, std::list<geos::geom::Geometry*>* lgeoms)
{	
	std::ifstream rfile(file_path,std::ios::in);
	std::list<std::string> * str_list = new std::list<std::string>;
	std::string temp_str;
	
	while(getline(rfile, temp_str))
	{
		str_list->push_back(temp_str);
	}
	rfile.close();

	std::cout<<"File size :: "<<str_list->size()<<std::endl;
	
	geos::io::WKTReader wkt_reader;
	
	for(std::list<std::string>::iterator itr = str_list->begin();itr != str_list->end();itr++)
	{
		temp_str = *itr;
		geos::geom::Geometry* temp_Geo = NULL;	
		try{
			temp_Geo = (wkt_reader.read(temp_str)).release();
		}
		catch(std::exception &e)
		{
			//skip errors;
		}
		
		if(temp_Geo != NULL && temp_Geo->isValid())
		{
			lgeoms->push_back(temp_Geo);
		}
	}
	
	delete str_list;
}

int main(int argc, char **argv){
	auto t_begin = std::chrono::high_resolution_clock::now();

	const std::string file0 = argv[1];
	const std::string file1 = argv[2];

	std::list<geos::geom::Geometry*> *lgeoms1 = new std::list<geos::geom::Geometry*>;
	std::list<geos::geom::Geometry*> *lgeoms2 = new std::list<geos::geom::Geometry*>;

	readGeoms(file0.c_str(), lgeoms1);
	readGeoms(file1.c_str(), lgeoms2);

	auto t_finish_parsing = std::chrono::high_resolution_clock::now();

	size_t work_count =0;

	geos::index::strtree::STRtree index;
	
	for (std::list<geos::geom::Geometry*>::iterator itr = lgeoms1->begin() ; itr != lgeoms1->end(); itr++) 
	{
		geos::geom::Geometry* geom = *itr;	
		index.insert( geom->getEnvelopeInternal(), geom);      
	}	
	
	for(std::list<geos::geom::Geometry*>::iterator itr = lgeoms2->begin(); itr != lgeoms2->end(); ++itr ){
		geos::geom::Geometry* temp_geom = *itr;			
		std::vector<void *> results;

		geos::geom::prep::PreparedGeometryFactory pgf;
		
		const geos::geom::Envelope * temp_env=  temp_geom->getEnvelopeInternal();
		std::unique_ptr<geos::geom::prep::PreparedGeometry> ppd_geom = pgf.create(temp_geom);				
		index.query(temp_env, results);

		for(std::vector<void *>::iterator vd_itr = results.begin(); vd_itr != results.end(); vd_itr ++){
			void *poly_to_ptr = *vd_itr;
			geos::geom::Geometry* qrd_geom = (geos::geom::Geometry*)poly_to_ptr;
			
			try{
				if(ppd_geom->intersects(qrd_geom)){
				    std::unique_ptr<geos::geom::Geometry> output_geom = temp_geom->intersection(qrd_geom);
                    if(NULL != output_geom)
                        work_count += output_geom->getNumPoints();
                }
			}catch(std::exception &e){
				//throw;
			}
		}
	}

	printf("\n");

	auto t_end = std::chrono::high_resolution_clock::now();

	std::cout<<"Join result :: "<<work_count<<std::endl;	
	std::cout<<"Reading files(ms) :: "<<std::chrono::duration_cast<std::chrono::milliseconds>
		(t_finish_parsing - t_begin).count()<<std::endl;
	std::cout<<"Finding intersections(ms) :: "<<std::chrono::duration_cast<std::chrono::milliseconds>
		(t_end - t_finish_parsing).count()<<std::endl;

	return 0;
}
