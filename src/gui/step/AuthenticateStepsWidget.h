/*!
 * AuthenticateStepsWidget.h
 *
 * \brief A stacked widget containing the widgets for the authentication steps.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QScopedPointer>
#include <QStackedWidget>

namespace Ui
{
class AuthenticateStepsWidget;
}

class QAbstractButton;

namespace governikus
{

class BusyOverlayContainer;
class SelfInfoWidget;
class StepChooseDeviceWidget;
class StepAuthenticationEac1Widget;

class AuthenticateStepsWidget
	: public QStackedWidget
{
	Q_OBJECT

	public:
		AuthenticateStepsWidget(QWidget* pParent = nullptr);
		virtual ~AuthenticateStepsWidget();

		BusyOverlayContainer* getProcessingPage() const
		{
			return mProcessingPage;
		}


		StepChooseDeviceWidget* getChooseDevicePage() const;

		StepAuthenticationEac1Widget* getEac1Page() const;

		SelfInfoWidget* getSelfInfoPage() const;

	protected:
		void paintEvent(QPaintEvent*) override;

	private:
		QScopedPointer<Ui::AuthenticateStepsWidget> mUi;
		BusyOverlayContainer* mProcessingPage;
};

} /* namespace governikus */
