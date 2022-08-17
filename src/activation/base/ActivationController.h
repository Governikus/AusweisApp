/*!
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ActivationHandler.h"

#include <QJsonObject>
#include <QVector>

namespace governikus
{

class ActivationController
{
	Q_DISABLE_COPY_MOVE(ActivationController)
	friend class AppController;

	private:
		QVector<ActivationHandler*> mInstances;

		ActivationController();
		~ActivationController();

		[[nodiscard]] static bool isPlugIn(const QJsonObject& pJson);

	public:
		[[nodiscard]] const QVector<ActivationHandler*>& getHandler() const;
		void shutdown();
};

} // namespace governikus
