/*!
 * InputAPDUInfo.h
 *
 * \brief Holds the data of an InputAPDUInfo element.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include <QByteArrayList>
#include <QList>

#pragma once

namespace governikus
{

class InputAPDUInfo
{
	public:
		InputAPDUInfo();
		InputAPDUInfo(const QByteArray& pInputApdu, const QByteArrayList& pAcceptableStatusCodes);

		bool isValid() const;

		const QByteArray& getInputApdu() const
		{
			return mInputApdu;
		}


		void setInputApdu(const QByteArray& pInputApdu)
		{
			mInputApdu = pInputApdu;
		}


		const QByteArrayList& getAcceptableStatusCodes() const
		{
			return mAcceptableStatusCodes;
		}


		void addAcceptableStatusCode(const QByteArray& pStatusCode)
		{
			mAcceptableStatusCodes.append(pStatusCode);
		}


	private:
		QByteArray mInputApdu;
		QByteArrayList mAcceptableStatusCodes;
};

}
