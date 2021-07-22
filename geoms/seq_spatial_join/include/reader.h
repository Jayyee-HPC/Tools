#ifndef __GSJ_READER_H_INCLUDE__
#define __GSJ_READER_H_INCLUDE__

#include <global_var.h>

#include <vector>
#include <sstream>
#include <fstream>
#include <thread>         // std::thread
#include <mutex>          // std::mutex
#include <list>
#include <stdlib.h>
#include <cstring>

//#include "mpi.h"

#include <geos/geom/Geometry.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKBReader.h>

namespace gsj{

class Reader{
    public:
        void Read_Strs_from_file(const std::string file_path, std::vector<std::string> *v_strs); 

        void Read_Geoms_from_file(const std::string file_path, std::list<geos::geom::Geometry*> *l_geoms);

        void Read_Geoms_from_file_wkb(const std::string file_path, std::list<geos::geom::Geometry*> *l_geoms);

        void Read_Geoms_from_strs(std::vector<std::string> *v_strs, std::list<geos::geom::Geometry*> *l_geoms);

        void Read_Geoms_from_strs_wkb(std::vector<std::string> *v_strs, std::list<geos::geom::Geometry*> *l_geoms);

        void Read_Envs_from_strs(std::vector<std::string> *v_strs, 
        		std::vector<geos::geom::Envelope *> *v_envs);

        void Read_Envs_Weights_from_strs(std::vector<std::string> *v_strs, 
        		std::vector<std::pair<geos::geom::Envelope *, int>* > *v_envs);

        void Read_Geoms_from_file_parallel(const std::string file_path, std::list<geos::geom::Geometry*> *l_geoms, 
        		uint num_threads = NUM_THREADS);

        Reader()
        {
        };

        ~Reader(){};

    private:

};

} // namespace gsj

#endif // ndef __GSJ_READER_H_INCLUDE__
