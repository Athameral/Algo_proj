/*****************************************************************//**
 * \file   speed_functions.hpp
 * \brief  ��plain_functions.hpp�����ϣ���float����ģ���ػ�����AVX2�����Ż�
 *
 * \author sunny
 * \date   January 2024
 *********************************************************************/
#include <array>
#include <vector>
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
 * ���ʹ��/fp:fastѡ������Զ���������������ʧ���ȡ�
 * ����ʹ��AVX2�Ը�����ͣ�����ͻ���ʧ���ȣ��ο������5%����
 * ���MSVCֻ���/fp:fastӦ��AVX2������Ϊ�ܺ���
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

/**
 * ��͵�ģ�壬���float�ػ���
 * ʹ��OpenMP���٣�ͬʱ����AVX2�����������ܺ���AVX2�ĵ��̰߳汾�������
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
	// �����߳��ڹ鲢ʱ���ж��⸺��
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
	std::cout << "���߳��������" << std::endl;
	// �鲢
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

