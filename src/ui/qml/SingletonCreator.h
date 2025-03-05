/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"

#include <QQmlEngine>


namespace governikus
{

// Use this in SingletonCreator::create only!
// Avoid another template here to reduce generation
// of a lot of templated functions in the binary.
QObject* registerSingletonModel(QObject* pModel);

template<typename T>
class SingletonCreator
{
	public:
		static T* create(const QQmlEngine*, const QJSEngine*)
		{
			return static_cast<T*>(registerSingletonModel(Env::getSingleton<T>()));
		}


};

} // namespace governikus
