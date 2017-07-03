/*!
 * KeylessPasswordEdit.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "KeylessPasswordEdit.h"

#include <QDateTime>
#include <QGridLayout>
#include <QLineEdit>
#include <QMargins>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QToolButton>
#include <QWheelEvent>

using namespace governikus;

static const int MAX_MAX_LENGTH = 256;


static inline void initRandom()
{
	static int dummy = (qsrand(uint(QDateTime::currentMSecsSinceEpoch())), 0);
	(void) dummy;
}


static int randomNumber()
{
	initRandom();
	return qrand();
}


class KeylessPasswordEdit::RenderingHelper
{
	public:
		RenderingHelper(QWidget* pParent)
			: mParent(pParent)
			, mLayoutValid(false)
			, mHiddenDigitText()
			, mUnsetDigitText(QStringLiteral("-"))
			, mDigitPadding(4, 4, 4, 4)
			, mButtonHeight(10)
			, mButtonSpacing(4)
			, mUpButtonRect()
			, mDownButtonRect()
			, mDigitRect()
			, mTotalSize()
		{
			QLineEdit lineEdit;
			lineEdit.setEchoMode(QLineEdit::Password);
			lineEdit.setText(QStringLiteral("x"));
			mHiddenDigitText = lineEdit.displayText();
		}


		const QString& getHiddenDigitText() const
		{
			return mHiddenDigitText;
		}


		const QString& getUnsetDigitText() const
		{
			return mUnsetDigitText;
		}


		QSize getTotalSize()
		{
			validateLayout();
			return mTotalSize;
		}


		const QRect& getDigitRect()
		{
			validateLayout();
			return mDigitRect;
		}


		const QMargins& getDigitPadding() const
		{
			return mDigitPadding;
		}


		const QRect& getUpButtonRect()
		{
			validateLayout();
			return mUpButtonRect;
		}


		const QRect& getDownButtonRect()
		{
			validateLayout();
			return mDownButtonRect;
		}


		QColor getInactiveDigitBackgroundColor() const
		{
			return QColor("#FFFFFF");
		}


		QColor getActiveDigitBackgroundColor() const
		{
			return QColor("#FFF9D2");
		}


		QColor getPressedButtonBackgroundColor() const
		{
			return QColor("#FFCC66");
		}


		QColor getNotPressedButtonBackgroundColor() const
		{
			return QColor("#FFF9D2");
		}


		QPainterPath getButtonTrianglePath(bool pUp)
		{
			validateLayout();

			QSizeF size = pUp ? mUpButtonRect.size() : mDownButtonRect.size();
			qreal padding = 2;
			qreal center = size.width() / 2;
			qreal top = padding;
			qreal bottom = size.height() - padding;
			qreal tip = pUp ? top : bottom;
			qreal butt = pUp ? bottom : top;
			QPainterPath path;
			path.moveTo(center, tip);
			path.lineTo(padding, butt);
			path.lineTo(size.width() - padding, butt);
			path.closeSubpath();
			return path;
		}


	private:
		void validateLayout()
		{
			if (mLayoutValid)
			{
				return;
			}

			QSize maxDigitSize = QSize(0, 0);

			const auto possibleChars = QStringLiteral("0123456789") + mHiddenDigitText + mUnsetDigitText;
			QFontMetrics myFontMetrics = mParent->fontMetrics();
			int height = myFontMetrics.height();
			for (QChar c : possibleChars)
			{
				QSize digitSize(myFontMetrics.boundingRect(c).width(), height);
				maxDigitSize = maxDigitSize.expandedTo(digitSize);
			}

			mUpButtonRect = QRect(0, 0, maxDigitSize.width() + mDigitPadding.left() + mDigitPadding.right(), mButtonHeight);
			mDigitRect = QRect(0, mUpButtonRect.top() + mUpButtonRect.height() + mButtonSpacing, mUpButtonRect.width(), maxDigitSize.height() + mDigitPadding.top() + mDigitPadding.bottom());
			mDownButtonRect = QRect(0, mDigitRect.top() + mDigitRect.height() + mButtonSpacing, mUpButtonRect.width(), mButtonHeight);
			mTotalSize = QSize(mUpButtonRect.width(), mDownButtonRect.top() + mDownButtonRect.height());

			mLayoutValid = true;
		}


	private:
		QWidget* mParent;
		bool mLayoutValid;
		QString mHiddenDigitText;
		QString mUnsetDigitText;
		QMargins mDigitPadding;
		int mButtonHeight;
		int mButtonSpacing;
		QRect mUpButtonRect;
		QRect mDownButtonRect;
		QRect mDigitRect;
		QSize mTotalSize;
		QLineEdit mLineEdit;
};


class KeylessPasswordEdit::DigitButton
	: public QToolButton
{
	Q_OBJECT

	public:
		DigitButton(RenderingHelper* pRenderingHelper, bool pIsUpButton, QWidget* pParent = nullptr)
			: QToolButton(pParent)
			, mRenderingHelper(pRenderingHelper)
			, mIsUpButton(pIsUpButton)
		{
			setAutoRepeat(true);
			setAutoRepeatDelay(1000);
			setAutoRepeatInterval(500);
		}


	protected:
		virtual void paintEvent(QPaintEvent* /*pEvent*/) override
		{
			QStyleOption opt;
			opt.init(this);
			QPainter painter(this);
			style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

			painter.setRenderHint(QPainter::Antialiasing);
			painter.fillPath(mRenderingHelper->getButtonTrianglePath(mIsUpButton), QColor(0, 0, 0));
		}


	private:
		RenderingHelper* mRenderingHelper;
		bool mIsUpButton;
};


class KeylessPasswordEdit::DigitField
	: public QWidget
{
	Q_OBJECT

	public:
		enum State
		{
			UNSET,
			ACTIVE,
			INACTIVE,
		};

	public:
		DigitField(RenderingHelper* pRenderingHelper, QWidget* pParent = nullptr)
			: QWidget(pParent)
			, mRenderingHelper(pRenderingHelper)
			, mValue(0)
			, mState(State::UNSET)
			, mWheelDelta(0)
			, mUpButton(new DigitButton(pRenderingHelper, true, this))
			, mDownButton(new DigitButton(pRenderingHelper, false, this))
			, mActivationAnimationTimer(0)
			, mActivationAnimationSteps(0)
			, mDeactivationTimer(0)
			, mDigitFieldInvalid(false)
		{
			mUpButton->setVisible(false);
			mDownButton->setVisible(false);

			mUpButton->setGeometry(mRenderingHelper->getUpButtonRect());
			mDownButton->setGeometry(mRenderingHelper->getDownButtonRect());

			connect(mUpButton, &QAbstractButton::pressed, this, &DigitField::onUpButtonPressed);
			connect(mDownButton, &QAbstractButton::pressed, this, &DigitField::onDownButtonPressed);
		}


		bool isActive() const
		{
			return mState == State::ACTIVE;
		}


		bool hasValue() const
		{
			return mState != State::UNSET;
		}


		int getValue() const
		{
			return mValue;
		}


		void setValue(int pValue)
		{
			mValue = pValue;

			if (!hasValue())
			{
				setState(State::INACTIVE);
			}
		}


		State getState() const
		{
			return mState;
		}


		void setState(State pState)
		{
			if (pState != mState)
			{
				bool wasActive = isActive();

				mState = pState;
				mWheelDelta = 0;

				bool active = isActive();
				mUpButton->setVisible(active);
				mDownButton->setVisible(active);

				if (active)
				{
					if (!wasActive)
					{
						mValue = randomNumber() % 10;
						startActivationAnimation();
						Q_EMIT valueEdited();
					}
				}
				else
				{
					stopDeactivationTimer();
				}

				update();
			}
		}


		virtual QSize sizeHint() const override
		{
			return mRenderingHelper->getTotalSize();
		}


		virtual QSize minimumSizeHint() const override
		{
			return mRenderingHelper->getTotalSize();
		}


		void setDigitFieldInvalid(bool pInvalid)
		{
			mDigitFieldInvalid = pInvalid;
		}


		void addToValue(int pDelta)
		{
			mValue = ((mValue + pDelta) % 10 + 10) % 10;
			update();
			startDeactivationTimer(false);

			Q_EMIT valueEdited();
		}


	Q_SIGNALS:
		void activationRequested();
		void deactivationRequested();
		void valueEdited();

	protected:
		virtual void mousePressEvent(QMouseEvent* pEvent) override
		{
			Q_EMIT activationRequested();
			pEvent->accept();
		}


		virtual void wheelEvent(QWheelEvent* pEvent) override
		{
			if (!isActive())
			{
				pEvent->ignore();
				return;
			}

			mWheelDelta += pEvent->angleDelta().y();
			int steps = mWheelDelta / 120;
			if (steps != 0)
			{
				mWheelDelta -= steps * 120;
				addToValue(steps);
			}

			pEvent->accept();
		}


		virtual void paintEvent(QPaintEvent* /*pEvent*/) override
		{
			QStyleOption opt;
			opt.init(this);
			QPainter painter(this);
			style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

			QString textToDraw;

			switch (mState)
			{
				case State::UNSET:
					textToDraw = mRenderingHelper->getUnsetDigitText();
					break;

				case State::INACTIVE:
					textToDraw = mRenderingHelper->getHiddenDigitText();
					break;

				case State::ACTIVE:
					textToDraw = QString(QLatin1Char(static_cast<char>('0' + char(mValue))));
					break;
			}

			QRect digitRect = mRenderingHelper->getDigitRect();

			if (mDigitFieldInvalid && getState() == State::ACTIVE)
			{
				painter.fillRect(digitRect, Qt::red);
			}

			painter.drawRect(digitRect.x(), digitRect.y(), digitRect.width() - 1, digitRect.height() - 1);

			digitRect -= mRenderingHelper->getDigitPadding();


			painter.drawText(digitRect, Qt::AlignHCenter | Qt::AlignVCenter, textToDraw);
		}


		virtual void timerEvent(QTimerEvent* pEvent) override
		{
			if (pEvent->timerId() == mActivationAnimationTimer)
			{
				mValue = randomNumber() % 10;
				update();

				if (--mActivationAnimationSteps <= 0)
				{
					stopActivationAnimation();
					startDeactivationTimer(true);
				}

				Q_EMIT valueEdited();
			}
			else if (pEvent->timerId() == mDeactivationTimer)
			{
				Q_EMIT deactivationRequested();
			}
		}


	private Q_SLOTS:
		void onDownButtonPressed()
		{
			if (isActive())
			{
				addToValue(-1);
			}
		}


		void onUpButtonPressed()
		{
			if (isActive())
			{
				addToValue(1);
			}
		}


	private:
		void startActivationAnimation()
		{
			stopActivationAnimation();

			mActivationAnimationTimer = startTimer(100);
			mActivationAnimationSteps = 5;
		}


		void stopActivationAnimation()
		{
			if (mActivationAnimationTimer != 0)
			{
				killTimer(mActivationAnimationTimer);
				mActivationAnimationTimer = 0;
			}
		}


		void startDeactivationTimer(bool pLongDelay)
		{
			stopDeactivationTimer();

			mDeactivationTimer = startTimer(pLongDelay ? 10000 : 3000);
		}


		void stopDeactivationTimer()
		{
			if (mDeactivationTimer != 0)
			{
				killTimer(mDeactivationTimer);
				mDeactivationTimer = 0;
			}
		}


	private:
		RenderingHelper* mRenderingHelper;
		int mValue;
		State mState;
		int mWheelDelta;
		DigitButton* mUpButton;
		DigitButton* mDownButton;
		int mActivationAnimationTimer;
		int mActivationAnimationSteps;
		int mDeactivationTimer;
		bool mDigitFieldInvalid;
};


KeylessPasswordEdit::KeylessPasswordEdit(QWidget* pParent)
	: QWidget(pParent)
	, mLayout(new QGridLayout(this))
	, mMinLength(0)
	, mMaxLength(0)
	, mRenderingHelper(new RenderingHelper(this))
	, mDigitFields()
	, mActiveFieldIndex(-1)
{
	mLayout->setMargin(0);

	QFont myFont = font();
	myFont.setBold(true);
	myFont.setPointSize(myFont.pointSize() + 2);
	setFont(myFont);

	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	setMaxLength(6);

	setFocusPolicy(Qt::StrongFocus);
}


KeylessPasswordEdit::~KeylessPasswordEdit()
{
}


void KeylessPasswordEdit::setMinLength(int pLength)
{
	if (pLength < 0 || pLength > MAX_MAX_LENGTH || pLength == mMinLength)
	{
		return;
	}

	if (pLength > mMaxLength)
	{
		setMaxLength(pLength);
	}

	mMinLength = pLength;
}


void KeylessPasswordEdit::setMaxLength(int pLength)
{
	if (pLength < 1 || pLength > MAX_MAX_LENGTH)
	{
		return;
	}

	if (pLength < mMinLength)
	{
		setMinLength(pLength);
	}

	if (mActiveFieldIndex >= pLength)
	{
		setActiveField(-1);
	}

	mMaxLength = pLength;

	while (mMaxLength > mDigitFields.size())
	{
		DigitField* field = new DigitField(mRenderingHelper.data(), this);
		mLayout->addWidget(field, 0, mDigitFields.size());
		mDigitFields += field;

		connect(field, &DigitField::activationRequested, this, &KeylessPasswordEdit::onFieldActivationRequested);
		connect(field, &DigitField::deactivationRequested, this, &KeylessPasswordEdit::onFieldDeactivationRequested);
		connect(field, &DigitField::valueEdited, this, &KeylessPasswordEdit::onFieldValueEdited);
	}

	while (mMaxLength < mDigitFields.size())
	{
		DigitField* field = mDigitFields.takeLast();
		delete field;
	}
}


void KeylessPasswordEdit::clear()
{
	setActiveField(-1);

	for (auto field : qAsConst(mDigitFields))
	{
		field->setState(DigitField::State::UNSET);
	}
}


QString KeylessPasswordEdit::text() const
{
	QString result;
	for (auto field : qAsConst(mDigitFields))
	{
		if (!field->hasValue())
		{
			break;
		}

		int value = field->getValue();
		result += char('0' + value);
	}

	return result;
}


void KeylessPasswordEdit::setText(const QString& pText)
{
	setActiveField(-1);

	int fieldCount = mDigitFields.length();
	int length = std::min(pText.length(), fieldCount);
	for (int i = 0; i < length; ++i)
	{
		mDigitFields.at(i)->setValue(pText.midRef(i, 1).toInt());
	}

	for (int i = length; i < fieldCount; ++i)
	{
		mDigitFields.at(i)->setState(DigitField::State::UNSET);
	}
}


void KeylessPasswordEdit::paintEvent(QPaintEvent* /*pEvent*/)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


void KeylessPasswordEdit::focusInEvent(QFocusEvent* /*pEvent*/)
{
	// if completely unset, activate the first field
	if (mActiveFieldIndex < 0 && !mDigitFields.isEmpty() && !mDigitFields.at(0)->hasValue())
	{
		setActiveField(0);
	}
}


void KeylessPasswordEdit::focusOutEvent(QFocusEvent* /*pEvent*/)
{
	// deactivate the active field
	if (mActiveFieldIndex >= 0)
	{
		setActiveField(-1);
	}
}


void KeylessPasswordEdit::keyPressEvent(QKeyEvent* pEvent)
{
	if (mDigitFields.isEmpty())
	{
		return;
	}

	int lastSetField = -1;
	for (int i = mDigitFields.count() - 1; i >= 0; i--)
	{
		if (mDigitFields.at(i)->hasValue())
		{
			lastSetField = i;
			break;
		}
	}

	switch (pEvent->key())
	{
		case Qt::Key_Up:
			if (mActiveFieldIndex >= 0)
			{
				mDigitFields.at(mActiveFieldIndex)->addToValue(1);
			}
			break;

		case Qt::Key_Down:
			if (mActiveFieldIndex >= 0)
			{
				mDigitFields.at(mActiveFieldIndex)->addToValue(-1);
			}
			break;

		case Qt::Key_Left:
			if (lastSetField > 0)
			{
				setActiveField(lastSetField - 1);
			}
			break;

		case Qt::Key_Right:
			if (lastSetField + 1 < mDigitFields.count())
			{
				setActiveField(lastSetField + 1);
			}
			break;

		default:
			return;
	}

	pEvent->accept();
}


void KeylessPasswordEdit::onFieldActivationRequested()
{
	DigitField* field = qobject_cast<DigitField*>(sender());
	int fieldIndex = field != nullptr ? mDigitFields.indexOf(field) : -1;
	if (fieldIndex >= 0)
	{
		setActiveField(fieldIndex);
		return;
	}
}


void KeylessPasswordEdit::onFieldDeactivationRequested()
{
	DigitField* field = qobject_cast<DigitField*>(sender());
	int fieldIndex = field != nullptr ? mDigitFields.indexOf(field) : -1;
	if (fieldIndex >= 0 && fieldIndex == mActiveFieldIndex)
	{
		setActiveField(-1);
		return;
	}
}


void KeylessPasswordEdit::onFieldValueEdited()
{
	Q_EMIT textEdited(text());
}


void KeylessPasswordEdit::setDigitFieldsInvalid(bool pInvalid)
{
	for (auto field : qAsConst(mDigitFields))
	{
		field->setDigitFieldInvalid(pInvalid);
	}
}


void KeylessPasswordEdit::setActiveField(int pIndex)
{
	if (pIndex == mActiveFieldIndex)
	{
		return;
	}

	// Activate the specified only, if there aren't any unset fields before it.
	if (pIndex > 0 && !mDigitFields.at(pIndex - 1)->hasValue())
	{
		return;
	}

	// deactivate the previously active field
	if (mActiveFieldIndex >= 0)
	{
		mDigitFields.at(mActiveFieldIndex)->setState(DigitField::State::INACTIVE);
		mActiveFieldIndex = -1;
	}

	// activate the new field
	mActiveFieldIndex = pIndex;

	if (mActiveFieldIndex >= 0)
	{
		mDigitFields.at(pIndex)->setState(DigitField::State::ACTIVE);

		// unset all fields after the newly active one
		int fieldCount = mDigitFields.count();
		for (int i = pIndex + 1; i < fieldCount; ++i)
		{
			mDigitFields.at(i)->setState(DigitField::State::UNSET);
		}
	}
}


#include "KeylessPasswordEdit.moc"
