#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <tuple>
#include <sstream>
#include <cmath>
#include <iterator>
#include <memory>
#include <algorithm>
#include <string>
#include <array>

typedef std::tuple<double, double, double> data_tuple;
typedef std::vector<data_tuple> frame;
typedef std::vector<frame> frames;


template<typename T>
T fromString (const std::string& s);

std::string exec (const std::string& str);

std::vector<data_tuple> coordinates_read (const std::string & name);

std::vector<data_tuple> getting_centroid_coordinates (const std::string & coordinate_files_name, int & data_files_count);

void data_file_creation (const std::string & name, std::vector<data_tuple> & data);


int main() {
    int data_files_count = fromString<int>(exec("find ./coordinates_only -type f | wc -l"));
    std::vector<data_tuple> centroid_coordinates = std::move(getting_centroid_coordinates("coordinates_only/base_p-1.pos_", data_files_count));
    data_file_creation("centroid", centroid_coordinates);

    //std::vector<data_tuple> centroid_coordinates = std::move(coordinates_read("centroid"));

    return 0;
}


template <typename T>
std::string toString (T val) {
    std::ostringstream oss;
    oss << val;
    return oss.str();
}


template<size_t Is = 0, typename... Tp>
void coordinates_average (std::tuple<Tp...>& coordinate, std::tuple<Tp...>& new_data, const double & data_count) {
    std::get<Is>(coordinate) += (std::get<Is>(new_data) / data_count);
    if constexpr(Is + 1 != sizeof...(Tp))
        coordinates_average<Is + 1>(coordinate, new_data, data_count);
}


std::vector<data_tuple> getting_centroid_coordinates (const std::string & coordinate_files_name, int & data_files_count) {
    std::vector<data_tuple> data = std::move(coordinates_read(coordinate_files_name + toString(0)));
    if (data_files_count == 1)
        return data;
    data_tuple zero_tuple = std::make_tuple(0.0, 0.0, 0.0);
    for (auto & i : data) i = zero_tuple;
    for (int i = 0; i < data_files_count; ++i)
        for (long j = 0; j < data.size(); ++j) {
            std::vector<data_tuple> new_data = std::move(coordinates_read(coordinate_files_name + toString(i)));
            coordinates_average(data[j], new_data[j], data_files_count);
        }
    return data;
}


template<typename T, size_t... Is>
std::string tuple_to_string_impl (T const& t, std::index_sequence<Is...>) {
    return ((toString(std::get<Is>(t)) + '\t') + ...);
}

template <class Tuple>
std::string tuple_to_string (const Tuple& t) {
    constexpr auto size = std::tuple_size<Tuple>{};
    return tuple_to_string_impl(t, std::make_index_sequence<size>{});
}


// Creates text-file with coordinates from std::vector<data_tuple> with given name.
void data_file_creation (const std::string & name, std::vector<data_tuple> & data) {
    std::ofstream fout;
    fout.open(name, std::ios::trunc);
    for (auto & i : data)
        fout << tuple_to_string(i) << '\n';
    fout.close();
}


// Returns number from string.
template<typename T>
T fromString (const std::string& s) {
    std::istringstream iss(s);
    T res;
    iss >> res;
    return res;
}


//The function returns the terminal ans. Input - string for term.
std::string exec (const std::string& str) {
    const char* cmd = str.c_str();
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        result += buffer.data();
    result = result.substr(0, result.length()-1);
    return result;
}


namespace std {
    istream& operator >> (istream& in, data_tuple & coordinates) {
        double first, second, third;
        in >> first >> second >> third;
        coordinates = {first, second, third};
        return in;
    }

    ostream& operator << (ostream& out, const data_tuple & coordinates) {
        auto [first, second, third] = coordinates;
        out << first << ' ' << second << ' ' << third << ' ';
        return out;
    }
}


// Read data from columns in text-file.
std::vector<data_tuple> coordinates_read (const std::string & name) {
    std::ifstream fin(name);
    if (!fin.is_open()) throw std::runtime_error("Error opening file.");
    std::vector<data_tuple> tuples_vector;
    copy(std::istream_iterator<data_tuple> {fin},
         std::istream_iterator<data_tuple> {},
         back_inserter(tuples_vector));
    //copy(tuples_vector.begin(), tuples_vector.end(), std::ostream_iterator<data>(std::cout, "\n"));
    return tuples_vector;
}