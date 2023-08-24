#ifndef GUI_FRONTEND_HPP
#define GUI_FRONTEND_HPP

#pragma once

#include <imgui.h>

#include "appcontext.hpp"

namespace TimeBox {

// utilities
void center_window(std::size_t t_height, std::size_t t_width);
void handle_gui(AppContext &tr_context);

// dialogs / popups
void about_dialog(AppContext &tr_context);
void connect_dialog(AppContext &tr_context);
void main_dialog(AppContext &tr_context);
void warning_popup(AppContext &tr_context);

}// namespace TimeBox

#endif// GUI_FRONTEND_HPP