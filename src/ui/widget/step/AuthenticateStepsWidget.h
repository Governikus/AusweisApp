/*!
 * \brief A stacked widget containing the widgets for the authentication steps.
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QScopedPointer>
#include <QStackedWidget>

namespace Ui
{
class AuthenticateStepsWidget;
} // namespace Ui


namespace governikus
{

class BusyOverlayContainer;
class SelfInfoWidget;
class StepAuthenticationEac1Widget;

class AuthenticateStepsWidget
	: public QStackedWidget
{
	Q_OBJECT

	public:
		AuthenticateStepsWidget(QWidget* pParent = nullptr);
		virtual ~AuthenticateStepsWidget() override;

		BusyOverlayContainer* getProcessingPage() const
		{
			return mProcessingPage;
		}


		StepAuthenticationEac1Widget* getEac1Page() const;

		SelfInfoWidget* getSelfInfoPage() const;

	protected:
		void paintEvent(QPaintEvent*) override;
		void changeEvent(QEvent* pEvent) override;

	private:
		QScopedPointer<Ui::AuthenticateStepsWidget> mUi;
		BusyOverlayContainer* mProcessingPage;
};

} // namespace governikus
