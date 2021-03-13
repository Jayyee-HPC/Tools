#define USE_UNSTABLE_GEOS_CPP_API

#include <iostream>
#include <sstream>
#include <fstream>
#include <list>
#include <vector>
#include <chrono>
#include <cmath>
#include <cstring>
#include <unistd.h>
#include <utility>
#include <sys/stat.h>
#include <sys/types.h>

#include <geos/geom/Geometry.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKBWriter.h>

//g++ -std=c++17 -I${HOME}/.local/include -L${HOME}/.local/lib -lgeos -o prog wkb_to_wkt.cpp
//
void Read_WKT_file(std::string file_name, std::list<geos::geom::Geometry*> *l_geoms);
void Write_WKB_to_file(std::string file_name, std::list<geos::geom::Geometry*> *l_geoms);

int main(int argc, char ** argv){
		
    auto t_start = std::chrono::steady_clock::now();
    const std::string file_path = argv[1];
    std::list<geos::geom::Geometry*> * l_geoms = new std::list<geos::geom::Geometry*>;

	Read_WKT_file(file_path, l_geoms);

    auto t_parse_end = std::chrono::steady_clock::now();

    if(l_geoms->empty())
    {
    	std::cerr<<"Get no geometries from " + file_path<<std::endl;
    	exit(0);
    }
    
    std::string output_file;

    output_file = file_path + "_wkb";

    Write_WKB_to_file(output_file, l_geoms);

    auto t_end = std::chrono::steady_clock::now();

    std::chrono::duration<double> t_diff_0 = t_parse_end - t_start;
    std::chrono::duration<double> t_diff_1 = t_end - t_parse_end;

	std::cout<<"WKT to WKB converting finished:: ";
	std::cout<<". # geometries:: "<<l_geoms->size()<<std::endl;
	std::cout<<"Time for reading files::"<<t_diff_0.count()<<std::endl;
	std::cout<<"Time for writing files::"<<t_diff_1.count()<<std::endl;
    std::cout<<"Path for the wkb file"<<output_file<<std::endl;

    return 0;
}

void Read_WKT_file(std::string file_name, std::list<geos::geom::Geometry*> *l_geoms)
{
    std::ifstream file(file_name.c_str());
	std::string temp_str;
	geos::io::WKTReader reader;

    while(std::getline(file, temp_str))
    {
       //omit empty strings and invalid strings
        if(temp_str.size() > 5)
        {

        }

        geos::geom::Geometry* temp_geom = NULL;
        try
        {
            temp_geom = (reader.read(temp_str)).release();
        }
        catch(std::exception &e)
        {
            //throw;
        }

        if(temp_geom != NULL){// && temp_geom->isValid()){
            l_geoms->push_back(temp_geom);
        }
    }
}

void Write_WKB_to_file(std::string file_name, std::list<geos::geom::Geometry*> *l_geoms)
{	
	std::ofstream temp_file;
	temp_file.open(file_name);
	geos::io::WKBWriter wkb_writer;
	wkb_writer.setOutputDimension(2);
    wkb_writer.setByteOrder(1);
    wkb_writer.setIncludeSRID(0);

	for(std::list<geos::geom::Geometry *>::iterator itr = l_geoms->begin(); itr != l_geoms->end(); ++itr){

        geos::geom::Geometry * tmpGeo = *itr;
        std::stringstream wkb_stream;

        try{
        	if(tmpGeo!= NULL && tmpGeo->isValid())
        	{
        		wkb_writer.writeHEX(*tmpGeo, wkb_stream);
            	temp_file<<wkb_stream.str()+"\n";
        	}
        }
        catch(std::exception &e)
        {

        }
    }

    temp_file.close();
}
