/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "PersoSimController.h"

#include <QHostAddress>
#include <QThread>

#ifdef PERSOSIM_EXECUTABLE
	#define PERSOSIM_TESTS_ENABLED
#else
	#define PERSOSIM_EXECUTABLE ""
#endif


using namespace governikus;


PersoSimController::PersoSimController()
	: QObject()
	, mProcess()
	, mSocket(new QTcpSocket)
{
}


bool PersoSimController::isEnabled() const
{
#ifdef PERSOSIM_TESTS_ENABLED
	return true;

#else
	return false;

#endif
}


bool PersoSimController::init()
{
	if (!isEnabled())
	{
		qDebug() << "PersoSim tests not enabled";
		return false;
	}

	return startProcess();
}


bool PersoSimController::write(const QByteArray& pData)
{
	qDebug() << "TO PersoSim:" << pData;
	bool success = mSocket->write(pData) == pData.size();
	if (!success)
	{
		qDebug() << "Write failed";
	}
	mSocket->flush();
	return success;
}


bool PersoSimController::shutdown()
{
	if (!mProcess.isNull())
	{
		write("shutdown\n");
		mSocket->close();
		if (!mProcess->waitForFinished(-1))
		{
			qDebug() << "Failed:" << mProcess->errorString();
			return false;
		}
	}

	return true;
}


bool PersoSimController::startProcess()
{
	if (!mProcess.isNull())
	{
		return false;
	}

	mProcess.reset(new QProcess);
	mProcess->setProcessChannelMode(QProcess::ForwardedErrorChannel);

	connect(mSocket.data(), &QTcpSocket::readyRead, this, &PersoSimController::newData);

	mProcess->start(QStringLiteral(PERSOSIM_EXECUTABLE), QStringList());
	if (!mProcess->waitForStarted(-1))
	{
		qDebug() << PERSOSIM_EXECUTABLE;
		return false;
	}

	if (mProcess->state() != QProcess::Running)
	{
		return false;
	}

	for (int i = 0; i < 10; i++)
	{
		mSocket->connectToHost(QHostAddress(QHostAddress::LocalHost), 9091);
		if (!mSocket->waitForConnected(500))
		{
			qWarning() << "Error(" << i << "):" << mSocket->errorString();
			QThread::msleep(500);
			continue;
		}

		write("select-reader basic\n");
		return true;
	}

	return false;
}


void PersoSimController::newData()
{
	qDebug() << "FROM PersoSim:" << mSocket->readAll();
}
