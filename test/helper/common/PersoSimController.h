/*!
 * \brief Controller for an external PersoSim process.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QByteArray>
#include <QObject>
#include <QProcess>
#include <QScopedPointer>
#include <QTcpSocket>

namespace governikus
{

class PersoSimController
	: public QObject
{
	Q_OBJECT

	public:
		PersoSimController();
		~PersoSimController() override = default;

		[[nodiscard]] bool isEnabled() const;
		bool init();
		bool write(const QByteArray& pData);
		bool shutdown();

	private:
		QScopedPointer<QProcess> mProcess;
		const QSharedPointer<QTcpSocket> mSocket;

		bool startProcess();
		void newData();


};

} // namespace governikus
