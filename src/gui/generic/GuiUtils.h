/*!
 * GuiUtils.h
 *
 * \brief Gui utility functions.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QWidget>

#include "ReturnCode.h"

class QFrame;


namespace governikus
{

class GuiUtils
	: private QObject
{
	Q_OBJECT

	public:
		static void setFontBold(QWidget* pWidget, bool pBold);

		static QFrame* createLine(bool pHorizontal);

		static void showPinCanPukErrorDialog(ReturnCode pReturnCode, int pRetryCounter, QWidget* pParent);
		static bool showWrongPinBlockedDialog(QWidget* pParent);
		static void updateFontSize(QWidget* pWidget);
};

} /* namespace governikus */
