/*!
 * PskManager.h
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once


#include <QByteArray>
#include <QMutex>


namespace governikus
{

class PskManager
{

	private:
		QByteArray mPsk;
		bool mSecureRandomPsk;
		QMutex mPskMutex;

	public:
		static PskManager& getInstance();
		QByteArray generatePsk(const QByteArray& pClientPartialPsk = QByteArray());
		QByteArray getPsk();
		bool isSecureRandomPsk();
};

}
