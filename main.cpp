#include <iostream>
#include <string>
#include <fstream>

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

// 解析 json 文件
std::vector<Task> loadTasks(const std::string& filename) {
  // 打开名为 filename 的文件，存储在 file 中方便后续操作
  std::ifstream file(filename);
  std::string json;
  std::vector<Task> tasks;

  // 无法打开文件的情形
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return tasks;
  }

  // 将 file 中的内容读取到 json 中，直到出现文字终止符 '\0'
  std::getline(file, json, '\0');

  size_t pos = 0;
  // 从 json 中找到第一个，将索引赋值给 pos，如果没有找到的话，find 返回值即为 std::string::npos
  // 这样循环就能读取 json 中所有的任务
  while ((pos = json.find("{")) != std::string::npos) {
    Task task;
    json = json.substr(pos + 1); // 从 { 后的子串开始解析
    task.id = std::stoi(json.substr(json.find(":") + 1, json.find(","))); // std::stoi 将字符串转换为整数，读取 ":" 和 "," 之间的 id
    json = json.substr(json.find("description") + 13); // find 返回值为 d 的索引，跳过 10 个字母，"，:， 得到13
    task.description = json.substr(0, json.find("\""));
    json = json.substr(json.find("status") + 9);
    task.status = json.substr(0, json.find("\""));
    tasks.push_back(task);
    json = json.substr(json.find("}") + 1);
  }
  return tasks;
}

// 将新添加的任务保存进 json 文件
void saveTasks(const std::vector<Task>& tasks, const std::string& filename) {
   std::ofstream file(filename);
   file << "[\n";
   
   // 写入文件
   for (size_t i = 0; i < tasks.size(); ++i) {
    file << "  " << tasks[i].toJson();
    if (i != tasks.size() - 1) {
      file << ",";
      file << "\n";
    }
   }
   file << "]";
}

int main(int argc, char *argv[]) {
  // 输入参数过少
  if (argc < 2) {
    std::cout << "Usage: task-cli <command> [arguments...]" << std::endl;
    return 1;
  }

  std::string command = argv[1];
  if (command == "add") {
    // 未提供 description 参数
    if (argc == 2) {
      std::cout << "Error: No description provided." << std::endl;
      return 1;
    }
    // description 参数过多
    if (argc >= 4) {
      std::cout << "Error: There are too many descriptions." << std::endl;
      return 1;
    }

    // 添加任务
    std::string description = argv[2];
    std::vector<Task> tasks = loadTasks("tasks.json");
    int newID = tasks.empty()? 1 : tasks.back().id + 1;
    tasks.push_back({newID, description, "todo"});
  }
}