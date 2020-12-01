/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"
#include "UIPlugIn.h"

#include <QMap>
#include <QUrl>

namespace governikus
{

class ActivationContext;

/*!
 * Format types for status responses
 */
defineEnumType(StatusFormat,
		PLAIN,
		JSON
		)


/*!
 * The ActivationHandler is beside the UI another API of the application.
 * The ActivationHandler sends authentication requests to the application and also
 * request to show the UI.
 * Some application requests may be handled internally by the ActivationHandler and
 * are not forwarded to the application.
 */
class ActivationHandler
	: public QObject
{
	Q_OBJECT

	protected:
		ActivationHandler() = default;
		virtual ~ActivationHandler() = default;

	public:
		virtual bool start() = 0;
		virtual void stop() = 0;

	public Q_SLOTS:
		virtual void onApplicationActivated();

	Q_SIGNALS:
		void fireShowUserInformation(const QString& pErrorMessage = QString());
		void fireShowUiRequest(UiModule pModule);
		void fireAuthenticationRequest(const QSharedPointer<ActivationContext>& pActivationContext);
};

} // namespace governikus

Q_DECLARE_INTERFACE(governikus::ActivationHandler, "governikus.ActivationHandler")
