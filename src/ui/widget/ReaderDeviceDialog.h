/*!
 * \brief Dialog for detecting attached card readers or available
 * remote card readers.
 *
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ReaderDeviceWidget.h"

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
class ReaderDeviceDialog;
} // namespace Ui


namespace governikus
{

class ReaderDeviceDialog
	: public QDialog
{
	Q_OBJECT

	public:
		ReaderDeviceDialog(QWidget* pParent = nullptr);
		virtual ~ReaderDeviceDialog() override;

	protected:
		virtual bool eventFilter(QObject* pObject, QEvent* pEvent) override;
		virtual void changeEvent(QEvent* pEvent) override;

	private:
		QScopedPointer<Ui::ReaderDeviceDialog> mUi;
		ReaderDeviceWidget* mReaderDeviceWidget;
};

} // namespace governikus
