#ifndef GUI_FRONTEND_HPP
#define GUI_FRONTEND_HPP

#pragma once

#include <imgui.h>

#include "appcontext.hpp"

namespace TimeBox {

void about_dialog(AppContext &);
void center_window(std::size_t, std::size_t);
void connect_dialog(AppContext &);
void handle_gui(AppContext &);
void main_dialog(AppContext &);
void warning_popup(AppContext &);

}// namespace TimeBox

#endif// GUI_FRONTEND_HPP