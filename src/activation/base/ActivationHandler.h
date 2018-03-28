/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"

#include <QMap>
#include <QUrl>

namespace governikus
{

class ActivationContext;

/*!
 * UI modules that can be requested to show.
 */
defineEnumType(UiModule,
		CURRENT,
		DEFAULT,
		IDENTIFY,
		SETTINGS,
		PINMANAGEMENT
		)


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

	private:
		static bool isPlugIn(const QJsonObject& pJson);

	protected:
		ActivationHandler();
		virtual ~ActivationHandler();

		/*!
		 * \brief Get the query items with lower-case keys, so we can support case-insensitive keys.
		 */
		static QMap<QString, QString> getQueryParameter(const QUrl& pUrl);

	public:
		static const QVector<ActivationHandler*>& getInstances();

		template<typename T> static T* getInstance()
		{
			for (auto handler : ActivationHandler::getInstances())
			{
				if (auto obj = qobject_cast<T*>(handler))
				{
					return obj;
				}
			}

			qCritical() << "Cannot find requested ActivationHandler";
			return nullptr;
		}


		virtual bool start() = 0;
		virtual void stop() = 0;

	Q_SIGNALS:
		void fireShowUserInformation(const QString& pErrorMessage = QString());
		void fireShowUiRequest(UiModule pModule);
		void fireAuthenticationRequest(const QSharedPointer<ActivationContext>& pActivationContext);
};

} /* namespace governikus */

Q_DECLARE_INTERFACE(governikus::ActivationHandler, "governikus.ActivationHandler")
