#include "utilities_common.hpp"

using namespace TimeBox;

std::string TimeBox::RenderNonPrintableCharacters(const std::string &t_input_string)
{
  std::string result;
  std::string character_represent;
  for (auto &character : t_input_string) {
    switch (static_cast<int>(character)) {
    case 0:// NULL
      character_represent = "\\0";
      break;
    case 9:// Tabulator
      character_represent = "\\t";
      break;
    case 10:// Line Feed LF
      character_represent = "\\n";
      break;
    case 13:// Carriage Return CR
      character_represent = "\\r";
      break;
    case 32:// Whitespace
      character_represent = "\\s";
      break;
    default:
      character_represent = character;
      break;
    }
    result.append(character_represent);
    character_represent.clear();
  }
  return result;
}

std::string TimeBox::RenderStringASCIICodes(const std::string &t_input_string)
{
  std::string result;
  for (auto &character : t_input_string) {
    result.append(std::to_string(static_cast<int>(character)));
    result.append(" ");
  }
  result.pop_back();
  return result;
}

bool TimeBox::CheckIfUsingDocker()
{
  if (std::filesystem::exists(std::filesystem::path("/.dockerenv"))) {
    return true;
  } else {
    return false;
  }
}

std::chrono::system_clock::time_point TimeBox::ConvertStringToTimepoint(const std::string t_time_string)
{
  std::vector<std::string> tmp;
  time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  struct tm tm = *std::localtime(&now);

  boost::split(tmp, t_time_string, boost::is_any_of(":."));

  tm.tm_hour = std::stoi(tmp[0]);
  tm.tm_min = std::stoi(tmp[1]);
  tm.tm_sec = std::stoi(tmp[2]);

  auto res = std::chrono::system_clock::from_time_t(std::mktime(&tm));
  res += std::chrono::duration<int, std::milli>(std::stoi(tmp[3]) * 10);

  return res;
}

std::string TimeBox::ConvertTimepointToString(const std::chrono::system_clock::time_point t_timepoint)
{
  time_t tmp = std::chrono::system_clock::to_time_t(t_timepoint);
  struct tm tm = *std::localtime(&tmp);
  std::string res{ std::to_string(tm.tm_hour) + ":" + std::to_string(tm.tm_min) + ":" + std::to_string(tm.tm_sec) };

  return res;
}