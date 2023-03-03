#ifndef GUI_FRONTEND_HPP
#define GUI_FRONTEND_HPP

#pragma once

#include <imgui.h>

#include "appcontext.hpp"

namespace TimeBox {

// Utility functions
void CenterWindow(std::size_t, std::size_t);

// Master GUI handler function
void HandleGUI(AppContext &);

// Dialog windows
void MainDialog(AppContext &);
void ConnectDialog(AppContext &);
void AboutDialog(AppContext &);

// Popups
void WarningPopup(AppContext &);

}// namespace TimeBox

#endif// GUI_FRONTEND_HPP