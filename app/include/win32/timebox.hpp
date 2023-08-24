#ifndef TIMEBOX_HPP
#define TIMEBOX_HPP

#pragma once

#include "timebox_common.hpp"

namespace TimeBox {

void main_loop(SDL_Window *tp_sdl_window, AppContext &tr_app_context, D3DContext &tr_d3d_context);

}// namespace TimeBox

#endif// TIMEBOX_HPP