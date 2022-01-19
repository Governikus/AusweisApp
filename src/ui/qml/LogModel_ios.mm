/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "LogModel.h"

#include "ApplicationModel.h"
#include "LogHandler.h"

#import <MessageUI/MessageUI.h>
#import <UIKit/UIKit.h>

#include <QFile>
#include <QLoggingCategory>

#include <optional>


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


static std::optional<QString> getTemporaryLogFile(std::optional<QString> pSourceFile = std::nullopt)
{
	LogHandler* logHandler = Env::getSingleton<LogHandler>();

	QString destinationFileName;
	if (pSourceFile)
	{
		destinationFileName = LogModel::createLogFileName(logHandler->getFileDate(QFileInfo(pSourceFile.value())));
	}
	else
	{
		destinationFileName = LogModel::createLogFileName();
	}

	QString destinationFilePath = QString::fromNSString([NSTemporaryDirectory() stringByAppendingPathComponent: destinationFileName.toNSString()]);

	if (QFile::exists(destinationFilePath))
	{
		QFile::remove(destinationFilePath);
	}

	if (pSourceFile)
	{
		if (!QFile::copy(pSourceFile.value(), destinationFilePath))
		{
			qCCritical(qml) << "Cannot copy logfile to" << destinationFilePath;
			return std::nullopt;
		}
	}
	else
	{
		logHandler->copy(destinationFilePath);
	}

	return destinationFilePath;
}


void LogModel::mailLog(const QString& pEmail, const QString& pSubject, const QString& pMsg)
{
	if (![MFMailComposeViewController canSendMail])
	{
		qCWarning(qml) << "Email not configured, cannot send mail.";
		Env::getSingleton<ApplicationModel>()->showFeedback(tr("Could not send log! Please configure your mail client first."));
		return;
	}

	QString fileName = LogModel::createLogFileName();
	std::optional<QString> logFile = getTemporaryLogFile();
	if (!logFile)
	{
		return;
	}

	NSData* fileContent = [NSData dataWithContentsOfFile: logFile.value().toNSString()];

	UIViewController* rootController = [UIApplication sharedApplication].windows[0].rootViewController;

	auto* mailComposeController = [[MailComposeController alloc] init];
	mailComposeController.mailComposeDelegate = mailComposeController;

	[mailComposeController setToRecipients:@[pEmail.toNSString()]];
	[mailComposeController setSubject: pSubject.toNSString()];
	[mailComposeController setMessageBody: pMsg.toNSString() isHTML:NO];
	[mailComposeController addAttachmentData: fileContent mimeType: @"text/plain" fileName: fileName.toNSString()];

	[rootController presentViewController:mailComposeController animated:YES completion:nil];
}


void LogModel::shareLog(const QPoint popupPosition)
{
	std::optional<QString> logFile;
	if (mSelectedLogFile == 0)
	{
		logFile = getTemporaryLogFile();
	}
	else
	{
		logFile = getTemporaryLogFile(mLogFiles.at(mSelectedLogFile));
	}
	if (!logFile)
	{
		return;
	}

	NSURL* logFileURL = [NSURL fileURLWithPath: logFile.value().toNSString()];

	NSArray* shareItems = @[logFileURL];

	UIActivityViewController* shareController = [[UIActivityViewController alloc]initWithActivityItems: shareItems applicationActivities:nil];

	UIViewController* rootController = [UIApplication sharedApplication].windows[0].rootViewController;

	shareController.popoverPresentationController.sourceView = rootController.view;
	shareController.popoverPresentationController.sourceRect = CGRectMake(popupPosition.x(), popupPosition.y(), 0, 0);

	[rootController presentViewController:shareController animated:YES completion: nil];
}
