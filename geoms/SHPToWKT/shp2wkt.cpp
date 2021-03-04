#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <ctime>
#include <cmath>

#include <unistd.h>
#include <utility>
#include <sys/stat.h>
#include <sys/types.h>

#include <mpi.h>
#include "SHPReader.h"

using namespace std;
using namespace geos::geom;

void writeWKT(std::string file_name, std::list<geos::geom::Geometry*> *lgeoms){	
	std::ofstream tmpfile;
	tmpfile.open(fileName);
		
	for(std::list<geos::geom::Geometry *>::iterator itr = lgeoms->begin(); itr != lgeoms->end(); ++itr){

        geos::geom::Geometry * tmpGeo = *itr;
        try{
            if(tmpGeo!= NULL && tmpGeo->isValid())
                tmpfile<<tmpGeo->toString()+"\n";

        }catch(exception e){

        }
    }

    tmpfile.close();
}

int main(int argc, char ** argv){
		
    double t1 = MPI_Wtime();
    const string filePath1 = argv[1];

    SHPReader reader;

    list<Geometry*> * lgeoms = new list<Geometry*>;

    reader.readShapefile1(argv[1], lgeoms);
	double t2 = MPI_Wtime();

    string outputFile;

    const size_t last_slash_idx = filePath1.rfind('/');
    if (std::string::npos != last_slash_idx){
        outputFile = filePath1.substr(0, last_slash_idx);
    }
   //cout<<lgeoms->size()<<endl; 
    outputFile = outputFile + "/wktfile";
    writeWKT(outputFile, lgeoms);
   // cout<<"2222"<<endl;

	double t3 = MPI_Wtime();
	//cout<<"Size::"<<envsEachStripe2->front().size()<<"::"<<envsEachStripe2->back().size()<<endl;
	//fflush(stdout);
	
	cout<<"Time for reading files::"<<t2 - t1<<endl;
	cout<<"Time for writing files::"<<t3 - t2<<endl;

    return 0;
}
