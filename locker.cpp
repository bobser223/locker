#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <cstdint>

//~/storage/documents/locker_numbers/
constexpr const char* LEFT_FILE   = "~/storage/documents/locker_numbers/left.txt";
constexpr const char* MIDDLE_FILE = "~/storage/documents/locker_numbers/middle.txt";
constexpr const char* RIGHT_FILE  = "~/storage/documents/locker_numbers/right.txt";

const char* file_for(const std::string& pos) {
  if (pos == "left")   return LEFT_FILE;
  if (pos == "middle") return MIDDLE_FILE;
  if (pos == "right")  return RIGHT_FILE;
  return nullptr;
}

void add(size_t key_number, const std::string& hostess_position) {

  const char* file_name = file_for(hostess_position);

  std::ofstream fin(file_name, std::ios::app);
  if (!fin)
    printf("Can not open the file_name %s\n", file_name);
  fin.seekp(0, std::ios::end); // teleporting to the end;

  fin << key_number << std::endl;

  printf("added %zu number with %s hostess position\n", key_number, hostess_position.c_str());
  fin.close();
  free((void *) file_name);
}

void pop(const std::string& hostess_position) {

  const char* file_name = file_for(hostess_position);

  std::ifstream file(file_name);

  std::vector<std::string> lines;
  std::string line;
  while(std::getline(file, line)){
      lines.push_back(line);
  }
  file.close();

  if(!lines.empty()){
    lines.pop_back(); //deleting last one;
  }


  std::ofstream fin(file_name, std::ios::trunc); // overwrite file
  for (const auto &l : lines) {
    fin << l << std::endl;
  }
  fin.close();
  free((void*) file_name);

}

void FAQ(){
  printf("Available commands:\n"
         "  add <left|middle|right> <key>\n"
         "  pop <left|middle|right>\n");
}



int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Usage: locker <command> [args]\n");
    FAQ();
    return 1;
  }

  std::string command = argv[1];

  if (command == "add"){
    if (argc < 4) {
      printf("Usage: locker add <left|middle|right> <key>\n");
      return 1;
    }

    std::string pos = argv[2];
    if (pos.empty()){
      printf("no hostess position\n");
      return 1;
    }

    std::string key_number_str = argv[3];
    std::uint64_t key{};
    try {
      key = std::stoull(argv[3]);
    } catch (...) {
      printf("invalid kay number: %s \n", key_number_str.c_str());
      return 1;
    }

    add(key, pos);
  } else if (command == "pop"){
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
