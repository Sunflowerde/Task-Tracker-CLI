#include <iostream>
#include <string>

struct Task {
  int id; // 用于表示每个任务的编号
  std::string description; // 每项任务的具体描述
  std::string status; // 每项任务的进行状态，包括 todo, done, in-progress

  // 将 struct 转为 json 的函数
  std::string toJson() const {
    return "{\n  \"id\": " + std::to_string(id) + ",\n" + 
    "  \"description\": \"" + description + "\",\n" + 
    "  \"status\": \"" + status + "\"\n}";
  }
};

int main() {
  Task t = {1, "do homework", "todo"};
  std::cout << t.toJson() << std::endl;
  return 0;
}