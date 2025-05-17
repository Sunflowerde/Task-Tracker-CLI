#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstdlib>

struct Task {
  int id;
  std::string description;
  std::string status;

  std::string toJson() const {
    return "{\n    \"id\": " + std::to_string(id) + ",\n" + 
           "    \"description\": \"" + description + "\",\n" + 
           "    \"status\": \"" + status + "\"\n  }";
  }
};

std::string getHomeDir() {
  const char *home = std::getenv("HOME");
  return home ? std::string(home) : "";
}

// 解析 json 文件，不破坏原字符串
std::vector<Task> loadTasks(const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    // 文件不存在，返回空 vector
    return {};
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string json = buffer.str();

  std::vector<Task> tasks;
  size_t pos = 0;
  while ((pos = json.find("{", pos)) != std::string::npos) {
    Task task;

    // 找 id
    size_t id_pos = json.find("\"id\"", pos);
    if (id_pos == std::string::npos) break;
    size_t colon_pos = json.find(":", id_pos);
    size_t comma_pos = json.find(",", id_pos);
    if (colon_pos == std::string::npos || comma_pos == std::string::npos) break;
    std::string id_str = json.substr(colon_pos + 1, comma_pos - colon_pos -1);
    task.id = std::stoi(id_str);

    // 找 description
    size_t desc_pos = json.find("\"description\"", pos);
    if (desc_pos == std::string::npos) break;
    size_t desc_start = json.find("\"", desc_pos + 13); // 找第一个引号
    size_t desc_end = json.find("\"", desc_start + 1);
    if (desc_start == std::string::npos || desc_end == std::string::npos) break;
    task.description = json.substr(desc_start + 1, desc_end - desc_start - 1);

    // 找 status
    size_t status_pos = json.find("\"status\"", pos);
    if (status_pos == std::string::npos) break;
    size_t status_start = json.find("\"", status_pos + 8);
    size_t status_end = json.find("\"", status_start + 1);
    if (status_start == std::string::npos || status_end == std::string::npos) break;
    task.status = json.substr(status_start + 1, status_end - status_start - 1);

    tasks.push_back(task);

    // 更新 pos 跳到当前 } 后面，准备找下一个任务
    pos = json.find("}", status_end);
    if (pos == std::string::npos) break;
    pos += 1;
  }

  return tasks;
}

void saveTasks(const std::vector<Task>& tasks, const std::string& filename) {
  std::ofstream file(filename);
  file << "[\n";
  for (size_t i = 0; i < tasks.size(); ++i) {
    file << "  " << tasks[i].toJson();
    if (i != tasks.size() - 1) {
      file << ",";
    }
    file << "\n";
  }
  file << "]";
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: task-cli <command> [arguments...]\n";
    std::cout << "Commands:\n  add <description>\n  update <id> <new_description>\n  list\n";
    return 1;
  }

  std::string command = argv[1];
  std::string filename = getHomeDir() + "/tasks.json";
  auto tasks = loadTasks(filename);

  if (command == "add") {
    if (argc != 3) {
      std::cerr << "Usage: task-cli add <description>\n";
      return 1;
    }
    std::string description = argv[2];
    int newID = tasks.empty() ? 1 : tasks.back().id + 1;
    tasks.push_back({newID, description, "todo"});
    saveTasks(tasks, filename);
    std::cout << "Task added successfully (ID: " << newID << ")\n";

  } else if (command == "update") {
    if (argc != 4) {
      std::cerr << "Usage: task-cli update <id> <new_description>\n";
      return 1;
    }
    int id = std::stoi(argv[2]);
    std::string newDescription = argv[3];
    bool found = false;
    for (auto& task : tasks) {
      if (task.id == id) {
        task.description = newDescription;
        found = true;
        break;
      }
    }
    if (!found) {
      std::cerr << "Error: Task with ID " << id << " not found\n";
      return 1;
    }
    saveTasks(tasks, filename);
    std::cout << "Task updated successfully (ID: " << id << ")\n";

  } else if (command == "list") {
    if (tasks.empty()) {
      std::cout << "No tasks found.\n";
      return 0;
    }
    for (const auto& task : tasks) {
      std::cout << "ID: " << task.id
                << ", Description: " << task.description
                << ", Status: " << task.status << "\n";
    }

  } else {
    std::cerr << "Unknown command: " << command << "\n";
    return 1;
  }

  return 0;
}
