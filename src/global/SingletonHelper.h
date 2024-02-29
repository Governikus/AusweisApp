/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*
 * \brief Helper to introduce a Singleton.
 * Be aware to use this helper in .cpp file only!
 */

#pragma once

#include <QGlobalStatic>
#include <QObject>

#define defineSingleton(className)\
	namespace\
	{\
	class Singleton##className final\
		: public className\
	{\
		public:\
			using className::className;\
	};\
	}\
\
	Q_GLOBAL_STATIC(Singleton##className, Instance)\
\
	className & className::getInstance()\
	{\
		return *Instance;\
	}\
\
	static_assert(!std::is_base_of_v<QObject, className>, "QObject cannot be Q_GLOBAL_STATIC");
