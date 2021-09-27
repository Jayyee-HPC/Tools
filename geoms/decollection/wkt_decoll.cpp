#include <fstream>
#include <string>
#include <list>
#include <iostream>

//g++ -std=c++11  -o prog wkt_clean.cpp

void parse_str_to_geoms(const std::string file_path, std::list<geos::geom::Geometry*>* l_geoms);

int main(int argc, char** argv)
{
	if(argc < 2){
		std::cerr<<"The path to a WKT file is required!"<<std::endl;
		exit(0);
	}

	std::list<std::string> * orginal_data = new std::list<std::string>;

	std::ifstream rfile(argv[1],std::ios::in);
	std::string tmp_str;

    while(getline(rfile, tmp_str)){
        orginal_data->push_back(tmp_str);
    }

	std::string cwd_path(argv[1]);
	
	std::string file_path = cwd_path + "_data";
	
	std::ofstream temp_file;
	temp_file.open(file_path);

	for(std::list<std::string>::iterator itr = orginal_data->begin(); itr != orginal_data->end(); ++itr)
	{
	    size_t i, start, end;
		std::string temp_str = *itr;
	    size_t size = temp_str.size();

	    if(size < 10) continue;

	    start = end = 0;

	    for(i = 0; i < size; ++i)
	    {
	    	if(temp_str[i] == 9 && start == 0)//To skip the comment
			{
				start = i + 1;
			}
			else if(temp_str[i] == 9 && start !=0)
			{
				end = i;
			}
	    }
	    
	    temp_file<<temp_str.substr(start, end - start);
    	temp_file<<'\n';
	}

	temp_file.close();
	return 0;
}


void parse_str_to_geoms(const std::string file_path, std::list<geos::geom::Geometry*>* l_geoms)
{
	geos::io::WKTReader reader;
	std::string temp_str;
	geos::geom::Geometry* temp_geom = NULL;
	uint i;

	std::ifstream file(file_path.c_str());

	while(std::getline(file, temp_str))
	{
		temp_geom = NULL;
		//omit empty strings and invalid strings
		if(temp_str.size() > 5)
		{
			try
			{
				temp_geom = (reader.read(temp_str)).release();
			}
			catch (std::exception &e)
			{
			}

			if(temp_geom != NULL && temp_geom->isValid())
			{
				if(temp_geom->getGeometryTypeId() == geos::geom::GeometryTypeId::GEOS_MULTIPOLYGON
						|| temp_geom->getGeometryTypeId() == geos::geom::GeometryTypeId::GEOS_MULTILINESTRING
						|| temp_geom->getGeometryTypeId() == geos::geom::GeometryTypeId::GEOS_GEOMETRYCOLLECTION)
				{
					for(i = 0; i < temp_geom->getNumGeometries(); ++i)
					{
						l_geoms->push_back(const_cast<geos::geom::Geometry*>(temp_geom->getGeometryN(i)));
					}
				}
			}
		}
	}
}
