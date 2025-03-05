/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QVariant>

#include <functional>


namespace governikus
{

class ExecuteCommand
	: public QObject
{
	Q_OBJECT

	private:
		Q_INVOKABLE void execute();

		const std::function<QVariant()> mFunc;

	protected:
		~ExecuteCommand() override = default;

	public:
		explicit ExecuteCommand(const std::function<QVariant()>& pFunc);
		void run();

	Q_SIGNALS:
		void fireCommandDone(const QVariant& pResult);
};

} // namespace governikus
