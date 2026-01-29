/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#include "LogModel.h"

#include "ApplicationModel.h"
#include "PlatformTools.h"

#import <MessageUI/MessageUI.h>
#import <UIKit/UIKit.h>

#include <QFile>
#include <QLoggingCategory>
#include <QUrl>


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


static QString getTemporaryLogFilePath(const QString& pTargetFileName)
{
	return QString::fromNSString([NSTemporaryDirectory() stringByAppendingPathComponent: pTargetFileName.toNSString()]);
}


void LogModel::mailLogFile(const QString& pEmail, const QString& pSubject, const QString& pMsg) const
{
	if (![MFMailComposeViewController canSendMail])
	{
		Env::getSingleton<ApplicationModel>()->showFeedback(tr("Could not mail log! Please configure your mail client first."));
		return;
	}

	const auto fileName = createLogFileName();
	const auto& logFile = getTemporaryLogFilePath(fileName);
	if (!saveLogFile(QUrl::fromLocalFile(logFile), false))
	{
		qCCritical(qml) << "Can't copy log file to" << logFile;
		return;
	}

	NSData* fileContent = [NSData dataWithContentsOfFile: logFile.toNSString()];

	UIWindow* window = PlatformTools::getFirstWindow();
	if (!window)
	{
		Env::getSingleton<ApplicationModel>()->showFeedback(tr("Could not mail log! Connection to OS failed."));
		return;
	}
	UIViewController* rootController = window.rootViewController;

	auto* mailComposeController = [[MailComposeController alloc] init];
	mailComposeController.mailComposeDelegate = mailComposeController;

	[mailComposeController setToRecipients:@[pEmail.toNSString()]];
	[mailComposeController setSubject: pSubject.toNSString()];
	[mailComposeController setMessageBody: pMsg.toNSString() isHTML:NO];
	[mailComposeController addAttachmentData: fileContent mimeType: @"text/plain" fileName: fileName.toNSString()];

	[rootController presentViewController:mailComposeController animated:YES completion:nil];
}


void LogModel::shareLogFile(const QPoint popupPosition) const
{
	const auto& logFile = getTemporaryLogFilePath(createLogFileName());
	if (!saveLogFile(QUrl::fromLocalFile(logFile), false))
	{
		qCCritical(qml) << "Can't copy log file to" << logFile;
		return;
	}

	NSURL* logFileURL = [NSURL fileURLWithPath: logFile.toNSString()];

	NSArray* shareItems = @[logFileURL];

	UIActivityViewController* shareController = [[UIActivityViewController alloc]initWithActivityItems: shareItems applicationActivities:nil];

	UIWindow* window = PlatformTools::getFirstWindow();
	if (!window)
	{
		Env::getSingleton<ApplicationModel>()->showFeedback(tr("Could not share log! Connection to OS failed."));
		return;
	}
	UIViewController* rootController = window.rootViewController;

	shareController.popoverPresentationController.sourceView = rootController.view;
	shareController.popoverPresentationController.sourceRect = CGRectMake(popupPosition.x(), popupPosition.y(), 0, 0);

	[rootController presentViewController:shareController animated:YES completion: nil];
}
