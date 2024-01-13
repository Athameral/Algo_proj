/*****************************************************************//**
 * \file   main.h
 * \brief  一些设置内容，包括最大线程数，子任务数目等
 * 
 * \author sunny
 * \date   January 2024
 *********************************************************************/
#pragma once

constexpr auto MAX_THREADS = 4;
constexpr auto SUBDATANUM = 80000;
constexpr auto DATANUM = MAX_THREADS * SUBDATANUM;