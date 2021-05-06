#ifndef __UNIFORM_GRID_H_INCLUDED__
#define __UNIFORM_GRID_H_INCLUDED__

#include "global_var.h"

#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <cmath>

#include <geos/geom/Envelope.h>
#include <geos/geom/Geometry.h>

class Uniform_Grid
{
	public:
	Uniform_Grid(const int cells, const geos::geom::Envelope *universe)
	{
		num_cells = cells;

		if(!is_int_exp_of_2(cells))
		{
			printf("The number of cells needs to be exponential of 2\n");
			exit(1);
		};

		grid_cells = new std::list<const geos::geom::Envelope *>; 
   
		n_partition(universe);
	} 
 
	std::list<const geos::geom::Envelope *> * get_grid_cells(void){
		return grid_cells;
	} 

	private:
	uint num_iterations;
	uint num_cells;
	std::list<const geos::geom::Envelope *> *grid_cells;
	
	int log2(uint value);
	bool is_int_exp_of_2(uint value);
		
	int n_partition(const geos::geom::Envelope * universe); 
	int bi_partition(const geos::geom::Envelope * env, 
	std::list<const geos::geom::Envelope *> * gridCellsBuf, int paritionDirection);
};

#endif //ndef __UNIFORM_GRID_H_INCLUDED__
