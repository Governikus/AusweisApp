/*!
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

#include "IosReaderDelegate.h"

#include "IosReaderDelegateImpl.h"


using namespace governikus;


namespace governikus
{

class IosReaderDelegate::Private
{
	public:
		IosReaderDelegateImpl* mIosReaderDelegateImpl;
};

} // namespace governikus


IosReaderDelegate::IosReaderDelegate()
	: mPrivate(new IosReaderDelegate::Private())
{
	IosReaderDelegateImpl* dlgt = [IosReaderDelegateImpl initWithDelegate: this];
	mPrivate->mIosReaderDelegateImpl = [dlgt retain];
}


IosReaderDelegate::~IosReaderDelegate()
{
	[mPrivate->mIosReaderDelegateImpl release];
	delete mPrivate;
}


void IosReaderDelegate::startSession()
{
	//: INFO IOS The ID card may be inserted, the authentication process may be started.
	[mPrivate->mIosReaderDelegateImpl startSession: tr("Please place your device on your ID card.")];
}


void IosReaderDelegate::stopSession(const QString& pMessage, bool pIsError)
{
	[mPrivate->mIosReaderDelegateImpl stopSession: pMessage isError: pIsError];
}


void IosReaderDelegate::onTagDiscovered(IosCard* pCard)
{
	Q_EMIT fireDiscoveredTag(pCard);
}


void IosReaderDelegate::onDidInvalidateWithError(const QString& pError, bool pDoRestart)
{
	Q_EMIT fireDidInvalidateWithError(pError, pDoRestart);
}
