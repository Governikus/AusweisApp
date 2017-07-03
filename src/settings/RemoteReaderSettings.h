/*!
 * \brief Remotereader settings
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#pragma once

#include "AbstractSettings.h"

#include <QString>

class test_RemoteReaderSettings;

namespace governikus
{

class RemoteReaderSettings
	: public AbstractSettings
{
	Q_OBJECT

	friend class AppSettings;
	friend class::test_RemoteReaderSettings;

	private:
		QString mServerName;
		bool mUseEncryption;
		QString mPassword;

		RemoteReaderSettings();

	public:
		virtual ~RemoteReaderSettings();

		virtual void load() override;
		virtual bool isUnsaved() const override;
		virtual void save() override;

		QString getServerName() const;
		void setServerName(const QString& pName);
		bool useEncryption() const;
		void setEncryption(bool pEnabled);
		QString getPassword() const;
		void setPassword(const QString& pPassword);

};

inline bool operator==(const RemoteReaderSettings& pLeft, const RemoteReaderSettings& pRight)
{
	return &pLeft == &pRight || (
		pLeft.getServerName() == pRight.getServerName() &&
		pLeft.useEncryption() == pRight.useEncryption() &&
		pLeft.getPassword() == pRight.getPassword());
}


inline bool operator!=(const RemoteReaderSettings& pLeft, const RemoteReaderSettings& pRight)
{
	return !(pLeft == pRight);
}


} /* namespace governikus */
