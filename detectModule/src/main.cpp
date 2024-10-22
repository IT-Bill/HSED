#include "float.hpp"
#include "json.hh"
#include "getresult.hpp"
#include <algorithm>
#include <functional>
#include <string>
#include <utility>
#include <fstream>
#include <vector>
#include <iomanip>
#include <chrono>

using std::cin;
using std::cout;
using nlohmann::json;

std::vector<double> layer2Input;

typedef union {
    int i;
    float f;
} FL;

typedef union {
    long int i;
    double d;
} DL;

json loadJson(const std::string &path) {
  json data;
  std::ifstream file(path);

  if (!file) {
    std::cerr << "Error opening JSON file: " << path << std::endl;
    exit(EXIT_FAILURE);
  }
  file >> data;
  return data;
}

void saveJson(const std::string &path, json &data) {
    std::ofstream file(path);
    file << std::setw(2) << data << std::endl;
}

int main(int argc, char *argv[]) {

    if (argc == 3) {
        std::string funcIndex = argv[1];
        json jsonData = loadJson(argv[2]);
        auto &funcData = jsonData[funcIndex];

        // !======================================================================================
        auto intervals = funcData["ranges"].get<std::vector<std::pair<double, double>>>();
        if (intervals.empty()) {
            return 1;
        }

        auto startTime = std::chrono::steady_clock::now();

        double ULP = 0.0;
        double input_x, start, end;
        std::pair<double, double> xuPair;
        for (const auto &interval : intervals) {
            double s = interval.first, e = interval.second;
            if (e <= 0) {//处理start和end<=0的情况
                xuPair = DoubleFunction::processNegativeRangeLayer1(s, e);
            } else if (s < 0 && e > 0) {//处理start<0但是end>0的情况，即区间跨越了0
                xuPair = DoubleFunction::processCrossZeroLayer1(s, e);
            } else {//处理start和end都大于等于0的情况
                xuPair = DoubleFunction::processPositiveRangeLayer1(s, e); 
            }
            if (ULP < xuPair.second) {
                input_x = xuPair.first;
                ULP = xuPair.second;
                start = s;
                end = e;
            }
        }
        auto ckptTime = std::chrono::steady_clock::now();
        

        cout << "Seleted Interval: [" << start << ", " << end << "]" << endl;
        // cout << "ULP: " << ULP << ", " << "x: " << input_x << endl;
        if (end <= 0) {//处理start和end<=0的情况
            DoubleFunction::processNegativeRangeLayer23(input_x, ULP, start, end);
        } else if (start < 0 && end > 0) {//处理start<0但是end>0的情况，即区间跨越了0
            DoubleFunction::processCrossZeroLayer23(input_x, ULP, start, end);
        } else {//处理start和end都大于等于0的情况
            DoubleFunction::processPositiveRangeLayer23(input_x, ULP, start, end); 
        }
        // for (auto input : layer2Input) {
        //     std::cout << input << std::endl;
        // }
        if (!layer2Input.empty()) {
            std::ofstream layer2InputFile("/HSED/layer2/" + funcIndex + ".json");
            std::sort(layer2Input.begin(), layer2Input.end());
            json layer2InputJson = layer2Input;
            layer2InputFile << std::setw(2) << layer2InputJson << std::endl;
        }

        auto finishTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsedTime = finishTime - startTime;
        std::chrono::duration<double> layer1Time = ckptTime - startTime;
        std::cout << "Layer1 Time: " << layer1Time.count() << std::endl;
        std::cout << "Elapsed Time: " << elapsedTime.count() << std::endl;
        cout << "-----------------------------------------------------------------------------" << endl;


        // !======================================================================================
        
        // auto inputsList = funcData["inputs"].get<std::vector<std::vector<double>>>();
        // auto resultsList = funcData["results"].get<std::vector<std::vector<double>>>();
        // std::vector<std::vector<double>> errorsList;
        
        // for (int i = 0; i < inputsList.size(); i++) {
        //     auto inputs = inputsList[i];
        //     auto results = resultsList[i];
        //     std::vector<double> errors(inputs.size());
        //     std::transform(inputs.begin(), inputs.end(), results.begin(), errors.begin(), 
        //         [](double input, double result) { return getULP(input, result); });
            
        //     errorsList.push_back(errors);
        // }
        // funcData["hsed"] = errorsList;
        // saveJson(argv[2], jsonData);

    } else {
        cout << "please input 3 or 5 numbers" << endl;
    }
    return 0;
}
