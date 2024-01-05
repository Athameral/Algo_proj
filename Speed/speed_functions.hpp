/*****************************************************************//**
 * \file   speed_functions.hpp
 * \brief  在plain_functions.hpp基础上，对float类型模板特化，用sse进行优化
 *
 * \author sunny
 * \date   January 2024
 *********************************************************************/
#include <array>
#include <iostream>
#include "main.h"

// 控制区间大小为多少时从快排切换为简单插入
constexpr auto CUT_OFF = 8;

/**
 * \brief 最朴素的方式给出一个数组的最大值.
 *
 * \param arr
 * \return
 */
template<typename T, size_t N>
typename std::array<T, N>::iterator getMaxIter(std::array<T, N>& arr)
{
	auto v = arr.begin();
	for (auto i = arr.begin(); i != arr.end(); ++i)
	{
		if (*i > *v)
			v = i;
	}
	return v;
}

/**
 * \brief 最朴素的方式对一个数组求和.
 * 如果使用/fp:fast选项，可以自动向量化
 *
 * \param arr
 * \return
 */
template<typename T, size_t N>
typename T getSum(std::array<T, N>& arr)
{
	T sum = 0;
	for (auto& e : arr)
		sum += e;
	return sum;
}

template <typename T, size_t N>
void insertionSort(std::array<T, N>& arr, typename std::array<T, N>::iterator low, typename std::array<T, N>::iterator high)
{
	for (auto i = low + 1; i != high; ++i)
	{
		auto key = *i;
		auto j = i;
		while (j > low && *(j - 1) > key)
		{
			*j = *(j - 1);
			--j;
		}
		*j = key;
	}
}

/**
 * \brief 一个经过简易优化的快速排序函数，在排序元素小于等于8个时切换至简单插入排序。
 * 选择8这个数字参考了微软的qsort.cpp文件，其中有说明，感兴趣可以去看看。
 *
 * \param arr 要排序的数组
 * \param low 排序起点（包括）
 * \param high 排序终点（不包括）
 */
template <typename T, size_t N>
void quickSort(std::array<T, N>& arr, typename std::array<T, N>::iterator low, typename std::array<T, N>::iterator high)
{
	if (high - low <= 1)
		return;
	else if (high - low <= CUT_OFF)
	{
		insertionSort(arr, low, high);
	}
	else
	{
		auto pivot = low + (high - low) / 2;
		std::swap(*pivot, *(high - 1));

		auto i = low;
		auto j = high - 1;
		while (true)
		{
			while (*i < *(high - 1))
				++i;
			while (*j > *(high - 1))
				--j;
			if (i >= j)
				break;
			std::swap(*i, *j);
			++i;
			--j;
		}
		std::swap(*i, *(high - 1));

		quickSort(arr, low, i);
		quickSort(arr, i + 1, high);
	}
}

/**
 * 求最大值迭代器的模板，针对float特化。
 *
 * \param arr
 * \return
 */
template<size_t N>
typename std::array<float, N>::iterator getMaxIter(std::array<float, N>& arr)
{
	std::array<size_t, MAX_THREADS> medium{};
	//medium.fill(0);
	auto&& len = arr.size();
#pragma omp parallel num_threads(MAX_THREADS)
	{
		auto thread_id = omp_get_thread_num();
#pragma omp for schedule(static)
		for (auto i = 0; i < len; ++i)
		{
			if (arr[medium[thread_id]] < arr[i])
				medium[thread_id] = i;
		}
	}

	size_t ret = 0;
	for (auto& e : medium)
		if (arr[ret] < arr[e])
			ret = e;
	return arr.begin() + ret;
}
