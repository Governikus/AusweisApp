/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "ChangePinModel.h"

#include "ReaderManager.h"
#include "context/ChangePinContext.h"

#include <QDebug>

using namespace governikus;


ChangePinModel::ChangePinModel(QObject* pParent)
	: QObject(pParent)
{
}


ChangePinModel::~ChangePinModel()
{
}


void ChangePinModel::resetContext(const QSharedPointer<ChangePinContext>& pContext)
{
	mContext = pContext;
	if (mContext)
	{
		connect(mContext.data(), &ChangePinContext::fireCurrentStateChanged, this, &ChangePinModel::fireCurrentStateChanged);
		connect(mContext.data(), &ChangePinContext::fireResultChanged, this, &ChangePinModel::fireResultChanged);
		connect(mContext.data(), &ChangePinContext::fireSuccessMessageChanged, this, &ChangePinModel::fireResultChanged);

		Q_EMIT fireResultChanged();
	}

	/*
	 * Only this state change is emitted when the context is reset, i.e. after the end of the workflow
	 */
	Q_EMIT fireCurrentStateChanged(getCurrentState());
}


QString ChangePinModel::getCurrentState() const
{
	return mContext ? mContext->getCurrentState() : QString();
}


QString ChangePinModel::getResultString() const
{
	if (!mContext)
	{
		return QString();
	}

	return mContext->getStatus().isNoError() ? mContext->getSuccessMessage() : mContext->getStatus().toErrorDescription(true);
}


bool ChangePinModel::isResultOk() const
{
	return mContext && mContext->getStatus().isNoError();
}


void ChangePinModel::startWorkflow()
{
	Q_EMIT fireStartWorkflow();
}


void ChangePinModel::cancelWorkflow()
{
	if (mContext)
	{
		Q_EMIT mContext->fireCancelWorkflow();
	}

}


void ChangePinModel::setReaderType(const QString& pReaderType)
{
	if (mContext)
	{
		mContext->setReaderType(Enum<ReaderManagerPlugInType>::fromString(pReaderType, ReaderManagerPlugInType::UNKNOWN));
	}
}


bool ChangePinModel::isBasicReader()
{
	return mContext->getCardConnection()->getReaderInfo().isBasicReader();
}


void ChangePinModel::abortCardSelection()
{
	if (mContext)
	{
		Q_EMIT mContext->fireAbortCardSelection();
	}
}
