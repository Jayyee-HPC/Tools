#include <uniform_grid.h>

int Uniform_Grid :: log2(uint value) {  //Calculate Exponentiation
	if (value == 1)
	{
		return 0;
	}
	else
	{
		return 1+log2(value>>1);
	}
}

bool Uniform_Grid :: is_int_exp_of_2(uint value){
	if(value&(value-1))
	{
		return 0;
	}
	else
	{
		num_iterations = log2(value);
		return 1;
	}	
}

int Uniform_Grid :: n_partition(const geos::geom::Envelope *universe)
{
	uint i, j, size;
	double mid_x, mid_y;

	std::vector<double> vect_min_x(num_cells);
	std::vector<double> vect_max_x(num_cells);
	std::vector<double> vect_min_y(num_cells);
	std::vector<double> vect_max_y(num_cells);

	vect_min_x[0] = universe->getMinX();
	vect_max_x[0] = universe->getMaxX();
	vect_min_y[0] = universe->getMinY();
	vect_max_y[0] = universe->getMaxY();

	for(i = 0; i < num_iterations; i++){
		size = std::pow(2,i+1);

		std::vector<double> temp_vect_min_x(size);
	 	std::vector<double> temp_vect_max_x(size);
	 	std::vector<double> temp_vect_min_y(size);
		std::vector<double> temp_vect_max_y(size);

		for(j = 0; j < size; j+=2)
		{
			if(i%2)
			{
				mid_x = (vect_max_x[j/2] + vect_min_x[j/2]) /2;
				
				temp_vect_min_x[j] = vect_min_x[j/2];
				temp_vect_max_x[j] = mid_x;
				temp_vect_min_y[j] = vect_min_y[j/2];
				temp_vect_max_y[j] = vect_max_y[j/2];
				
				temp_vect_min_x[j+1] = mid_x;
				temp_vect_max_x[j+1] = vect_max_x[j/2];
				temp_vect_min_y[j+1] = vect_min_y[j/2];
				temp_vect_max_y[j+1] = vect_max_y[j/2];
			}
			else
			{
				mid_y = (vect_max_y[j/2] + vect_min_y[j/2]) /2;
				
				temp_vect_min_x[j] = vect_min_x[j/2];
				temp_vect_max_x[j] = vect_max_x[j/2];
				temp_vect_min_y[j] = vect_min_y[j/2];
				temp_vect_max_y[j] = mid_y;
				
				temp_vect_min_x[j+1] = vect_min_x[j/2];
				temp_vect_max_x[j+1] = vect_max_x[j/2];
				temp_vect_min_y[j+1] = mid_y;
				temp_vect_max_y[j+1] = vect_max_y[j/2];				
			}		
		}
		
		for(j = 0; j < size; j++)
		{
			vect_min_x[j] = temp_vect_min_x[j];
			vect_max_x[j] = temp_vect_max_x[j];
			vect_min_y[j] = temp_vect_min_y[j];
			vect_max_y[j] = temp_vect_max_y[j];
		}
	}	
	
	for(i = 0; i < num_cells; i++)
	{
		grid_cells->push_back(new geos::geom::Envelope(vect_min_x[i],vect_max_x[i],vect_min_y[i],vect_max_y[i]));	
	}

	return 0;
}
