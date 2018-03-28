/*!
 * \brief Gui utility functions.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QWidget>

#include "CardReturnCode.h"

class QFrame;


namespace governikus
{

class GuiUtils
	: private QObject
{
	Q_OBJECT

	public:
		static void showPinCanPukErrorDialog(CardReturnCode pReturnCode, int pRetryCounter, QWidget* pParent);
		static bool showWrongPinBlockedDialog(QWidget* pParent);
};

} /* namespace governikus */
