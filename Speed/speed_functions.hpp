/*****************************************************************//**
 * \file   speed_functions.hpp
 * \brief  ��plain_functions.hpp�����ϣ���float����ģ���ػ�����sse�����Ż�
 *
 * \author sunny
 * \date   January 2024
 *********************************************************************/
#include <array>
#include <iostream>
#include "main.h"

// ���������СΪ����ʱ�ӿ����л�Ϊ�򵥲���
constexpr auto CUT_OFF = 8;

/**
 * \brief �����صķ�ʽ����һ����������ֵ.
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
 * \brief �����صķ�ʽ��һ���������.
 * ���ʹ��/fp:fastѡ������Զ�������
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
 * \brief һ�����������Ż��Ŀ�����������������Ԫ��С�ڵ���8��ʱ�л����򵥲�������
 * ѡ��8������ֲο���΢���qsort.cpp�ļ���������˵��������Ȥ����ȥ������
 *
 * \param arr Ҫ���������
 * \param low ������㣨������
 * \param high �����յ㣨��������
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
 * �����ֵ��������ģ�壬���float�ػ���
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
