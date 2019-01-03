/*!
 * \brief Controller for an external PersoSim process.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QByteArray>
#include <QObject>
#include <QProcess>
#include <QTcpSocket>

namespace governikus
{

class PersoSimController
	: public QObject
{
	Q_OBJECT

	public:
		PersoSimController();
		~PersoSimController() = default;

		bool isEnabled() const;
		bool init();
		bool write(const QByteArray& pData);
		bool shutdown();

	private:
		QProcess* mProcess;
		const QSharedPointer<QTcpSocket> mSocket;

		bool startProcess();
		void newData();


};

} // namespace governikus
