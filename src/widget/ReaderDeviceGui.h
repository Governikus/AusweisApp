/*!
 * \brief Qt widget based ReaderDriverUi implementation.
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QObject>

class QWidget;

namespace governikus
{

class ReaderDeviceDialog;


class ReaderDeviceGui
	: public QObject
{
	Q_OBJECT

	public:
		ReaderDeviceGui(QWidget* pParentWidget);
		virtual ~ReaderDeviceGui();

		void activate();
		void deactivate();

	Q_SIGNALS:
		void fireFinished();

	private:
		ReaderDeviceDialog* mDialog;
		void reactivate();

	private Q_SLOTS:
		void onFinished(int result);
};

} /* namespace governikus */
