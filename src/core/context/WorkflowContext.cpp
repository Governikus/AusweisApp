/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "WorkflowContext.h"

using namespace governikus;

WorkflowContext::WorkflowContext()
	: QObject()
	, mStateApproved(false)
	, mCurrentState()
	, mReaderType(ReaderManagerPlugInType::UNKNOWN)
	, mReaderName()
	, mCardConnection()
	, mCan()
	, mPin()
	, mPaceOutputData()
	, mOldRetryCounter(-1)
	, mLastPaceResult(ReturnCode::OK)
	, mResult(Result::createOk())
	, mErrorReportedToUser(true)
	, mWorkflowFinished(false)
{
}
