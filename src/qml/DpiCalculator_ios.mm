#include "DpiCalculator.h"

#include <QGuiApplication>
#include <QLoggingCategory>
#import <UIKit/UIKit.h>


Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;

#define IS_IPAD (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
#define IS_IPHONE (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
#define IS_RETINA ([[UIScreen mainScreen] scale] >= 2.0)

// screen height is orientation dependent
#define SCREEN_WIDTH ([[UIScreen mainScreen] bounds].size.width)
#define SCREEN_HEIGHT ([[UIScreen mainScreen] bounds].size.height)
#define SCREEN_MAX_LENGTH (qMax(SCREEN_WIDTH, SCREEN_HEIGHT))

#define IS_IPHONE_4 (IS_IPHONE && SCREEN_MAX_LENGTH == 480.0 && IS_RETINA)
#define IS_IPHONE_5 (IS_IPHONE && SCREEN_MAX_LENGTH == 568.0)
#define IS_IPHONE_6 (IS_IPHONE && SCREEN_MAX_LENGTH == 667.0)
#define IS_IPHONE_6PLUS (IS_IPHONE && SCREEN_MAX_LENGTH == 736.0)


qreal DpiCalculator::getDpi()
{
	qreal dpi;
	qreal renderFactor = [UIScreen mainScreen].scale;

	if (IS_IPHONE_4 || IS_IPHONE_5 || IS_IPHONE_6)
	{
		dpi = 326.0;
	}
	else if (IS_IPHONE_6PLUS)
	{
		dpi = 401.0;
		renderFactor /= 1.15;         // the iPhone 6 plus is downsampled
	}
	else if (IS_IPAD)
	{
		dpi = 264.0;
	}
	else
	{
		qCCritical(qml) << "Unknown iOS device, cannot determine dpi";
		return -1.0;
	}

	/*
	 * For some reason on iOS the unit is interpreted per point, not per pixel.
	 * Therefore we adapt it to points per inch by dividing by the render factor.
	 * See: http://www.paintcodeapp.com/news/ultimate-guide-to-iphone-resolutions
	 */
	dpi /= renderFactor;

	qCInfo(qml) << "Determined dpi" << dpi;
	return dpi;
}
