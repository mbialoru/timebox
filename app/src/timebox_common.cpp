#include "timebox_common.hpp"

using namespace TimeBox;

void TimeBox::SaveHistoryToFile(std::unique_ptr<ClockController> tp_clock_controller)
{
  if (tp_clock_controller != nullptr && !tp_clock_controller->GetDifferenceHistory().empty()) {
    std::fstream output_file;
    output_file.open("timebox_history.log", std::ios::out);
    for (const auto entry : tp_clock_controller->GetDifferenceHistory()) {
      output_file << std::to_string(entry.count()).c_str() << "\n";
    }
    output_file.close();
  }
}