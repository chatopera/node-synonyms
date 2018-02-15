//                                     __
//    ____ ___  ___  ____ ___  ____   / /__   __
//   / __ `__ \/ _ \/ __ `__ \/ __ \ / __/ | / /
//  / / / / / /  __/ / / / / / /_/ // /_ | |/ /
// /_/ /_/ /_/\___/_/ /_/ /_/\____(_)__/ |___/
//
//
// Created by Memo Akten, www.memo.tv
//
// Templated generic vector<T> util functions for when you don't want to use a huge linalg lib
// Only very basic vector functions implemented. For more complex stuff I suggest a dedicated library (like Eigen or Armadillo)


#pragma once

#include <vector>
#include <sstream>
#include <random>
#include <cctype> // for isspace
#include <string>

using namespace std;

namespace msa {
namespace vector_utils {

// Left trim the given string ("  hello!  " --> "hello!  ")
std::string left_trim(std::string str) {
    int numStartSpaces = 0;
    for (int i = 0; i < str.length(); i++) {
        if (!isspace(str[i])) break;
        numStartSpaces++;
    }
    return str.substr(numStartSpaces);
}

// Right trim the given std::string ("  hello!  " --> "  hello!")
std::string right_trim(std::string str) {
    int numEndSpaces = 0;
    for (int i = str.length() - 1; i >= 0; i--) {
        if (!isspace(str[i])) break;
        numEndSpaces++;
    }
    return str.substr(0, str.length() - numEndSpaces);
}

// Left and right trim the given std::string ("  hello!  " --> "hello!")
std::string trim(std::string str) {
    return right_trim(left_trim(str));
}

/**
 * return as string
 * @param v      [description]
 * @param result [description]
 */
inline void to_string(const std::vector<float> v, std::string& result){
    std::stringstream ss;
    ss << "[";
    std::vector<float>::const_iterator it = v.begin();
    std::vector<float>::const_iterator last = (v.end() - 1);
    for(; it != v.end(); it++){
        ss << *it;
        if(it != last){
            ss << ",";
        } 
    }
    ss << "]";
    result = ss.str();
}

template <typename T>
std::string to_string(const vector<T>& v0) {
    std::stringstream s;
    s << "[ ";
    for (const auto& f : v0) s << f << " ";
    s << "]";
    return s.str();
}



//-------------------------------------------------------
// return weighted sum (v0 * w0) + (v1 * w1)
template <typename T>
vector<T> weighted_sum(const vector<T>& v0, T w0, const vector<T>& v1, T w1) {
    assert(v0.size() == v1.size());
    int n = (int)v0.size();
    vector<T> vr(n);
    for(int i=0; i<n; i++) vr[i] = (v0[i] * w0) + (v1[i] * w1);
    return vr;
}



//-------------------------------------------------------
// return sum of two vectors
template <typename T>
vector<T> add(const vector<T>& v0, const vector<T>& v1) {
    return weighted_sum(v0, (T)1, v1, (T)1);
}



//-------------------------------------------------------
// return difference bewteen two vectors
template <typename T>
vector<T> subtract(const vector<T>& v0, const vector<T>& v1) {
    return weighted_sum(v0, (T)1, v1, (T)-1);
}



//-------------------------------------------------------
// return mix (blend) of two vectors
template <typename T>
vector<T> mix(const vector<T>& v0, const vector<T>& v1, T t) {
    return weighted_sum(v0, (T)(1-t), v1, (T)t);
}




//-------------------------------------------------------
// return scaled vector (multiply with scalar)
template <typename T>
vector<T> scaled(const vector<T>& v0, T f) {
    vector<T> vr(v0);
    int n = (int)v0.size();
    for(int i=0; i<n; i++) vr[i] *= f;
    return vr;
}



//-------------------------------------------------------
// get dot product of two vectors
template <typename T>
T dot(const vector<T>& v0, const vector<T>& v1) {
    assert(v0.size() == v1.size());
    int n = (int)v0.size();
    T r = 0;
    for(int i=0; i<n; i++) r += v0[i] * v1[i];
    return r;
}



//-------------------------------------------------------
// return element wise multiplication
template <typename T>
vector<T> mul(const vector<T>& v0, const vector<T>& v1) {
    assert(v0.size() == v1.size());
    int n = (int)v0.size();
    vector<T> vr(n);
    for(int i=0; i<n; i++) vr[i] = v0[i] * v1[i];
    return vr;
}



//-------------------------------------------------------
// return element wise division
template <typename T>
vector<T> div(const vector<T>& v0, const vector<T>& v1) {
    assert(v0.size() == v1.size());
    int n = (int)v0.size();
    vector<T> vr(n);
    for(int i=0; i<n; i++) vr[i] = v0[i] / v1[i];
    return vr;
}



//-------------------------------------------------------
// return L1 norm of vector
template <typename T>
T l1_norm(const vector<T>& v0) {
    int n = (int)v0.size();
    T r = 0;
    for(int i=0; i<n; i++) r += abs(v0[i]);
    return r;
}




//-------------------------------------------------------
// return L2 norm of vector
template <typename T>
T l2_norm(const vector<T>& v0) {
    int n = (int)v0.size();
    T r = 0;
    for(int i=0; i<n; i++) { T a = v0[i]; r += a * a; }
    return sqrt(r);
}



//-------------------------------------------------------
// return normalized vector (length of vector is 1)
template <typename T>
vector<T> normalized(const vector<T>& v0) {
    T l2_inv = 1/l2_norm(v0);
    vector<T> vr(v0);
    int n = (int)v0.size();
    for(int i=0; i<n; i++) vr[i] *=l2_inv;
    return vr;
}





//-------------------------------------------------------
// return number of bytes
template <typename T>
int num_bytes(const vector<T>& v) {
    return sizeof(v[0]) * (int)v.size();
}



//-------------------------------------------------------
// return min and max values of a 2d table (vector of vectors)
template <typename T>
void get_range(const std::vector< vector<T> >& data, vector<T>& min_values, vector<T>& max_values) {
    if (data.empty()) return;

    int row_size = (int)data[0].size();

    // start with first set of data for both min and max
    min_values = data[0];
    max_values = data[0];

    // iterate all entries in data
    for (const auto& row : data) {

        // find min&  max values for each element in row
        for (int i = 0; i < row_size; i++) {
            min_values[i] = std::min( row[i], min_values[i] );
            max_values[i] = std::max( row[i], max_values[i] );
        }
    }
}



//-------------------------------------------------------
// simple linear map function, should put this elsewhere
template <typename T>
inline T map(T value, T input_min, T input_max, T output_min, T output_max) {
    if (input_max == input_min) return 0;
    return (value - input_min) / (input_max - input_min) * (output_max - output_min) + output_min;
}



//-------------------------------------------------------
// normalize vectors across dataset with range [min_values...max_values] (each of which is a vector), to the range [min_value...max_value]
template <typename T>
void normalize_dataset(const vector<T>& vector_in, const vector<T>& min_values, const vector<T>& max_values, const T min_value, const T max_value, vector<T>& norm_vector_out) {
    int n = (int)vector_in.size();

    // resize output vector if it's not the right size
    if(norm_vector_out.size() != n) norm_vector_out.resize(n);

    // iterate all elements and map to relevant range
    for (int i = 0; i < n; i++) norm_vector_out[i] = map(vector_in[i], min_values[i], max_values[i], min_value, max_value);
}



//-------------------------------------------------------
// unnormalize vectors across dataset with range [min_value...max_value], to the range [min_values...max_values] (each of which is a vector)
template <typename T>
void unnormalize_dataset(const vector<T>& norm_vector_in, const vector<T>& min_values, const vector<T>& max_values, const T min_value, const T max_value, vector<T>& vector_out) {
    int n = (int)norm_vector_in.size();

    // resize output vector if it's not the right size
    if(vector_out.size() != n) vector_out.resize(n);

    // iterate all elements and map to relevant range
    for (int i = 0; i < n; i++) vector_out[i] = map(norm_vector_in[i], min_value, max_value, min_values[i], max_values[i]);
}



//==============================================================================================================
// unit tests

// unit tests for basic functions
template <typename T>
void test() {
    vector<T> v0 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    vector<T> v1 = {11, 13, 15, 17, 19, 21, 23, 25, 27, 29};

    std::cout << "v0 = " << to_string(v0) << std::endl;
    std::cout << "v1 = " << to_string(v1) << std::endl;
    std::cout << "v0 + v1 = " << to_string(add(v0, v1)) << std::endl;
    std::cout << "v0 - v1 = " << to_string(subtract(v0, v1)) << std::endl;
    std::cout << "v0 * 2 = " << to_string(scaled(v0, (T)2.0)) << std::endl;
    std::cout << "v0 * 0.5 + v1 * 0.5 = " << to_string(weighted_sum(v0, (T)0.5, v1, (T)0.5)) << std::endl;
    std::cout << "v0 * 0.2 + v1 * 0.8 = " << to_string(weighted_sum(v0, (T)0.2, v1, (T)0.8)) << std::endl;
    std::cout << "v0 -> v1, 0.0 = " << to_string(mix(v0, v1, (T)0.0)) << std::endl;
    std::cout << "v0 -> v1, 1.0 = " << to_string(mix(v0, v1, (T)1.0)) << std::endl;
    std::cout << "v0 -> v1, 0.8 = " << to_string(mix(v0, v1, (T)0.8)) << std::endl;
    std::cout << "v0 -> v1, 0.5 = " << to_string(mix(v0, v1, (T)0.5)) << std::endl;
    std::cout << "v0 . v1 = " << dot(v0, v1) << std::endl;
    std::cout << "v0 * v1 = " << to_string(mul(v0, v1)) << std::endl;
    std::cout << "v0 / v1 = " << to_string(div(v0, v1)) << std::endl;
    std::cout << "l1_norm v0 = " << l1_norm(v0) << std::endl;
    std::cout << "l2_norm v0 = " << l2_norm(v0) << std::endl;
    std::cout << "norm v0 = " << to_string(normalized(v0)) << std::endl;
    std::cout << "l1_norm norm v0 = " << l1_norm(normalized(v0)) << std::endl;
    std::cout << "l2_norm norm v0 = " << l2_norm(normalized(v0)) << std::endl;

}


//-------------------------------------------------------
// unit tests for dataset related functions
template <typename T>
void dump_dataset_stats(const std::vector< vector<T> >& dataset) {
    if(dataset.empty()) return;

    int num_rows = dataset.size();
    int num_elements = dataset[0].size();

    // calculate stats
    T sum(0), avg(0);
    vector<T> l1_norms, l2_norms;
    for(const auto& row : dataset) {
        T l1 = l1_norm(row);
        l1_norms.push_back(l1);

        T l2 = l2_norm(row);
        l2_norms.push_back(l2);

        sum += l1;
//            for(const auto& d : row) sum += d;
    }
    avg = sum/(num_rows * num_elements);

    // log
    for(const auto& row : dataset) std::cout << to_string(row) << std::endl;
    std::cout << "l1_norms: " << to_string(l1_norms) << std::endl;
    std::cout << "l2_norms: " << to_string(l2_norms) << std::endl;
    std::cout << "Sum: " << sum << ", Avg: " << avg << std::endl;
}



//-------------------------------------------------------
// unit tests for dataset related functions
template <typename T>
void test_dataset(const int num_rows = 10, const int num_elements = 10, const T min_rand = 3, const T max_rand = 9, const T norm_min = -1, const T norm_max = 1) {
    std::cout << std::endl << "vector_utils::test" << std::endl;

    std::vector< vector<T> > data_orig(num_rows, vector<T>(num_elements));
    std::vector< vector<T> > data_norm;    // normalized data
    std::vector< vector<T> > data_unnorm;   // unnormalized data
    std::vector< vector<T> > data_diff2;   // diff squared between original and unnormalized data
    vector<T> min_values;
    vector<T> max_values;

    // generate data
    {
        // init random generator
        std::default_random_engine rand_engine(std::random_device{}());
        std::uniform_real_distribution<T> rand_dist(min_rand, max_rand);

        // generate data
        std::cout << "generating data ... " << std::endl;
        for(auto& row : data_orig) for(auto& d : row) d = rand_dist(rand_engine);

        // calculate min and max values
        get_range(data_orig, min_values, max_values);
    }

    // original data
    {
        dump_dataset_stats(data_orig);
        std::cout << "min_values: " << to_string(min_values) << std::endl;
        std::cout << "max_values: " << to_string(max_values) << std::endl;
    }

    // normalized data
    {
        std::cout << std::endl << "Normalized" << std::endl;
        data_norm = data_orig;   // to initialize sizes
        for(int i=0; i<num_rows; i++) normalize_dataset(data_orig[i], min_values, max_values, norm_min, norm_max, data_norm[i]);
        dump_dataset_stats(data_norm);
    }

    // unnormalized data
    {
        std::cout << std::endl << "Un-Normalized" << std::endl;
        data_unnorm = data_norm; // to initialize sizes
        for(int i=0; i<num_rows; i++) unnormalize_dataset(data_norm[i], min_values, max_values, norm_min, norm_max, data_unnorm[i]);
        dump_dataset_stats(data_unnorm);
    }

    // squared diff
    {
        std::cout << std::endl << "Squared diff" << std::endl;
        data_diff2 = data_orig; // to initialize sizes
        for(int i=0; i<num_rows; i++) for(int j=0; j<num_elements; j++) { T e = data_orig[i][j] - data_unnorm[i][j]; data_diff2[i][j] = e * e; }
        dump_dataset_stats(data_diff2);
    }


    std::cout << " /vector_utils::test " << std::endl << std::endl << std::endl;
}

}
}
