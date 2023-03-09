/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

Button {
	id: root

	property bool animationsDisabled: false
	property color buttonColor: Style.color.button
	property int cursorShape: Qt.PointingHandCursor
	property string disabledTooltipText

	// Similar to "enabled", but tooltips will continue to work
	property bool enableButton: true
	property string enabledTooltipText
	property alias maximumLineCount: buttonText.maximumLineCount
	property color textHighlightColor: textStyle.textColor
	property TextStyle textStyle: enabled && enableButton ? Style.text.button : Style.text.button_disabled
	property bool tintIcon: false

	Accessible.name: text
	ToolTip.delay: Constants.toolTipDelay
	ToolTip.text: enableButton ? enabledTooltipText : disabledTooltipText
	ToolTip.visible: hovered && ToolTip.text !== ""
	activeFocusOnTab: enableButton

	background: Rectangle {
		readonly property color pressColor: Qt.darker(buttonColor, Constants.highlightDarkerFactor)

		border.color: Style.color.high_contrast_item_border
		border.width: Style.dimens.high_contrast_item_border
		color: {
			if (!enabled || !enableButton) {
				return Style.color.button_disabled;
			}
			return !animationsDisabled && pressed ? pressColor : buttonColor;
		}
		radius: Style.dimens.button_radius
	}
	contentItem: RowLayout {
		spacing: Constants.text_spacing
		z: 1

		TintableIcon {
			id: buttonIcon

			readonly property color iconColor: root.textStyle.textColor
			readonly property color pressColor: Qt.darker(iconColor, Constants.highlightDarkerFactor)

			source: root.icon.source
			sourceSize.height: root.textStyle.textSize + 2 * verticalPadding
			tintColor: !animationsDisabled && root.pressed ? pressColor : iconColor
			tintEnabled: tintIcon
			visible: source != ""
			width: height
		}
		GText {
			id: buttonText
			Accessible.ignored: true
			Layout.fillWidth: true
			Layout.minimumHeight: root.textStyle.textSize + 2 * verticalPadding
			Layout.minimumWidth: Style.dimens.large_icon_size - x
			color: !animationsDisabled && root.pressed ? root.textHighlightColor : root.textStyle.textColor
			elide: Text.ElideRight
			horizontalAlignment: buttonIcon.visible ? Text.AlignLeft : Text.AlignHCenter
			maximumLineCount: 1
			text: root.text
			textStyle: root.textStyle
			verticalAlignment: Text.AlignVCenter
			visible: text !== ""

			FocusFrame {
				isOnLightBackground: !root.background
				marginFactor: 0.7
				scope: root
			}
		}
	}

	Accessible.onPressAction: clicked()
	onActiveFocusOnTabChanged: if (!activeFocusOnTab)
		focus = false

	MouseArea {
		anchors.fill: parent
		cursorShape: enableButton ? root.cursorShape : Qt.ArrowCursor
		z: 2

		onPressed: mouse => {
			mouse.accepted = !enableButton;
		}
	}
}
