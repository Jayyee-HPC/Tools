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
#include <geos/io/WKTWriter.h>

//g++ -std=c++17 -I${HOME}/.local/include -L${HOME}/.local/lib -lgeos -o prog wkt_to_mbr.cpp

void Read_WKT_file(std::string file_name, std::list<geos::geom::Geometry *> *list_geoms);

void Write_MBR_with_Weight_to_file(std::string file_name, std::string output_file_name = "");

void Write_MBR_with_Weight_to_folder(std::string folder_name, const int num_files);

int main(int argc, char **argv)
{
    auto t_start = std::chrono::steady_clock::now();
    const std::string file_path = argv[1];
    const int num_files = std::stoi(argv[2]);

    std::cout << "File path " << file_path << std::endl;
    std::cout << "Num_files " << num_files << std::endl;

    if (num_files == 0)
    {
        //Num_files = 0 means input is single file and output will also be single file
        Write_MBR_with_Weight_to_file(file_path);
    }
    else
    {
        //Input should be a folder and all files in it should have names from 0 to (num_files-1)
        Write_MBR_with_Weight_to_folder(file_path, num_files);
    }

    std::list<geos::geom::Geometry *> *list_geoms = new std::list<geos::geom::Geometry *>;

    Read_WKT_file(file_path, list_geoms);

    auto t_parse_end = std::chrono::steady_clock::now();

    if (list_geoms->empty())
    {
        std::cerr << "Get no geometries from " + file_path << std::endl;
        exit(0);
    }

    std::string output_file;

    output_file = file_path + "_wkt";

    //Write_WKT_to_file(output_file, list_geoms);

    auto t_end = std::chrono::steady_clock::now();

    std::chrono::duration<double> t_diff_0 = t_parse_end - t_start;
    std::chrono::duration<double> t_diff_1 = t_end - t_parse_end;

    std::cout << "WKT to WKB converting finished:: ";
    std::cout << ". # geometries:: " << list_geoms->size() << std::endl;
    std::cout << "Time for reading files::" << t_diff_0.count() << std::endl;
    std::cout << "Time for writing files::" << t_diff_1.count() << std::endl;
    std::cout << "Path for the wkt file" << output_file << std::endl;

    return 0;
}

void Read_WKT_file(std::string file_name, std::list<geos::geom::Geometry *> *list_geoms)
{
    std::ifstream file(file_name.c_str());
    std::string temp_str;
    geos::io::WKTReader reader;

    while (std::getline(file, temp_str))
    {
        //omit empty strings and invalid strings
        if (temp_str.size() > 5)
        {
        }

        geos::geom::Geometry *temp_geom = NULL;
        try
        {
            temp_geom = (reader.read(temp_str)).release();
        }
        catch (std::exception &e)
        {
            //throw;
        }

        if (temp_geom != NULL && temp_geom->isValid())
        {
            list_geoms->push_back(temp_geom);
        }
    }
}

void Write_MBR_with_Weight_to_file(std::string file_name, std::string output_file_name)
{
    std::list<geos::geom::Geometry *> *list_geoms = new std::list<geos::geom::Geometry *>;

    Read_WKT_file(file_name, list_geoms);

    if (list_geoms->empty() && output_file_name.empty())
    {
        std::cerr << "Get no geometries from " + file_name << std::endl;
        return;
    }

    std::string output_file;

    if (output_file_name.empty())
        output_file = file_name + "_mbr";
    else
        output_file = output_file_name;

    std::ofstream temp_file;
    temp_file.open(output_file);

    geos::io::WKTWriter wkt_writer;
    wkt_writer.setTrim(false);

    for (std::list<geos::geom::Geometry *>::iterator itr = list_geoms->begin(); itr != list_geoms->end(); ++itr)
    {

        geos::geom::Geometry *tmpGeo = *itr;

        if (tmpGeo != NULL)
        {
            std::string temp_str = std::to_string(tmpGeo->getEnvelopeInternal()->getMinX()) + " " +
                                   std::to_string(tmpGeo->getEnvelopeInternal()->getMaxX()) + " " +
                                   std::to_string(tmpGeo->getEnvelopeInternal()->getMinY()) + " " +
                                   std::to_string(tmpGeo->getEnvelopeInternal()->getMaxY()) + " " +
                                   std::to_string(tmpGeo->getNumPoints()) + "\n";

            temp_file << temp_str;
        }
    }

    temp_file.close();
}

void Write_MBR_with_Weight_to_folder(std::string folder_path, const int num_files)
{
    //Make dirs
    char cwd_path[100];

    if (NULL == getcwd(cwd_path, sizeof(cwd_path)))
    {
        std::cout << "Fail to get current directory." << std::endl;
        exit(1);
    }

    std::string cwd_path_str(cwd_path);

    std::string output_folder_path = cwd_path_str + "/mbrs_" + std::to_string(num_files);

    if (-1 == access(output_folder_path.c_str(), F_OK))
    {
        mkdir(output_folder_path.c_str(), S_IRWXU);
        std::cout << output_folder_path << " created." << std::endl;
    }
    else
    {
        std::cout << output_folder_path << " exists and will reuse it. Old files will be overwritten."
                  << std::endl;
    }


    for (int i = 0; i < num_files; ++i)
    {
        std::string temp_file = folder_path + std::to_string(i);
        std::string temp_output_file = output_folder_path + "/" + std::to_string(i);

        Write_MBR_with_Weight_to_file(temp_file, temp_output_file);
    }
}