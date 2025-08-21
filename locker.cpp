#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <cstdint>
#include <cstdlib>   // getenv

// КУДИ ПИСАТИ ФАЙЛИ:
// спершу пробуємо спільну теку Documents, інакше просто в домашню
static std::string base_dir() {
  const char* home = std::getenv("HOME");
  std::string h = home ? home : "";
  std::string d1 = h + "/storage/shared/Documents/locker_numbers";
  std::string d2 = h + "/storage/shared/locker_numbers";
  // Не створюємо тут теку через <filesystem>, а покладаємося на крок 2)
  // Вибираємо те, що існує, або d1 за замовчуванням
  std::ifstream t1(d1 + "/.probe"); bool docs = !!t1;
  return docs ? d1 : d2;
}

static std::string file_for(const std::string& pos) {
  const std::string base = base_dir();
  if (pos == "left")   return base + "/left.txt";
  if (pos == "middle") return base + "/middle.txt";
  if (pos == "right")  return base + "/right.txt";
  return {};
}

static void add(std::uint64_t key_number, const std::string& hostess_position) {
  const std::string file_name = file_for(hostess_position);
  if (file_name.empty()) { std::cerr << "Unknown position\n"; return; }

  std::ofstream fout(file_name, std::ios::app);
  if (!fout) { std::cerr << "Cannot open " << file_name << "\n"; return; }

  fout << key_number << '\n';
  std::cout << "added " << key_number
            << " number with " << hostess_position << " hostess position\n";
}

static void pop(const std::string& hostess_position) {
  const std::string file_name = file_for(hostess_position);
  if (file_name.empty()) { std::cerr << "Unknown position\n"; return; }

  std::ifstream fin(file_name);
  if (!fin) { std::cerr << "Cannot open " << file_name << "\n"; return; }

  std::vector<std::string> lines;
  std::string line;
  while (std::getline(fin, line)) lines.push_back(line);
  fin.close();

  if (!lines.empty()) lines.pop_back();

  std::ofstream fout(file_name, std::ios::trunc);
  for (const auto& l : lines) fout << l << '\n';
}

static void FAQ() {
  std::cout << "Available commands:\n"
               "  add <left|middle|right> <key>\n"
               "  pop <left|middle|right>\n";
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: locker <command> [args]\n";
    FAQ();
    return 1;
  }

  std::string command = argv[1];

  if (command == "add") {
    if (argc < 4) {
      std::cout << "Usage: locker add <left|middle|right> <key>\n";
      return 1;
    }
    const std::string pos = argv[2];
    const std::string key_number_str = argv[3];
    std::uint64_t key{};
    try {
      key = std::stoull(key_number_str);
    } catch (...) {
      std::cout << "invalid key number: " << key_number_str << "\n";
      return 1;
    }
    add(key, pos);
  } else if (command == "pop") {
    if (argc < 3) {
      std::cerr << "Usage: locker pop <left|middle|right>\n";
      return 1;
    }
    pop(argv[2]);
  } else {
    FAQ();
    return 1;
  }
}
