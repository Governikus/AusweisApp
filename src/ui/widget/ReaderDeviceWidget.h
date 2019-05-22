/*!
 * \brief Widget for detecting attached card readers and
 * suggesting an appropriate driver to be installed.
 *
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AppSettings.h"
#include "ReaderDriverModel.h"
#include "RemoteDeviceModel.h"

#include <QDialog>
#include <QTableView>
#include <QTimer>
#include <QWidget>

namespace Ui
{
class ReaderDeviceWidget;
} // namespace Ui

namespace governikus
{

class ReaderDeviceWidget
	: public QWidget
{
	Q_OBJECT

	private:
		QScopedPointer<Ui::ReaderDeviceWidget> mUi;
		ReaderDriverModel mLocalReaderDataModel;
		RemoteDeviceModel mRemoteReaderDataModel;

		void setDisplayText();

		void updateInfoIcon();
		void updateInfoText();
		void updateInfoUpdate();

		static QString askForPin(QWidget* pParent);

	private Q_SLOTS:
		void showEvent(QShowEvent* pEevent) override;
		void hideEvent(QHideEvent* pEvent) override;
		void onUpdateInfo();
		void onAdjustReaderNameColumnWidth();
		void onUpdateLocalTableSelection();
		void onUpdateRemoteTableSelection();
		void onConnectClicked();
		void onEstablishConnectionDone(const QSharedPointer<RemoteDeviceListEntry>& pEntry, const GlobalStatus& pStatus);
		void onRemoteSelectionChanged();
		void onForgetClicked();
		void onRemoteDoubleClicked(const QModelIndex& pIndex);

	protected:
		virtual void changeEvent(QEvent* pEvent) override;

	public:
		explicit ReaderDeviceWidget(QWidget* pParent = nullptr);
		virtual ~ReaderDeviceWidget() override;

		void prependAccessibleName(const QString& pAccessibleNameAddition);

	Q_SIGNALS:
		void fireWidgetShown();
		void fireWidgetHidden();
};

} // namespace governikus
