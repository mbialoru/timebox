#ifndef TIMEBOX_HPP
#define TIMEBOX_HPP

#pragma once

#include <fstream>

#include "appcontext.hpp"
#include "clockcontroller.hpp"
#include "config.hpp"
#include "gui_backend.hpp"
#include "gui_frontend.hpp"

namespace TimeBox {

void SaveHistoryToFile(std::unique_ptr<ClockController>);
void MainLoop(SDL_Window *, AppContext &, D3DContext &);

}// namespace TimeBox

#endif// TIMEBOX_HPP