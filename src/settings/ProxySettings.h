/*!
 * ProxySettings.h
 *
 * \brief Contains the definition of the ProxySettings class.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "AbstractSettings.h"

#include <QByteArray>
#include <QNetworkProxy>
#include <QString>


class test_ProxySettings;


namespace governikus
{

/*!
 * \brief Represents proxy server settings.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */
class ProxySettings
	: public AbstractSettings
{
	Q_OBJECT

	friend class AppSettings;
	friend class::test_ProxySettings;

	private:
		QNetworkProxy::ProxyType mProxyType;
		ProxySettings();

	public:
		virtual ~ProxySettings();

		QNetworkProxy::ProxyType getProxyType() const;
		void setProxyType(QNetworkProxy::ProxyType pProxyType);

		virtual void load() override;
		virtual bool isUnsaved() const override;
		virtual void save() override;
};

inline bool operator==(const ProxySettings& pLeft, const ProxySettings& pRight)
{
	return &pLeft == &pRight || pLeft.getProxyType() == pRight.getProxyType();
}


inline bool operator!=(const ProxySettings& pLeft, const ProxySettings& pRight)
{
	return !(pLeft == pRight);
}


} /* namespace governikus */
