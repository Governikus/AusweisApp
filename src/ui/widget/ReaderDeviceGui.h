/*!
 * \brief Qt widget based ReaderDriverUi implementation.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ReaderDeviceDialog.h"

#include <QWidget>

namespace governikus
{

class ReaderDeviceGui
	: public QObject
{
	Q_OBJECT

	public:
		ReaderDeviceGui(QWidget* pParentWidget);
		virtual ~ReaderDeviceGui();

		void activate();
		void deactivate();
		void reactToReaderCount(int pReaderCount);

	Q_SIGNALS:
		void fireFinished();

	private:
		ReaderDeviceDialog* mDialog;
		void reactivate();

	private Q_SLOTS:
		void onFinished(int result);
};

} // namespace governikus
