/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <functional>
#include <type_traits>

#include <QList>


namespace governikus
{


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
 * where readers has type QList<Reader>
 */
template<typename T>
std::enable_if_t<!std::is_void_v<T>, QList<T>> filter(const std::function<bool(const T&)>& pFunc, const QList<T>& pItems)
{
	QList<T> result;
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
