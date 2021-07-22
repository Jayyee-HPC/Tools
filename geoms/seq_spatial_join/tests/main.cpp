#define USE_UNSTABLE_GEOS_CPP_API
#include <global.h>

int Spatial_Join_ws(int argc, char **argv);
int Spatial_Join_seq(int argc, char **argv);
int Spatial_Join_seq_no_partition(int argc, char **argv);

int main(int argc, char **argv)
{
    //Spatial_Join_ws(argc, argv);
    Spatial_Join_seq(argc, argv);
    return 0;
}

int Spatial_Join_ws(int argc, char **argv)
{
    spdlog::set_pattern("[%H:%M:%S.%e] %v");

#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
#else
    spdlog::set_level(spdlog::level::info); // Set global log level to info
#endif

    spdlog::debug("Enter function {}", "Spatial_Join_using_numa");

    auto t_begin = std::chrono::steady_clock::now();

    const int num_files = atoi(argv[1]);

    const std::string file_path_1 = argv[2];
    const std::string file_path_2 = argv[3];

    spdlog::info("File path 1 {}, file path 2 {}, num_files {}", file_path_1, file_path_2, num_files);

#ifdef USE_ST_INTERSECTION
    spdlog::info("USE_ST_INTERSECTION");
#elif USE_ST_INTERSECTS
    spdlog::info("USE_ST_INTERSECTS");
#elif USE_ST_UNION
    spdlog::info("USE_ST_UNION");
#else //default using USE_ST_INTERSECTION
    spdlog::info("USE_ST_INTERSECTION");
#endif

    uint num_threads = 36;

    ulong join_result = 0;

    join_result = gsj::thread_func::Thread_Wrapper_spatial_join_multi_owners_multi_files_parsing(num_threads, file_path_1, file_path_2, num_files);

    auto t_join_finished = std::chrono::steady_clock::now();

    std::chrono::duration<double> t_diff_1 = t_join_finished - t_begin;

    spdlog::info("Program finished in {0:03.3f}, join result {1:d}",
                 t_diff_1.count(), join_result);

    spdlog::debug("Leave function {}", "Spatial_Join_using_numa");

    return 0;
}

int Spatial_Join_seq(int argc, char **argv)
{
    spdlog::set_pattern("[%H:%M:%S.%e] %v");

#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
#else
    spdlog::set_level(spdlog::level::info); // Set global log level to info
#endif

    spdlog::debug("Enter function {}", "Spatial_Join_seq");

    auto t_begin = std::chrono::steady_clock::now();

    const int num_files = atoi(argv[1]);

    const std::string file_path_1 = argv[2];
    const std::string file_path_2 = argv[3];

    spdlog::info("File path 1 {}, file path 2 {}, num_files {}", file_path_1, file_path_2, num_files);

#ifdef USE_ST_INTERSECTION
    spdlog::info("USE_ST_INTERSECTION");
#elif USE_ST_INTERSECTS
    spdlog::info("USE_ST_INTERSECTS");
#elif USE_ST_UNION
    spdlog::info("USE_ST_UNION");
#else //default using USE_ST_INTERSECTION
    spdlog::info("USE_ST_INTERSECTION");
#endif

    gsj::Reader reader;
    int join_result = 0;
    double time_parse = 0.0;
    double time_join = 0.0;

    for (int i = 0; i < num_files; ++i)
    {
        auto t_parse_begin = std::chrono::steady_clock::now();

        std::list<geos::geom::Geometry *> *list_geoms_1 = new std::list<geos::geom::Geometry *>;
        std::list<geos::geom::Geometry *> *list_geoms_2 = new std::list<geos::geom::Geometry *>;

        std::string temp_file_path1 = file_path_1 + std::to_string(i);
        std::string temp_file_path2 = file_path_2 + std::to_string(i);

        reader.Read_Geoms_from_file_parallel(temp_file_path1, list_geoms_1);
        reader.Read_Geoms_from_file_parallel(temp_file_path2, list_geoms_2);

        auto t_parse_end = std::chrono::steady_clock::now();

        geos::index::strtree::STRtree index_for_layer_1;
        for (std::list<geos::geom::Geometry *>::iterator itr = list_geoms_1->begin(); itr != list_geoms_1->end(); ++itr)
        {
            geos::geom::Geometry *temp_geom = *itr;

            index_for_layer_1.insert(temp_geom->getEnvelopeInternal(), temp_geom);
        }

        for (std::list<geos::geom::Geometry *>::iterator itr = list_geoms_2->begin(); itr != list_geoms_2->end(); ++itr)
        {
            geos::geom::Geometry *temp_geom = *itr;
            std::vector<void *> results;

            const geos::geom::Envelope *temp_env = temp_geom->getEnvelopeInternal();

            index_for_layer_1.query(temp_env, results);

            if (results.size() != 0)
            {
#ifdef USE_ST_INTERSECTS
                std::unique_ptr<geos::geom::prep::PreparedGeometry> pg =
                    geos::geom::prep::PreparedGeometryFactory::prepare(temp_geom);
#endif //ifdef USE_ST_INTERSECTS
                for (std::vector<void *>::iterator void_itr = results.begin(); void_itr != results.end(); void_itr++)
                {
                    void *temp_geom_ptr = *void_itr;
                    geos::geom::Geometry *qrd_geom = (geos::geom::Geometry *)temp_geom_ptr;

                    try
                    {
#ifdef USE_ST_INTERSECTION
                        std::unique_ptr<geos::geom::Geometry> geoms_intersection = temp_geom->intersection(qrd_geom);

                        join_result += geoms_intersection->getNumPoints();

                        geoms_intersection = nullptr;

#elif USE_ST_INTERSECTS
                        if (pg->intersects(qrd_geom))
                            ++join_result;

#elif USE_ST_UNION
                        std::unique_ptr<geos::geom::Geometry> geoms_union = temp_geom->Union(qrd_geom);

                        join_result += geoms_union->getNumPoints();

                        geoms_union = nullptr;

#else //default using USE_ST_INTERSECTION
                        std::unique_ptr<geos::geom::Geometry> geoms_intersection = temp_geom->intersection(qrd_geom);

                        join_result += geoms_intersection->getNumPoints();
                        geoms_intersection = nullptr;
#endif
                    }
                    catch (std::exception &e)
                    {
                        spdlog::error("Join error: {}", e.what());
                    }
                }
            }
        }
        auto t_join_end = std::chrono::steady_clock::now();

        std::chrono::duration<double> t_diff_temp_1 = t_parse_end - t_parse_begin;
        std::chrono::duration<double> t_diff_temp_2 = t_join_end - t_parse_end;

        time_parse += t_diff_temp_1.count();
        time_join += t_diff_temp_2.count();
    }

    auto t_end = std::chrono::steady_clock::now();

    std::chrono::duration<double> t_diff_1 = t_end - t_begin;

    spdlog::info("Whole time in {0:03.3f}, parse in {1:03.3f}, join in {2:03.3f}, join result {3:d}",
                 t_diff_1.count(), time_parse, time_join, join_result);

    spdlog::debug("Leave function {}", "Spatial_Join_seq");

    return 0;
}

int Spatial_Join_seq_no_partition(int argc, char **argv)
{
    spdlog::set_pattern("[%H:%M:%S.%e] %v");

#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
#else
    spdlog::set_level(spdlog::level::info); // Set global log level to info
#endif

    spdlog::debug("Enter function {}", "Spatial_Join_seq_no_partition");

    auto t_begin = std::chrono::steady_clock::now();

    const std::string file_path_1 = argv[2];
    const std::string file_path_2 = argv[3];

    spdlog::info("File path 1 {}, file path 2 {}", file_path_1, file_path_2);

#ifdef USE_ST_INTERSECTION
    spdlog::info("USE_ST_INTERSECTION");
#elif USE_ST_INTERSECTS
    spdlog::info("USE_ST_INTERSECTS");
#elif USE_ST_UNION
    spdlog::info("USE_ST_UNION");
#else //default using USE_ST_INTERSECTION
    spdlog::info("USE_ST_INTERSECTION");
#endif

    gsj::Reader reader;
    ulong join_result = 0;
    double time_parse = 0.0;
    double time_join = 0.0;

    std::list<geos::geom::Geometry *> *list_geoms_1 = new std::list<geos::geom::Geometry *>;
    std::list<geos::geom::Geometry *> *list_geoms_2 = new std::list<geos::geom::Geometry *>;

    reader.Read_Geoms_from_file_parallel(file_path_1, list_geoms_1);
    reader.Read_Geoms_from_file_parallel(file_path_2, list_geoms_2);

    auto t_parse_end = std::chrono::steady_clock::now();

    geos::index::strtree::STRtree index_for_layer_1;
    for (std::list<geos::geom::Geometry *>::iterator itr = list_geoms_1->begin(); itr != list_geoms_1->end(); ++itr)
    {
        geos::geom::Geometry *temp_geom = *itr;

        index_for_layer_1.insert(temp_geom->getEnvelopeInternal(), temp_geom);
    }

    for (std::list<geos::geom::Geometry *>::iterator itr = list_geoms_2->begin(); itr != list_geoms_2->end(); ++itr)
    {
        geos::geom::Geometry *temp_geom = *itr;
        std::vector<void *> results;

        const geos::geom::Envelope *temp_env = temp_geom->getEnvelopeInternal();

        index_for_layer_1.query(temp_env, results);

        if (results.size() != 0)
        {
#ifdef USE_ST_INTERSECTS
            std::unique_ptr<geos::geom::prep::PreparedGeometry> pg =
                geos::geom::prep::PreparedGeometryFactory::prepare(temp_geom);
#endif //ifdef USE_ST_INTERSECTS
            for (std::vector<void *>::iterator void_itr = results.begin(); void_itr != results.end(); void_itr++)
            {
                void *temp_geom_ptr = *void_itr;
                geos::geom::Geometry *qrd_geom = (geos::geom::Geometry *)temp_geom_ptr;
                try
                {
#ifdef USE_ST_INTERSECTION
                    std::unique_ptr<geos::geom::Geometry> geoms_intersection = temp_geom->intersection(qrd_geom);

                    join_result += geoms_intersection->getNumPoints();

                    geoms_intersection = nullptr;

#elif USE_ST_INTERSECTS
                    if (pg->intersects(qrd_geom))
                        ++join_result;

#elif USE_ST_UNION
                    std::unique_ptr<geos::geom::Geometry> geoms_union = temp_geom->Union(qrd_geom);

                    join_result += geoms_union->getNumPoints();

                    geoms_union = nullptr;

#else //default using USE_ST_INTERSECTION
                    std::unique_ptr<geos::geom::Geometry> geoms_intersection = temp_geom->intersection(qrd_geom);

                    join_result += geoms_intersection->getNumPoints();
                    geoms_intersection = nullptr;
#endif
                }
                catch (std::exception &e)
                {
                    spdlog::error("Join error: {}", e.what());
                }
            }
        }
    }
    auto t_join_end = std::chrono::steady_clock::now();

    std::chrono::duration<double> t_diff_temp_1 = t_parse_end - t_begin;
    std::chrono::duration<double> t_diff_temp_2 = t_join_end - t_parse_end;

    time_parse == t_diff_temp_1.count();
    time_join == t_diff_temp_2.count();

    auto t_end = std::chrono::steady_clock::now();

    std::chrono::duration<double> t_diff_1 = t_end - t_begin;

    spdlog::info("Whole time in {0:03.3f}, parse in {1:03.3f}, join in {2:03.3f}, join result {3:d}",
                 t_diff_1.count(), time_parse, time_join, join_result);

    spdlog::debug("Leave function {}", "Spatial_Join_seq_no_partition");

    return 0;
}
