/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Template functions that allow to map and filter over QVectors.
 */

#pragma once

#include <functional>
#include <type_traits>

#include <QVector>

namespace governikus
{

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))

/*
 * Usage example: map<Reader, QString>([](const Reader& r){ return r.getName(); }, readers)
 *
 * where readers has type QVector<Reader>
 */
template<typename S, typename T>
std::enable_if_t<!std::is_void_v<T>, QVector<T>> map(const std::function<T(const S&)>& pFunc, const QVector<S>& pItems)
{
	const auto sz = pItems.size();
	QVector<T> result(sz);
	for (int index = 0; index < sz; ++index)
	{
		result[index] = pFunc(pItems[index]);
	}

	return result;
}


#endif


/*
 * Usage example: map<Reader, QString>([](const Reader& r){ return r.getName(); }, readers)
 *
 * where readers has type QList<Reader>
 */
template<typename S, typename T>
std::enable_if_t<!std::is_void_v<T>, QList<T>> map(const std::function<T(const S&)>& pFunc, const QList<S>& pItems)
{
	const auto sz = pItems.size();
	QList<T> result;
	for (int index = 0; index < sz; ++index)
	{
		result.append(pFunc(pItems[index]));
	}

	return result;
}


/*
 * Usage example: filter<Reader>([](const Reader& r){ return r.getCard() != nullptr; }, readers)
 *
 * where readers has type QVector<Reader>
 */
template<typename T>
std::enable_if_t<!std::is_void_v<T>, QVector<T>> filter(const std::function<bool(const T&)>& pFunc, const QVector<T>& pItems)
{
	QVector<T> result;
	for (const T& item : pItems)
	{
		if (pFunc(item))
		{
			result += item;
		}
	}

	return result;
}


} // namespace governikus
