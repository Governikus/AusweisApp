/*!
 * ReaderDriverWidget.h
 *
 * \brief Widget for detecting attached card readers and
 * suggesting an appropriate driver to be installed.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include <QDialog>
#include <QTimer>

#include "context/ReaderDriverContext.h"
#include "generic/Page.h"

namespace Ui
{
class ReaderDriverWidget;
}

class QTableWidget;

namespace governikus
{
class ReaderDriverWidget
	: public Page
{
	Q_OBJECT

	public:
		explicit ReaderDriverWidget(QWidget* pParent = nullptr);

		~ReaderDriverWidget();

	public Q_SLOTS:
		void onUpdateContent();

	private:
		Ui::ReaderDriverWidget* ui;

		ReaderDriverContext mContext;

		QVector<QSharedPointer<DeviceDescriptor> > mAttachedDevices;

		int mSelectedRow;

		QTimer mTimer;

		void removeOldTableItems();

		void updateTableItems();

		void updateInfo();

		void updateInfoIcon();

		void updateInfoText();

		void updateInfoUpdate();

		// Get the names of readers from the settings and find the width
		// in pixels needed to accomodate the longest name.
		int getLongestReaderNameWidth() const;

	private Q_SLOTS:
		void onDeviceSelectionChanged();
		void onTimerEvent();
};

}
