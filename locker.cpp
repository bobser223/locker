#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <cstdint>
#include <cstdlib>   // getenv
#include <array>
#include <map>
#include <iostream>
#include <iomanip>
#include <map>
#include <numeric>
#include <cmath>


std::vector<std::string> ALL_HOSTESS_POSITIONS = {"left", "middle", "right"};

struct three_digit_statistics{
    std::array<std::uint64_t, 10> first  = {};
    std::array<std::uint64_t, 10> second = {};
    std::array<std::uint64_t, 10> third  = {};

    void operator+=(const three_digit_statistics& other){
      for (uint8_t i = 0; i < 10; i++){
        this->first[i] += other.first[i];
      }
      for (uint8_t i = 0; i < 10; i++){
        this->second[i] += other.second[i];
      }

      for (uint8_t i = 0; i < 10; i++){
        this->third[i] += other.third[i];
      }
    }

    uint8_t most_popular_third() const noexcept {
      uint8_t most_popular_third_digit = 0;
      for (uint8_t i = 1; i < 10; i++){
        if (third[most_popular_third_digit] < third[i])
          most_popular_third_digit = i;
      }
      return most_popular_third_digit;
    }

    uint8_t most_popular_second() const noexcept {
      uint8_t most_popular_second_digit = 0;
      for (uint8_t i = 1; i < 10; i++){
        if (second[most_popular_second_digit] < second[i])
          most_popular_second_digit = i;
      }
      return most_popular_second_digit;
    }

    uint8_t most_popular_first() const noexcept {
      uint8_t most_popular_first_digit = 0;
      for (uint8_t i = 1; i < 10; i++){
        if (first[most_popular_first_digit] < first[i])
          most_popular_first_digit = i;
      }
      return most_popular_first_digit;
    }

};


static std::string base_dir() {
  const char* home = std::getenv("HOME");
  std::string h = home ? home : "";
  std::string d1 = h + "/storage/shared/Documents/locker_numbers";
  std::string d2 = h + "/storage/shared/locker_numbers";

  std::ifstream t1(d1 + "/.probe"); bool docs = !!t1;
  return docs ? d1 : d2;
}

static std::string file_for(const std::string& pos) {
  const std::string base = base_dir();
  if (pos == "left" || pos == "l")   return base + "/left.txt";
  if (pos == "middle" || pos == "m") return base + "/middle.txt";
  if (pos == "right"|| pos == "r")  return base + "/right.txt";
  return {};
}

static std::vector<std::uint64_t> get_all_keys(const std::string& hostess_position){

    if(hostess_position == "all"){
        std::vector<std::uint64_t> all;
        for (auto& hostess_pos: ALL_HOSTESS_POSITIONS){
            auto part = get_all_keys(hostess_pos);
            all.insert(all.end(), part.begin(), part.end());
        }
        return all;
    }

    std::vector<std::uint64_t> vec;

    const std::string file_name = file_for(hostess_position);
    if (file_name.empty()) { std::cerr << "Unknown position\n"; return {}; }
    std::ifstream fin(file_name);

    if (!fin) { std::cerr << "Cannot open " << file_name << "\n"; return {}; }
    std::string line;

    while(std::getline(fin, line)){
        try{
            std::uint64_t curr_key = std::stoull(line);
            vec.push_back(curr_key);

        } catch(...){
            std::cout  << "invalid key number: " << line << " so, it was ignored" << "\n";
        }
    }
    return vec;
}

static void print_distribution(const std::map<std::uint64_t, std::uint64_t>& counts,
                               const char* title) {
    std::uint64_t sum = 0;
    for (auto& [num, cnt] : counts) sum += cnt;

    std::cout << "\n" << title << " (n=" << sum << ")\n";
    std::cout << " value |  count | percent | histogram\n";
    std::cout << "---------------------------------------\n";

    for (auto& [num, cnt] : counts) {
        double pct = sum ? (100.0 * static_cast<double>(cnt) / static_cast<double>(sum)) : 0.0;
        int bars = static_cast<int>(std::round(pct / 2.0)); // шкала ~50 символів

        std::cout << std::setw(6) << num << " | "
                  << std::setw(6) << cnt << " | "
                  << std::setw(6) << std::fixed << std::setprecision(1) << pct << "% | ";
        for (int i = 0; i < bars; ++i) std::cout << "█";
        std::cout << '\n';
    }
}

static std::map<std::uint64_t, std::uint64_t>
get_frequency_map(const std::vector<std::uint64_t>& numbers) {
  std::map<std::uint64_t, std::uint64_t> frequency_map;
  for (auto num : numbers) frequency_map[num]++;
  return frequency_map;
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
               "  pop <left|middle|right>\n"
               "  stat <all|left|middle|right>";
}

int add_3_digit_key_to_statistic(three_digit_statistics& curr_stat, std::uint64_t three_digit_key){

  if (three_digit_key / 1000 ) // more than 3 digit num;
    return 1;

  curr_stat.first[three_digit_key%10]++;
  curr_stat.second[three_digit_key/10%10]++;
  curr_stat.third[three_digit_key/100%10]++;
  return 0;
}

static int statistics_three_digit(const std::string& hostess_position, three_digit_statistics& curr_stat){

  auto all_keys = get_all_keys(hostess_position);

  for(auto key: all_keys){

      if (add_3_digit_key_to_statistic(curr_stat, key)){
        std::cout << "This is not a three digit key: " << key << " , call another func\n";
      }
  }



  return 0;
}

static void statistics_three_digit(const std::string& hostess_position){
  three_digit_statistics curr_stat;
  statistics_three_digit(hostess_position, curr_stat);

  std::map<std::uint64_t, std::uint64_t> dist_first;
  std::map<std::uint64_t, std::uint64_t> dist_second;
  std::map<std::uint64_t, std::uint64_t> dist_third;

  for (uint8_t i = 0; i < 10; i++){
    dist_first[i] = curr_stat.first[i];
    dist_second[i] = curr_stat.second[i];
    dist_third[i] = curr_stat.third[i];
  }

  std::cout << "\n=== Distribution for position: " << hostess_position << " ===\n";

  print_distribution(dist_third, "Hundreds digit");
  print_distribution(dist_second, "Tens digit");
  print_distribution(dist_first, "Ones digit");

  std::cout << "\nMost popular digits:\n";
  std::cout << "  Hundreds: " << static_cast<int>(curr_stat.most_popular_third()) << "\n";
  std::cout << "  Tens:     " << static_cast<int>(curr_stat.most_popular_second()) << "\n";
  std::cout << "  Ones:     " << static_cast<int>(curr_stat.most_popular_first()) << "\n";
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
  } else if (command == "stat"){
      if (argc == 2){
          statistics_three_digit("all");
      } else {
        statistics_three_digit(argv[2]);
      }

  } else {
    FAQ();
    return 1;
  }
}
