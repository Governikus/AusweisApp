/*!
 * \copyright Copyright (c) 2020-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ActivationHandler.h"

#include <QJsonObject>
#include <QVector>

namespace governikus
{

class ActivationController
{
	friend class AppController;

	private:
		QVector<ActivationHandler*> mInstances;

		ActivationController();
		~ActivationController();
		ActivationController(const ActivationController&) = delete;
		ActivationController& operator=(const ActivationController&) = delete;
		ActivationController(ActivationController&&) = delete;
		ActivationController& operator=(ActivationController&&) = delete;

		[[nodiscard]] bool isPlugIn(const QJsonObject& pJson) const;

	public:
		[[nodiscard]] const QVector<ActivationHandler*>& getHandler() const;
		void shutdown();
};

} // namespace governikus
