/*
 * \brief Initializer to register a lambda that will be executed if QCoreApplication is ready.
 *
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QCoreApplication>

#define INIT_FUNCTION(pRegister)\
	namespace {\
	static void init_function(){\
		qAddPreRoutine(pRegister);\
	}\
	Q_CONSTRUCTOR_FUNCTION(init_function)\
	}
