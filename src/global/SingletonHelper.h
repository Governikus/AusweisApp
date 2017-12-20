/*
 * \brief Helper to introduce a Singleton.
 * Be aware to use this helper in .cpp file only!
 *
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QGlobalStatic>

#define defineSingletonInstance(className, instanceName)\
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
	Q_GLOBAL_STATIC(Singleton##className, instanceName)

#define defineSingleton(className) defineSingletonInstance(className, Instance)
