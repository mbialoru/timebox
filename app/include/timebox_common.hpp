#ifndef TIMEBOX_COMMON_HPP
#define TIMEBOX_COMMON_HPP

#pragma once

#include <fstream>

#include "appcontext.hpp"
#include "clockcontroller.hpp"
#include "config.hpp"
#include "gui_backend.hpp"
#include "gui_frontend.hpp"

namespace TimeBox {

void SaveHistoryToFile(std::unique_ptr<ClockController>);

}// namespace TimeBox

#endif// TIMEBOX_COMMON_HPP