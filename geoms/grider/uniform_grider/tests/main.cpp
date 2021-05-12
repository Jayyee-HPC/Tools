#include <fstream>
#include <ctime>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <grid_utils.h>
#include <uniform_grid.h>

//./ugrider 1024 ~/data/lakes_data ~/data/sports_data

int main(int argc, char **argv)
{
	const int num_partitions = atoi(argv[1]);
	time_t t_start, t_end_parsing, t_end_build_grid, t_end_index, t_end;

	t_start = time(0);

	std::list<geos::geom::Geometry*> *layer_1_geoms = new std::list<geos::geom::Geometry*>;
	std::list<geos::geom::Geometry*> *layer_2_geoms = new std::list<geos::geom::Geometry*>;
	
	Util_Read_geoms(argv[2], layer_1_geoms);
	Util_Read_geoms(argv[3], layer_2_geoms);		

    std::cout<<"Lists size::"<<layer_1_geoms->size()<<"::"<<layer_2_geoms->size()<<std::endl;

	const geos::geom::Envelope * universe = Util_Get_global_env(layer_1_geoms,layer_2_geoms);
	//const geos::geom::Envelope * universe = new geos::geom::Envelope(-180.0,180.0,-90.0,90.0);

	t_end_parsing = time(0);

	Uniform_Grid *grid = new Uniform_Grid(num_partitions, universe);
	
	std::list<const geos::geom::Envelope*> *grid_cells = grid->get_grid_cells();

	t_end_build_grid = time(0);

	geos::index::strtree::STRtree layer_1_index;
	
	for (std::list<geos::geom::Geometry*>::iterator itr = layer_1_geoms->begin() ; itr != layer_1_geoms->end(); ++itr) 
	{
		geos::geom::Geometry* geom_ptr = *itr;	

		layer_1_index.insert(geom_ptr->getEnvelopeInternal(), geom_ptr);      
	}	
	
	geos::index::strtree::STRtree layer_2_index;
	
	for (std::list<geos::geom::Geometry*>::iterator it = layer_2_geoms->begin() ; it != layer_2_geoms->end(); it++) {
		geos::geom::Geometry* p = *it;
	
		layer_2_index.insert( p->getEnvelopeInternal(), p );      
	}

	t_end_index = time(0);

	//Make dirs	
	char cwd_path[100];

	if (NULL == getcwd(cwd_path, sizeof(cwd_path)))
	{
		std::cout<<"Fail to make directory.\n"<<std::endl;
		exit(1);
	}
	
	std::string cwd_path_str(cwd_path);
	std::string file_path_1 = cwd_path_str + "/uni_grid_data_a_" + std::to_string(num_partitions);	

	if (-1 == access(file_path_1.c_str(), F_OK))
	{
			mkdir(file_path_1.c_str(),S_IRWXU);
	}
	
	std::string file_path_2 = cwd_path_str + "/uni_grid_data_b_" + std::to_string(num_partitions);	

	if (-1 == access(file_path_2.c_str(), F_OK)){
			mkdir(file_path_2.c_str(),S_IRWXU);
	}
	
	std::string grid_path = cwd_path_str+"/uni_grid_wkt_file_"+ std::to_string(num_partitions);
	Util_Write_grid_to_WKT(grid_path, grid_cells);
	int name_counter = 0;
		
	for (std::list<const geos::geom::Envelope*>::iterator itr = grid_cells->begin(); itr != grid_cells->end(); ++itr)
	{
		const geos::geom::Envelope* tmpEnv = *itr;
	
		std::string file_name_1 = file_path_1 + "/" + std::to_string(name_counter);
		std::string file_name_2 = file_path_2 + "/" + std::to_string(name_counter);
	
		Util_Write_geoms_to_WKT(file_name_1, tmpEnv, &layer_1_index);
		Util_Write_geoms_to_WKT(file_name_2, tmpEnv, &layer_2_index);
		
		name_counter++;
	}

	t_end = time(0);

	std::cout<<"Reading files:: "<<t_end_parsing - t_start<<std::endl;
	std::cout<<"Making cells:: "<<t_end_build_grid - t_end_parsing<<std::endl;
	std::cout<<"Building index:: "<<t_end_index - t_end_build_grid<<std::endl;
	std::cout<<"Writing files:: "<<t_end - t_end_index<<std::endl;
	std::cout<<"Total time:: "<<t_end - t_start<<std::endl;	

	return 0;
}
