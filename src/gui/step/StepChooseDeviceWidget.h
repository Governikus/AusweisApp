/*!
 * StepChooseDeviceWidget.h
 *
 * \brief Widget for the step StepChooseDeviceGui.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "context/AuthContext.h"
#include "generic/Page.h"
#include "StepGui.h"

#include <QGroupBox>

namespace Ui
{
class StepChooseDeviceWidget;
}

class QLabel;

namespace governikus
{

class StepChooseDeviceWidget
	: public Page
{
	Q_OBJECT

	public:
		StepChooseDeviceWidget(QWidget* pParent = nullptr);
		virtual ~StepChooseDeviceWidget();

		void setState(StepChooseDeviceUi::State pState);

	Q_SIGNALS:
		void setForwardButtonState(ButtonState pState, const QString& pText = QString());
		void setCancelButtonState(ButtonState pState);
		void fireScanButtonClicked();
		void fireUiFinished(const QString& pReaderName);

	private Q_SLOTS:
		void onScanButtonClicked();
		void onReaderAdded(const QString& pReaderName);
		void onReaderRemoved(const QString& pReaderName);
		void onCardInserted(const QString& pReaderName);
		void onCardRemoved(const QString& pReaderName);

	private:
		QScopedPointer<Ui::StepChooseDeviceWidget> mUi;
		QScopedPointer<QLabel> mMovieLabel;
		bool mInitialized;

		QWidget* getWidgetWithProperty(const char* pName, const QString& pValue) const;
		void setSubtitleOfReader(const QString& pReaderName, const QString& pSubTitle);
		void setIconVisible(const QString& pReaderName, bool pVisible);
		bool eventFilter(QObject* pWatched, QEvent* pEvent);
};

} /* namespace governikus */
