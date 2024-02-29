/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#include "LogModel.h"

#include "ApplicationModel.h"
#include "LogHandler.h"

#import <MessageUI/MessageUI.h>
#import <UIKit/UIKit.h>

#include <QFile>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(qml)


using namespace governikus;

@interface MailComposeController
	: MFMailComposeViewController<MFMailComposeViewControllerDelegate>
- (void)mailComposeController:(MFMailComposeViewController*)controller didFinishWithResult:(MFMailComposeResult)result error:(NSError*)error;
@end

@implementation MailComposeController

- (void)mailComposeController:(MFMailComposeViewController*)controller didFinishWithResult:(MFMailComposeResult)result error:(NSError*)error {

	Q_UNUSED(result)
	Q_UNUSED(error)

	[controller dismissViewControllerAnimated: true completion:nil];
}


@end


static QString getTemporaryLogFile(const QString& pSourceFile = QString())
{
	LogHandler* logHandler = Env::getSingleton<LogHandler>();

	QString destinationFileName;
	if (pSourceFile.isEmpty())
	{
		destinationFileName = LogModel::createLogFileName();
	}
	else
	{
		destinationFileName = LogModel::createLogFileName(logHandler->getFileDate(QFileInfo(pSourceFile)));
	}

	QString destinationFilePath = QString::fromNSString([NSTemporaryDirectory() stringByAppendingPathComponent: destinationFileName.toNSString()]);

	if (QFile::exists(destinationFilePath))
	{
		QFile::remove(destinationFilePath);
	}

	if (pSourceFile.isEmpty())
	{
		logHandler->copy(destinationFilePath);
	}
	else
	{
		if (!QFile::copy(pSourceFile, destinationFilePath))
		{
			qCCritical(qml) << "Cannot copy logfile to" << destinationFilePath;
			return QString();
		}
	}

	return destinationFilePath;
}


void LogModel::mailLog(const QString& pEmail, const QString& pSubject, const QString& pMsg) const
{
	if (![MFMailComposeViewController canSendMail])
	{
		qCWarning(qml) << "Email not configured, cannot send mail.";
		Env::getSingleton<ApplicationModel>()->showFeedback(tr("Could not send log! Please configure your mail client first."));
		return;
	}

	QString fileName = LogModel::createLogFileName();
	const auto& logFile = getTemporaryLogFile();
	if (logFile.isEmpty())
	{
		return;
	}

	NSData* fileContent = [NSData dataWithContentsOfFile: logFile.toNSString()];

	UIViewController* rootController = [UIApplication sharedApplication].windows[0].rootViewController;

	auto* mailComposeController = [[MailComposeController alloc] init];
	mailComposeController.mailComposeDelegate = mailComposeController;

	[mailComposeController setToRecipients:@[pEmail.toNSString()]];
	[mailComposeController setSubject: pSubject.toNSString()];
	[mailComposeController setMessageBody: pMsg.toNSString() isHTML:NO];
	[mailComposeController addAttachmentData: fileContent mimeType: @"text/plain" fileName: fileName.toNSString()];

	[rootController presentViewController:mailComposeController animated:YES completion:nil];
}


void LogModel::shareLog(const QPoint popupPosition) const
{
	const QString& logFile = mSelectedLogFile == 0 ? getTemporaryLogFile() : getTemporaryLogFile(mLogFiles.at(mSelectedLogFile));
	if (logFile.isEmpty())
	{
		return;
	}

	NSURL* logFileURL = [NSURL fileURLWithPath: logFile.toNSString()];

	NSArray* shareItems = @[logFileURL];

	UIActivityViewController* shareController = [[UIActivityViewController alloc]initWithActivityItems: shareItems applicationActivities:nil];

	UIViewController* rootController = [UIApplication sharedApplication].windows[0].rootViewController;

	shareController.popoverPresentationController.sourceView = rootController.view;
	shareController.popoverPresentationController.sourceRect = CGRectMake(popupPosition.x(), popupPosition.y(), 0, 0);

	[rootController presentViewController:shareController animated:YES completion: nil];
}
