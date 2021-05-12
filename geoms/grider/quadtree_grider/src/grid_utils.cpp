#include <grid_utils.h>

void Util_Read_geoms(std::string file_path, std::list<geos::geom::Geometry *>* list_geoms)
{	
	std::ifstream file(file_path.c_str());
	std::list<std::string> * list_strs = new std::list<std::string>;
	std::string temp_str;

    while (std::getline(file, temp_str))
    {
       //omit empty strings and invalid strings
        if(temp_str.size() > 5)
            list_strs->push_back(temp_str);
    }

    file.close();

	if (VERBOSE)
		std::cout<<"File size::"<<list_strs->size()<<std::endl;
	
	geos::io::WKTReader wktreader;
	geos::geom::Geometry* temp_geom = NULL;

	for (std::list<std::string>::iterator itr = list_strs->begin(); itr != list_strs->end(); ++itr){
		temp_str = *itr;
		temp_geom  = NULL;

		try
		{
			temp_geom  = (wktreader.read(temp_str)).release();
		}
		catch(std::exception &e)
		{
			//throw;
		}
		
		if(temp_geom != NULL && temp_geom->isValid()){
			list_geoms->push_back(temp_geom);
		}
	}
	
	delete list_strs;
}

int Util_Write_geoms_to_WKT(std::string file_name, const geos::geom::Envelope* env, 
		geos::index::strtree::STRtree *index)
{	
	//Finding geometries
	std::vector<void *> results;
	
	index->query(env, results);

	//do not produce empty files
	if(results.empty())
	{
		return 0;
	}
	
	std::ofstream temp_file;
	temp_file.open(file_name);
	
	//Writing cell bounding box
	std::string env_str = std::to_string(env->getMinX())+ " " + 
								std::to_string(env->getMaxX())+ " " + 
								std::to_string(env->getMinY())+ " " + 
								std::to_string(env->getMaxY())+ "\n";

	temp_file<<	env_str;		
			
	for (std::vector<void *>::iterator void_itr = results.begin(); void_itr != results.end(); ++void_itr)
	{		
		void *void_geom_ptr = *void_itr;

		geos::geom::Geometry* query_geom = (geos::geom::Geometry*)void_geom_ptr;
		temp_file<<query_geom->toString()+"\n";	
	}
	
	temp_file.close();
	return 0;
}

geos::geom::Geometry* Util_Covert_env_to_geom(const geos::geom::Envelope* env){
	if(env == NULL)
		return NULL;

	double min_x = env->getMinX();
	double max_x = env->getMaxX();
	double min_y = env->getMinY();
	double max_y = env->getMaxY();
	
	std::string temp_str = "POLYGON ((" + std::to_string(min_x) + " " + std::to_string(min_y) + ","
							+ std::to_string(min_x) + " " + std::to_string(max_y) + ","
							+ std::to_string(max_x) + " " + std::to_string(max_y) + ","
							+ std::to_string(max_x) + " " + std::to_string(min_y) + ","
							+ std::to_string(min_x) + " " + std::to_string(min_y) + +"))";
																	
	geos::io::WKTReader wktreader;
	
	geos::geom::Geometry* temp_geom = NULL;

	try
	{
		temp_geom = (wktreader.read(temp_str)).release();
	}
	catch(std::exception &e)
	{
		//throw;
	}
	
	return temp_geom;
}

void Util_Write_grid_to_WKT(std::string file_name, std::list<const geos::geom::Envelope*> *list_envs){
	std::ofstream temp_file;
	temp_file.open(file_name);
	
	geos::geom::Geometry* temp_geom = NULL;
	for (std::list<const geos::geom::Envelope*>::iterator itr = list_envs->begin() ; 
			itr != list_envs->end(); itr++)
	{		
		const geos::geom::Envelope* temp_env = *itr;
		
		temp_geom = Util_Covert_env_to_geom(temp_env);
		
		if (temp_geom != NULL)
			temp_file<<temp_geom->toString()+"\n";
	}

	temp_file.close();
}

void Util_Write_geoms_to_array(std::string file_name, const geos::geom::Envelope* env, 
		geos::index::strtree::STRtree *index)
{
	std::vector<void *> results;
	
	index->query(env, results);
	
	//not produce empty files
	if (results.empty())
	{
		return;
	}

	std::ofstream tmpfile;

	tmpfile.open (file_name);

	std::string x_array_str;
	std::string y_array_str;
	std::string index_array_str;
	std::string envs_array_str;
	int count = 0;

	for(std::vector<void *>::iterator vdItr = results.begin(); vdItr != results.end(); ++ vdItr){
		void *void_geom_ptr = *vdItr;
		geos::geom::Geometry* temp_geom = (geos::geom::Geometry*)void_geom_ptr;

		envs_array_str.append(std::to_string(temp_geom->getEnvelopeInternal()->getMinX())+" ");
		envs_array_str.append(std::to_string(temp_geom->getEnvelopeInternal()->getMaxX())+" ");
		envs_array_str.append(std::to_string(temp_geom->getEnvelopeInternal()->getMinY())+" ");
		envs_array_str.append(std::to_string(temp_geom->getEnvelopeInternal()->getMaxY())+" ");

		geos::geom::CoordinateSequence * coord_seq = (temp_geom->getCoordinates()).release();

		int endCount = coord_seq->getSize();
		for(int i = 0; i < endCount; i++){
			x_array_str.append(std::to_string(coord_seq->getAt(i).x));

			y_array_str.append(std::to_string(coord_seq->getAt(i).y));				
			
			x_array_str.append(" ");
			y_array_str.append(" ");
		}
		
		index_array_str.append(std::to_string(count));
		index_array_str.append(" ");
		count+=endCount;
	}

	envs_array_str.erase(envs_array_str.end() - 1);
	x_array_str.erase(x_array_str.end() - 1);
	y_array_str.erase(y_array_str.end() - 1);
	
	x_array_str.append("\n");
	y_array_str.append("\n");		
	index_array_str.append(std::to_string(count));
	index_array_str.append("\n");
	envs_array_str.append("\n");

	std::string envStr = std::to_string(env->getMinX())+ " " + 
							std::to_string(env->getMaxX())+ " " + 
							std::to_string(env->getMinY())+ " " + 
							std::to_string(env->getMaxY());
							
	tmpfile << x_array_str;
	tmpfile << y_array_str;
	tmpfile << index_array_str;
	tmpfile << envs_array_str;
	tmpfile << envStr;

	tmpfile.close();
	
	return;
}

const geos::geom::Envelope* Util_Get_global_env(std::list<geos::geom::Geometry*> *layer_1_geoms, 
		std::list<geos::geom::Geometry*> *layer_2_geoms)
{
	double min_x = 180.0;
	double max_x = -180.0;
	double min_y = 90.0;
	double max_y = -90.0;
		
	for (std::list<geos::geom::Geometry*>::iterator itr = layer_1_geoms->begin(); itr != layer_1_geoms->end(); ++itr)
	{
		geos::geom::Geometry* temp_geom = *itr;

		if(temp_geom == NULL || !temp_geom->isValid())
			continue;

		const geos::geom::Envelope* temp_env = temp_geom->getEnvelopeInternal();

		if(temp_env->getMinX() < min_x)
			min_x = temp_env->getMinX();
		if(temp_env->getMaxX() > max_x)
			max_x = temp_env->getMaxX();
		if(temp_env->getMinY() < min_y)
			min_y = temp_env->getMinY();
		if(temp_env->getMaxY() > max_y)
			max_y = temp_env->getMaxY();
	}
	
	for (std::list<geos::geom::Geometry*>::iterator itr = layer_2_geoms->begin(); 
			itr != layer_2_geoms->end();itr++)
	{
		geos::geom::Geometry* temp_geom = *itr;

		if (temp_geom == NULL || !temp_geom->isValid())
			continue;

		const geos::geom::Envelope* temp_env = temp_geom->getEnvelopeInternal();

		if (temp_env->getMinX() < min_x)
			min_x = temp_env->getMinX();
		if (temp_env->getMaxX() > max_x)
			max_x = temp_env->getMaxX();
		if (temp_env->getMinY() < min_y)
			min_y = temp_env->getMinY();
		if (temp_env->getMaxY() > max_y)
			max_y = temp_env->getMaxY();
	}
	
	const geos::geom::Envelope* env = new geos::geom::Envelope(min_x, max_x, min_y, max_y);

	return env;
}
