#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <typeinfo>

using namespace std;

struct part{
    int start;
    int numbers;
    string filename;
    float mins[20];
    float maxs[20];
    double weights[4][21];
};


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

int result_corrects(part p){
    int s = p.start;
    int n = p.numbers;
    int cors = 0;
    for (int i = s; i < s + n; i++){
        if(classify(p.filename, i + 1, p.mins, p.maxs, p.weights)){
            cors = cors + 1;
        }
    }
    
}

int main(int argc, char const *argv[]){
    float mini[20];
    float maxi[20];
    for (int i = 1; i < 21; i++){
        pair<float, float> p = minmax(i, 2000);
        mini[i - 1] = p.first;
        maxi[i - 1] = p.second;
    }
    double weights[4][21];
    read_weighs(weights);
    int corrects = 0;
    double accuracy;
    // threads.
    accuracy = corrects / 2000.0;
    accuracy = accuracy * 100;
    cout << "Accuracy: ";
    cout << fixed << setprecision(2) << accuracy << '%' << '\n';
    return 0;
}
