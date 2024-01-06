/*****************************************************************//**
 * \file   speed_functions.hpp
 * \brief  在plain_functions.hpp基础上，对float类型模板特化，用AVX2进行优化
 *
 * \author sunny
 * \date   January 2024
 *********************************************************************/
#include <array>
#include <vector>
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
 * 如果使用/fp:fast选项，可以自动向量化，但会损失精度。
 * 不过使用AVX2对浮点求和，本身就会损失精度，何况误差在5%以内
 * 因此MSVC只会对/fp:fast应用AVX2，我认为很合理
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

/**
 * 求和的模板，针对float特化。
 * 使用OpenMP加速，同时可以AVX2向量化，尽管和有AVX2的单线程版本结果相差不大
 * \param arr
 * \return
 */
template<size_t N>
float getSum(std::array<float, N>& arr)
{
	std::array<float, MAX_THREADS> medium{};
	auto&& len = arr.size();
#pragma omp parallel num_threads(MAX_THREADS)
	{
		auto thread_id = omp_get_thread_num();
#pragma omp for schedule(static)
		for (auto i = 0; i < len; ++i)
		{
			medium[thread_id] += arr[i];
		}

	}
	float ret = 0.0F;
	for (auto& e : medium)
		ret += e;
	return ret;
}

template<size_t N>
void quickSort(std::array<float, N>& arr, typename std::array<float, N>::iterator low, typename std::array<float, N>::iterator high)
{
	const auto size = arr.size();
	const auto len = size / MAX_THREADS;
#pragma omp parallel num_threads(MAX_THREADS > 16 ? 16:MAX_THREADS)
	// 过多线程在归并时会有额外负担
	{
#pragma omp for schedule(static)
		for (auto i = 0; i < MAX_THREADS; ++i)
		{
			auto low = arr.begin() + i * len;
			if (i != MAX_THREADS - 1)
				::quickSort<float, N>(arr, low, low + len);
			else
				::quickSort<float, N>(arr, low, high);
		}
	}
	std::cout << "各线程排序完成" << std::endl;
	// 归并
	std::array<size_t, MAX_THREADS> medium_ptr{};
	for (size_t i = 0; i < MAX_THREADS; ++i)
		medium_ptr[i] = i * len;

	std::vector<float> result;
	while (result.size() < arr.size())
	{
		auto max_ptr = medium_ptr[0];
		for (size_t i = 0; i < MAX_THREADS; ++i)
		{
			if ((i < MAX_THREADS - 1 && medium_ptr[i] < (i + 1) * len) || (i == MAX_THREADS - 1 && medium_ptr[i] < size))
			{
				if (arr[max_ptr] < arr[medium_ptr[i]])
					max_ptr = medium_ptr[i];
			}
			else
				continue;
		}
		result.push_back(arr[max_ptr]);
	}

	std::copy(result.cbegin(), result.cend(), arr.begin());
}

