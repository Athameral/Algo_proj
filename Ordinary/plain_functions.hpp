/*****************************************************************//**
 * \file   plain_functions.hpp
 * \brief  �����˼��㺯�����������ֵ����ͣ�����ȡ�
 *
 * \author sunny
 * \date   January 2024
 *********************************************************************/
#include <array>
#include <iostream>

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
