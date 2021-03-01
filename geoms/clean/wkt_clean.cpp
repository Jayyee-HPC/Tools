#include <fstream>
#include <string>
#include <list>
#include <iostream>

//g++ -std=c++11  -o prog wkt_clean.cpp

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
