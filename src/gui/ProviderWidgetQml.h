/*!
 * \brief TODO
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "AppSettings.h"
#include "generic/Page.h"
#include "HistoryModel.h"
#include "ProviderCategoryFilterModel.h"
#include "QmlExtension.h"

#include <QQuickWidget>
#include <QScopedPointer>
#include <QUrl>

namespace Ui
{
class ProviderWidgetQml;
}

namespace governikus
{

class ProviderDetailWidget;

class ProviderWidgetQml
	: public Page
{
	Q_OBJECT

	private:
		QScopedPointer<Ui::ProviderWidgetQml> mUi;

		qreal mDpi;
		HistoryModel mHistoryModel;
		ProviderCategoryFilterModel mProviderModel;
		QmlExtension mQmlExtension;

	private Q_SLOTS:
		void onQQuickWidgetStatusChanged(QQuickWidget::Status pStatus);

	public:
		ProviderWidgetQml(QWidget* pParent = nullptr);
		virtual ~ProviderWidgetQml();
};

} /* namespace governikus */
