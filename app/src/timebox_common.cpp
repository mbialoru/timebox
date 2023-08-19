#include "timebox_common.hpp"

using namespace TimeBox;

void TimeBox::save_history_to_file(std::unique_ptr<ClockController> tp_clock_controller)
{
  if (tp_clock_controller != nullptr && !tp_clock_controller->get_difference_history().empty()) {
    std::fstream output_file;
    output_file.open("timebox_history.log", std::ios::out);
    for (const auto &entry : tp_clock_controller->get_difference_history()) {
      output_file << std::to_string(entry.count()).c_str() << "\n";
    }
    output_file.close();
  }
}