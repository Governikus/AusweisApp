/*!
 * \brief A history widget which embeds QML
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "AppSettings.h"
#include "DetailDialog.h"
#include "HistoryModel.h"
#include "QmlExtension.h"
#include "generic/Page.h"

#include <QQuickWidget>
#include <QScopedPointer>
#include <QUrl>

namespace Ui
{
class HistoryWidgetQml;
}

namespace governikus
{

class ProviderDetailWidget;

class HistoryWidgetQml
	: public Page
{
	Q_OBJECT

	private:
		QScopedPointer<Ui::HistoryWidgetQml> mUi;

		qreal mDpi;
		HistoryModel mHistoryModel;
		QmlExtension mQmlExtension;

	private Q_SLOTS:
		void onQQuickWidgetStatusChanged(QQuickWidget::Status pStatus);

	public:
		HistoryWidgetQml(QWidget* pParent = nullptr);
		virtual ~HistoryWidgetQml();

		Q_INVOKABLE void showDetailDialog(const QString& pDetails);
};

} /* namespace governikus */
