#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <typeinfo>
#include <pthread.h>
#include <chrono>

#define THREAD_NUMBER 4

using namespace std;

float mini[20];
float maxi[20];
double weights[4][21];

struct part{
    int thread_id;
    int numbers;
    string filename;
    int cors = 0;
};

struct part thread_parts_data[THREAD_NUMBER];

double inner_product(double weighs[], float data[], int c){
    double ans(0);
    for (int i = 0; i < c; i++){
        ans += weighs[i] * data[i];
    }
    return ans;
}

int max_index(double scores[]){
    double maximum = scores[0];
    int index = 0;
    for (int i = 1; i < 4; i++){
        if (scores[i] > maximum){
            maximum = scores[i];
            index = i;
        }
    }
    return index;    
}

pair<float, float> minmax(int column, int rows){
    ifstream data("train.csv");
    if (!data.is_open())    {
        cout << "FILE DID NOT OPEN!";
    }
    string test;
    string b;
    getline(data, b, '\n');

    for(int i = 0; i < column; i++){
        getline(data, test, ',');
    }

    getline(data, b, '\n');

    float min;
    float max;
    min = max = stof(test);

    for(int i = 1; i < rows; i++){
        string value;
        for(int j = 0; j < column; j++){
            getline(data, value, ',');
        }

        float v;
        v = stof(value);
        if (v < min){
            min = v;
        }
        if (v > max){
            max = v;
        }
        
        getline(data, b, '\n');
    }
    pair<float, float> ans = {min, max};
    data.close();
    return ans;
}

bool classify(string filename, int row, float mins[], float maxs[], double weights[][21]){
    ifstream data(filename);
    if (!data.is_open()){
        cout << "FILE DID NOT OPEN!";
    }
    string price;
    int price_range;
    string b;
    double scores[4];
    float val[20];
    float va;
    for (int i = 0; i < row; i++){
        getline(data, b, '\n');
    }
    for (int j = 0; j < 20; j++){    
        string v;
        getline(data, v, ',');
        va = stof(v);
        val[j] = (va - mins[j]) / (maxs[j] - mins[j]);
    }
    getline(data, price, '\n');
    for(int k = 0; k < 4; k++){
        double score = 0;
        price_range = stoi(price);
        scores[k] = inner_product(weights[k], val, 20) + weights[k][20];
    }
    data.close();
    if (max_index(scores) == price_range){
        return true;
    } else  {
        return false;
    }
}

void read_weighs(double weights[][21]){
    ifstream weightfile("weights.csv");
    string b;
    getline(weightfile, b, '\n');
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 20; j++){
            string value;
            double v;
            getline(weightfile, value, ',');
            v = stod(value);
            weights[i][j] = v;
        }
        string bias;
        getline(weightfile, bias, '\n');
        weights[i][20] = stod(bias);
    }
    weightfile.close();
}

void* result_corrects(void* p){
    struct part* thread_part = (struct part*) p;
    int k = thread_part->thread_id;
    int n = thread_part->numbers;
    for (int i = 0; i < n; i++){
        if(classify(thread_part->filename, i + 1, mini, maxi, weights)){
            thread_part->cors = thread_part->cors + 1;
        }
    }
    long result = thread_part->cors;
	pthread_exit((void*)result);
}

int main(int argc, char const *argv[]){
    auto start = chrono::high_resolution_clock::now();
    for (int i = 1; i < 21; i++){
        pair<float, float> p = minmax(i, 2000);
        mini[i - 1] = p.first;
        maxi[i - 1] = p.second;
    }
    read_weighs(weights);
    int corrects = 0;
    double accuracy;
    int n = 2000 / THREAD_NUMBER;
    int return_code;
    void* response;
    long output;
    float m1 = 0;
    float* cpp = &m1;

    pthread_t threads[THREAD_NUMBER];
    for (int i = 0; i < THREAD_NUMBER; i++){
        thread_parts_data[i].thread_id = i;
        thread_parts_data[i].numbers = n;
        thread_parts_data[i].filename = "train_" + to_string(i) + ".csv";
        return_code = pthread_create(&threads[i], NULL, result_corrects,
                      (void*)&thread_parts_data[i]);
    }

    for (int i = 0; i < THREAD_NUMBER; i++){
        return_code = pthread_join(threads[i], &response);
        output = (long)response;
        corrects = corrects + output;
    }
    

    accuracy = corrects / 2000.0;
    accuracy = accuracy * 100;
    cout << "Accuracy: ";
    cout << fixed << setprecision(2) << accuracy << '%' << '\n';
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::nanoseconds>(stop - start);
    *cpp = *cpp + duration.count();
    cout << fixed << setprecision(2) << "time : " << *cpp << '\n';
    pthread_exit(NULL);
    return 0;
}
