/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
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
