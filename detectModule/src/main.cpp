#include "float.hpp"
#include "getresult.hpp"
#include "json.hh"
#include <fstream>
#include <chrono>

using std::cin;
using std::cout;
typedef union {
  int i;
  float f;
} FL;

typedef union {
  long int i;
  double d;
} DL;

using json = nlohmann::json;
int main(int argc, char *argv[]) {
  if (argc == 4) {
    double iidx = atoi(argv[1]);
    double start = atof(argv[2]);
    double end = atof(argv[3]);
    
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point finishTime;
    std::chrono::duration<double> elapsedTime;

    startTime = std::chrono::high_resolution_clock::now();

    // cout << iidx << ". ";
    // if (start > end) {
    //   cout << "start > end, input range error, you have to input start <= end"
    //        << endl;
    //   return 0;
    // } else if (end <= 0) { // 处理start和end<=0的情况
    //   DoubleFunction::detectMethod2_1(start, end);
    // } else if (start < 0 &&
    //            end > 0) { // 处理start<0但是end>0的情况，即区间跨越了0
    //   DoubleFunction::detectMethod2_2(start, end);
    // } else { // 处理start和end都大于等于0的情况
    //   DoubleFunction::detectMethod2(start, end);
    // }

    std::ifstream file("inputs/range_float32_hsed.json");
    json jsonData;
    file >> jsonData;

    // 用于存储 func_idx 的向量
    std::vector<int> func_indices;

    // 提取并转换 func_idx 为整数并存储在向量中
    for (const auto &[func_idx, _] : jsonData.items()) {
      func_indices.push_back(std::stoi(func_idx));
    }
    std::string result_key = "hsed";
    // 按照整数顺序对 func_idx 排序
    std::sort(func_indices.begin(), func_indices.end());

    for (int idx : func_indices) {
      if (idx != iidx) {
        continue;
      }
      // std::cout << "Processing FuncIndex: " << idx << std::endl;
      auto &ranges = jsonData[std::to_string(idx)];

      for (auto &range_item : ranges) { // 注意这里的 auto& 来修改 input_item
        auto &info = range_item[1];
        std::vector<double> inputs =
        info["inputs"].get<std::vector<double>>();

        std::vector<double> results;
        for (auto &input : inputs) {
            results.push_back(getULP(input, getDoubleOfOrigin(input)));
        }

        // 将 results 写入 jsonData
        info[result_key] = results;
      }
    }
    std::ofstream outFile("inputs/range_float32_hsed.json");
    outFile << std::setw(4) << jsonData
            << std::endl; // 使用 std::setw(4) 使 JSON 更加美观
    outFile.close();

    // double x = atof(argv[1]);
    // cout << getULP(x, getDoubleOfOrigin(x)) << endl;
    finishTime = std::chrono::high_resolution_clock::now();
    elapsedTime = finishTime - startTime;
    // std::cout << "time: " << elapsedTime.count() << " | " << std::endl;

  } else if (argc == 5) { // 双参算术表达式

  } else {
    cout << "please input 3 or 5 numbers" << endl;
  }
  return 0;
}
