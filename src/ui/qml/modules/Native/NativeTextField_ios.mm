/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#include "NativeTextField.h"

#include <QCoreApplication>

#import <CoreGraphics/CoreGraphics.h>
#import <UIKit/UIKit.h>


@interface GAccessibleTextField
	: UITextField
@property NativeTextField* qtBridgeItem;
@end

@implementation GAccessibleTextField

@synthesize qtBridgeItem;


- (id) initWithBridge: (NativeTextField*) pQtBridgeItem
{
	self = [super initWithFrame:CGRectMake(0, 0, 1, 1)];
	if (self)
	{
		qtBridgeItem = pQtBridgeItem;
	}
	return self;
}


- (void)accessibilityElementDidBecomeFocused {
	[super accessibilityElementDidBecomeFocused];
	qtBridgeItem->sinkFocus();
	UIAccessibilityPostNotification(UIAccessibilityLayoutChangedNotification, self);
}


@end


@interface TextInputDelegate
	: NSObject<UITextFieldDelegate>
{
	NativeTextField* m_inputWrapper;
}

- (id) initWithTextInput: (NativeTextField*) textInput;
- (void) onTextChange;

- (BOOL) textField: (UITextField*) textField
		shouldChangeCharactersInRange:(NSRange) range
		replacementString:(NSString*) string;

- (void) textFieldDidBeginEditing: (UITextField*) textField;
- (void) textFieldDidEndEditing: (UITextField*) textField;
- (BOOL) textFieldShouldBeginEditing: (UITextField*) textField;
- (BOOL) textFieldShouldClear: (UITextField*) textField;
- (BOOL) textFieldShouldEndEditing: (UITextField*) textField;
- (BOOL) textFieldShouldReturn: (UITextField*) textField;

@end

@implementation TextInputDelegate

- (id) initWithTextInput:(NativeTextField*) textInput
{
	self = [super init];
	if (self)
	{
		m_inputWrapper = textInput;
	}
	return self;
}


- (void) onTextChange
{
	m_inputWrapper->textChanged();
	m_inputWrapper->textEdited();
}


- (BOOL) textField: (UITextField*) textField
		shouldChangeCharactersInRange:(NSRange) range
		replacementString:(NSString*) string
{
	if (range.length + range.location > textField.text.length)
	{
		return NO;
	}

	int newLength = static_cast<int>(textField.text.length + string.length - range.length);
	int maximumLength = m_inputWrapper->maximumLength();

	if (maximumLength != -1 && newLength > maximumLength)
	{
		UIAccessibilityPostNotification(UIAccessibilityAnnouncementNotification, QCoreApplication::translate("GTextField", "Maximum allowed length reached.").toNSString());
		return NO;
	}

	return YES;
}


- (void) textFieldDidBeginEditing: (UITextField*) textField
{
	Q_UNUSED(textField)
}


- (void) textFieldDidEndEditing: (UITextField*) textField
{
	Q_UNUSED(textField)
}


- (BOOL) textFieldShouldBeginEditing: (UITextField*) textField
{
	Q_UNUSED(textField)
	return YES;
}


- (BOOL) textFieldShouldClear: (UITextField*) textField
{
	Q_UNUSED(textField)
	return YES;
}


- (BOOL) textFieldShouldEndEditing: (UITextField*) textField
{
	Q_UNUSED(textField)
	return YES;
}


- (BOOL) textFieldShouldReturn: (UITextField*) textField
{
	qApp->inputMethod()->hide();
	[textField resignFirstResponder];
	m_inputWrapper->accepted();
	return YES;
}


@end


class NativeTextFieldPrivate
{
	friend class NativeTextField;

	private:
		NativeTextField* const q;
		TextInputDelegate* delegate;
		GAccessibleTextField* textInput;
		QScopedPointer<QWindow> window;
		QScopedPointer<QQuickItem> focusSink;
		QList<QMetaObject::Connection> parentConnections;
		int maximumLength;
		QFont font;

		void setWindow(QWindow* pWindow)
		{
			window.reset(pWindow);

			if (pWindow)
			{
				QObject::connect(pWindow, &QWindow::visibleChanged, q, [this] {
							updateNativeFocus();
						});
			}

			q->nativeWindowChanged();
		}

	public:
		explicit NativeTextFieldPrivate(NativeTextField* pNativeTextField)
			: q(pNativeTextField)
			, delegate(nullptr)
			, textInput(nullptr)
			, window(nullptr)
			, focusSink(new QQuickItem())
			, parentConnections(QList<QMetaObject::Connection>())
			, maximumLength(-1)
			, font(QGuiApplication::font())
		{
			delegate = [[TextInputDelegate alloc] initWithTextInput: pNativeTextField];

			textInput = [[GAccessibleTextField alloc] initWithBridge: q];
			[textInput setReturnKeyType:UIReturnKeyDone];
			[textInput setBorderStyle:UITextBorderStyleRoundedRect];
			[textInput addTarget: delegate action: @selector(onTextChange) forControlEvents: UIControlEventEditingChanged];
			[textInput setDelegate:delegate];

			setWindow(QWindow::fromWinId(WId(textInput)));
			focusSink->setParentItem(pNativeTextField);
		}


		~NativeTextFieldPrivate()
		{
			clearParentConnections();

			[textInput setDelegate:nil];
			[textInput removeTarget:delegate action:@selector(onTextChange) forControlEvents:UIControlEventEditingChanged];

			setWindow(nullptr);
		}


		void clearParentConnections()
		{
			for (auto& connection : std::exchange(parentConnections, {}))
			{
				QObject::disconnect(connection);
			}
		}


		void updateNativeFocus()
		{
			if (window && window->isVisible() && q->hasActiveFocus())
			{
				focusSink->forceActiveFocus();
				[textInput becomeFirstResponder];
			}
		}


		void updateImplicitSize()
		{
			const CGSize implicitSize = textInput.intrinsicContentSize;
			q->setImplicitWidth(implicitSize.width);
			q->setImplicitHeight(implicitSize.height);
		}


};


static inline QColor fromUIColor(UIColor* color)
{
	CGFloat r, g, b, a;
	[color getRed:&r green:&g blue:&b alpha:&a];
	return QColor::fromRgbF(static_cast<float>(r),
			static_cast<float>(g),
			static_cast<float>(b),
			static_cast<float>(a));
}


NativeTextField::NativeTextField(QQuickItem* pParent)
	: QQuickItem(pParent)
	, d(new NativeTextFieldPrivate(this))
{
	setFlag(ItemHasContents, true);
	connect(this, &QQuickItem::activeFocusChanged, this, [this] {
				d->updateNativeFocus();
			});

	onParentChanged();
	d->updateImplicitSize();
}


NativeTextField::~NativeTextField() = default;


QFont NativeTextField::font() const
{
	return d->font;
}


void NativeTextField::setFont(const QFont& pFont)
{
	QFont font = pFont;
	if (font.pointSizeF() == -1)
	{
		QScreen* screen = QGuiApplication::primaryScreen();
		qreal dpi = screen->logicalDotsPerInch();
		font.setPointSizeF(font.pixelSize() * 72.0 / dpi);
	}

	if (font == d->font)
	{
		return;
	}

	d->textInput.font = [UIFont fontWithName: font.family().toNSString() size: font.pointSizeF()];
	d->updateImplicitSize();
	d->font = font;
	Q_EMIT fontChanged();
}


QColor NativeTextField::textColor() const
{
	return fromUIColor(d->textInput.textColor);
}


void NativeTextField::setTextColor(QColor pColor)
{
	d->textInput.textColor = [UIColor
			colorWithRed: pColor.redF()
			green: pColor.greenF()
			blue: pColor.blueF()
			alpha: pColor.alphaF()];

	[d->textInput setNeedsDisplay];
	Q_EMIT textColorChanged();
}


QColor NativeTextField::backgroundColor() const
{
	return fromUIColor(d->textInput.backgroundColor);
}


void NativeTextField::setBackgroundColor(QColor pColor)
{
	d->textInput.backgroundColor = [UIColor
			colorWithRed: pColor.redF()
			green: pColor.greenF()
			blue: pColor.blueF()
			alpha: pColor.alphaF()];

	[d->textInput setNeedsDisplay];
	Q_EMIT backgroundColorChanged();
}


QColor NativeTextField::borderColor() const
{
	return fromUIColor([UIColor colorWithCGColor:d->textInput.layer.borderColor]);
}


void NativeTextField::setBorderColor(QColor pColor)
{
	d->textInput.layer.borderColor = [[UIColor
			colorWithRed: pColor.redF()
			green: pColor.greenF()
			blue: pColor.blueF()
			alpha: pColor.alphaF()] CGColor];

	[d->textInput setNeedsDisplay];
	Q_EMIT borderColorChanged();
}


qreal NativeTextField::borderWidth() const
{
	return d->textInput.layer.borderWidth;
}


void NativeTextField::setBorderWidth(qreal pWidth)
{
	d->textInput.layer.borderWidth = pWidth;

	[d->textInput setNeedsDisplay];
	Q_EMIT borderWidthChanged();
}


qreal NativeTextField::borderRadius() const
{
	return d->textInput.layer.cornerRadius;
}


void NativeTextField::setBorderRadius(qreal pRadius)
{
	d->textInput.layer.cornerRadius = pRadius;
	d->textInput.layer.masksToBounds = YES;

	[d->textInput setNeedsDisplay];
	Q_EMIT borderRadiusChanged();
}


int NativeTextField::maximumLength() const
{
	return d->maximumLength;
}


void NativeTextField::setMaximumLength(int pLength)
{
	if (pLength == d->maximumLength)
	{
		return;
	}

	d->maximumLength = pLength;
	Q_EMIT maximumLengthChanged();

	setText(text());
}


QString NativeTextField::text() const
{
	return QString::fromNSString(d->textInput.text);
}


void NativeTextField::setText(const QString& pText)
{
	QString text = pText;
	if (text.length() > d->maximumLength)
	{
		text.truncate(d->maximumLength);
	}

	d->textInput.text = text.toNSString();
	Q_EMIT textChanged();
	d->updateImplicitSize();
}


QWindow* NativeTextField::nativeWindow() const
{
	return d->window.data();
}


void NativeTextField::onParentChanged()
{
	d->clearParentConnections();

	QQuickItem* parent = this;
	do
	{
		d->parentConnections += connect(parent, &QQuickItem::opacityChanged, this, &NativeTextField::updateEffectiveOpacity);
		d->parentConnections += connect(parent, &QQuickItem::parentChanged, this, &NativeTextField::onParentChanged);
		parent = parent->parentItem();
	}
	while (parent);
}


void NativeTextField::updateEffectiveOpacity()
{
	qreal opacity = 1;
	QQuickItem* parent = this;
	do
	{
		opacity *= parent->opacity();
		parent = parent->parentItem();
	}
	while (parent && opacity != 0);

	d->textInput.alpha = opacity;
	[d->textInput setNeedsDisplay];
}


void NativeTextField::sinkFocus() const
{
	/* We use the focusSink to remove the focus from the previous item to keep the focus state
	 * consistent within the view in which NativeTextField is placed. Otherwise the focus would
	 * remain on the previous item and cause issues with the (a11y) navigation. We cannot set the
	 * focus on NativeTextField itself, because this activates the cursor which is not desired.
	 **/
	d->focusSink->forceActiveFocus();
}
