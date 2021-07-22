#include <reader.h>

namespace gsj{

void Reader :: Read_Strs_from_file(const std::string file_path, std::vector<std::string> *v_strs){
    std::ifstream file(file_path.c_str());
	std::string temp_str;

    while(std::getline(file, temp_str))
    {
       //omit empty strings and invalid strings
        if(temp_str.size() > 5)
            v_strs->push_back(temp_str);       
    }

    file.close();
}

void Reader :: Read_Geoms_from_strs(std::vector<std::string> *v_strs, std::list<geos::geom::Geometry*> *l_geoms)
{
    geos::io::WKTReader reader;
    std::string temp_str;
    geos::geom::Geometry* temp_geom = NULL;
    uint i;

    for(std::vector<std::string>::iterator itr = v_strs->begin();itr != v_strs->end();++itr)
    {
        temp_str = *itr;
        temp_geom = NULL;

        try
        {
            temp_geom = (reader.read(temp_str)).release();
        }
        catch(std::exception &e)
        {
            //throw;
        }

        if(temp_geom != NULL){// && temp_geom->isValid()){
            if(temp_geom->getGeometryTypeId() == geos::geom::GeometryTypeId::GEOS_MULTIPOLYGON
                    || temp_geom->getGeometryTypeId() == geos::geom::GeometryTypeId::GEOS_MULTILINESTRING
                    || temp_geom->getGeometryTypeId() == geos::geom::GeometryTypeId::GEOS_GEOMETRYCOLLECTION)
            {
                for(i = 0; i < temp_geom->getNumGeometries(); ++i)
                {
                    l_geoms->push_back(const_cast<geos::geom::Geometry*>(temp_geom->getGeometryN(i)));
                }
            }
            else
            {
                l_geoms->push_back(temp_geom);  
            }
        }
    }

    //Do not free temp_geom, it's used in the future.
}

void Reader :: Read_Geoms_from_strs_wkb(std::vector<std::string> *v_strs, std::list<geos::geom::Geometry*> *l_geoms)
{
    geos::io::WKBReader reader;
    std::string temp_str;
    geos::geom::Geometry* temp_geom = NULL;
    uint i;

    for(std::vector<std::string>::iterator itr = v_strs->begin();itr != v_strs->end();++itr)
    {
        temp_str = *itr;
        temp_geom = NULL;
        std::stringstream temp_stream(temp_str);

        try
        {
            temp_geom = (reader.readHEX(temp_stream)).release();
        }
        catch(std::exception &e)
        {
            //throw;
        }

        if(temp_geom != NULL){// && temp_geom->isValid()){
            if(temp_geom->getGeometryTypeId() == geos::geom::GeometryTypeId::GEOS_MULTIPOLYGON
                    || temp_geom->getGeometryTypeId() == geos::geom::GeometryTypeId::GEOS_MULTILINESTRING
                    || temp_geom->getGeometryTypeId() == geos::geom::GeometryTypeId::GEOS_GEOMETRYCOLLECTION)
            {
                for(i = 0; i < temp_geom->getNumGeometries(); ++i)
                {
                    l_geoms->push_back(const_cast<geos::geom::Geometry*>(temp_geom->getGeometryN(i)));
                }
            }
            else
            {
                l_geoms->push_back(temp_geom);  
            }
        }
    }

    //Do not free temp_geom, it's used in the future.
}

static void  Thread_Read_geoms_from_strs(std::vector<std::string> *v_strs, std::list<geos::geom::Geometry*>* l_geoms,
    geos::io::WKTReader *reader, std::mutex *push_mutex)
{
    std::string temp_str;
    geos::geom::Geometry* temp_geom = NULL;
    std::list<geos::geom::Geometry*>* thread_l_geoms = new std::list<geos::geom::Geometry*>();
    uint i;

    for(std::vector<std::string>::iterator itr = v_strs->begin();itr != v_strs->end();++itr)
    {
        temp_str = *itr;
        temp_geom = NULL;

        try
        {
            temp_geom = (reader->read(temp_str)).release();
        }
        catch(std::exception &e)
        {
            //throw;
        }

        if(temp_geom != NULL && temp_geom->isValid()){//Disable validation check increase performance
            if(temp_geom->getGeometryTypeId() == geos::geom::GeometryTypeId::GEOS_MULTIPOLYGON
                    || temp_geom->getGeometryTypeId() == geos::geom::GeometryTypeId::GEOS_MULTILINESTRING
                    || temp_geom->getGeometryTypeId() == geos::geom::GeometryTypeId::GEOS_GEOMETRYCOLLECTION)
            {
                for(i = 0; i < temp_geom->getNumGeometries(); ++i)
                {
                    thread_l_geoms->push_back(const_cast<geos::geom::Geometry*>(temp_geom->getGeometryN(i)));
                }
            }
            else
            {
                thread_l_geoms->push_back(temp_geom);  
            }
        }
    }

	push_mutex->lock();

	while(!thread_l_geoms->empty())
	{
		l_geoms->push_back(thread_l_geoms->back());
		thread_l_geoms->pop_back();
	}

	push_mutex->unlock();

	//Do not free temp_geom, it's used in the future.
    //To avoid memory leak, we may free it after using.
}

void Reader :: Read_Geoms_from_file(const std::string file_path, std::list<geos::geom::Geometry*> *l_geoms)
{
    std::vector<std::string> * v_strs = new std::vector<std::string>;

    Read_Strs_from_file(file_path, v_strs);

    if(v_strs->empty())
    {
        //std::cerr<<"File [" + file_path + "] is empty."<<std::endl;
        return;
    }

    Read_Geoms_from_strs(v_strs, l_geoms);

    v_strs->clear();
    v_strs->shrink_to_fit();
}

void Reader :: Read_Geoms_from_file_wkb(const std::string file_path, std::list<geos::geom::Geometry*> *l_geoms)
{
    std::vector<std::string> * v_strs = new std::vector<std::string>;

    Read_Strs_from_file(file_path, v_strs);

    if(v_strs->empty())
    {
        //std::cerr<<"File [" + file_path + "] is empty."<<std::endl;
        return;
    }

    Read_Geoms_from_strs_wkb(v_strs, l_geoms);

    v_strs->clear();
    v_strs->shrink_to_fit();
}

void Reader :: Read_Geoms_from_file_parallel(const std::string file_path, std::list<geos::geom::Geometry*> *l_geoms, 
        uint num_threads)
{
    uint i, vect_size;
    geos::io::WKTReader wkt_reader[num_threads];
    std::vector<std::string> * v_strs = new std::vector<std::string>;
    std::vector<std::string>::iterator sub_itr_begin, sub_itr_end;
    std::vector<std::thread>* local_thread_vect = new std::vector<std::thread>();

    Read_Strs_from_file(file_path, v_strs);

    if(v_strs->empty())
        return;

    if(v_strs->size() < 200)//Too tiny for parallel parsing
    {
        Read_Geoms_from_strs(v_strs, l_geoms);
        return;
    }
    std::vector<std::vector<std::string>*> vect_l_str(num_threads, NULL);
    vect_size = v_strs->size();
    vect_size = vect_size/num_threads;

    std::mutex push_mutex;

    for(i = 0; i < num_threads; ++i) 
    {
        sub_itr_begin = v_strs->begin() + i*vect_size;
        if(i == num_threads-1)
            sub_itr_end = v_strs->end();
        else
            sub_itr_end = v_strs->begin() + (i+1)*vect_size;

        vect_l_str[i] = new std::vector<std::string>(sub_itr_begin, sub_itr_end);
        wkt_reader[i] = geos::io::WKTReader();

        local_thread_vect->push_back(std::thread(Thread_Read_geoms_from_strs,
                vect_l_str[i], l_geoms, &wkt_reader[i], &push_mutex));
    }

    for(std::vector<std::thread>::iterator itr = local_thread_vect->begin(); itr != local_thread_vect->end(); ++itr)
    {
        (*itr).join();
    }

    delete local_thread_vect;
    v_strs->clear();
    v_strs->shrink_to_fit();
}

void Reader :: Read_Envs_from_strs(std::vector<std::string> *v_strs, std::vector<geos::geom::Envelope *> *v_envs)
{
    size_t i, j, k, start, end, str_len, size = v_strs->size();
    std::string temp_str;
    double env[4] = {0.0};;

    for(i = 0; i < size; ++i)
    {
        temp_str = v_strs->at(i);
        start = end = 0;
        str_len = temp_str.size();

        for(j = 0; j < str_len; ++j)
        {
            k = 0;
            if(temp_str[j] == ' ' && k < 4)
            {
                end = j;
                env[k]=std::strtod((temp_str.substr(start, end -start)).c_str(), NULL);
                ++k;
                start = j+1;
            }
           if(4 <= k)break;  
        }
        v_envs->push_back(new geos::geom::Envelope(env[0], env[1], env[2], env[3]));
    }
}

void Reader :: Read_Envs_Weights_from_strs(std::vector<std::string> *v_strs, 
        std::vector<std::pair<geos::geom::Envelope *, int>* > *v_envs)
{
    uint i, j, k, start, end, str_len, size = v_strs->size();
    std::string temp_str;
    double env[4] = {0.0};

    for(i = 0; i < size; ++i)
    {
        temp_str = v_strs->at(i);
        start = end = 0;        
        str_len = temp_str.size();

        for(j = 0; j < str_len; ++j)
        {  
            k = 0;          
            if(temp_str[j] == ' ' && k < 4)
            {
                end = j;
                env[k] = std::atof(temp_str.substr(start, end-start).c_str());
                ++k;
                start = j+1;
            }
            if(4 <= k)break;           
        }

        v_envs->push_back(new std::pair<geos::geom::Envelope *, int>
            (new geos::geom::Envelope(env[0], env[1], env[2], env[3]), 
                std::atoi((temp_str.substr(start, str_len-start)).c_str())));
    }
}

}// namespace gsj
