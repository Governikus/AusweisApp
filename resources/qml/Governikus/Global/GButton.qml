/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0


Button {
	id: root

	property TextStyle textStyle: enabled && enableButton ? Style.text.button : Style.text.button_disabled
	property color buttonColor: Style.color.button
	property int cursorShape: Qt.PointingHandCursor
	property bool animationsDisabled: false
	property color textHighlightColor: textStyle.textColor

	/// Similar to "enabled", but tooltips will continue to work
	property bool enableButton: true

	property string enabledTooltipText
	property string disabledTooltipText
	property bool tintIcon: false

	activeFocusOnTab: enableButton
	onActiveFocusOnTabChanged: if (!activeFocusOnTab) focus = false
	Accessible.name: text
	Accessible.onPressAction: if (Qt.platform.os === "ios") clicked(null)

	ToolTip.delay: Constants.toolTipDelay
	ToolTip.visible: hovered && ToolTip.text !== ""
	ToolTip.text: enableButton ? enabledTooltipText : disabledTooltipText

	font.pixelSize: textStyle.textSize

	contentItem: Item {
		z: 1

		// The icon's width is already included in the text's margins
		implicitWidth: buttonText.visible ? Math.max(buttonText.effectiveWidth, Style.dimens.large_icon_size) : buttonIcon.width
		implicitHeight: root.textStyle.textSize + 2 * verticalPadding

		TintableIcon {
			id: buttonIcon

			readonly property color iconColor: root.textStyle.textColor
			readonly property color pressColor: Qt.darker(iconColor, Constants.highlightDarkerFactor)

			visible: source != ""
			anchors.left: parent.left
			anchors.verticalCenter: parent.verticalCenter

			source: root.icon.source
			sourceSize.height: parent.height

			tintEnabled: tintIcon
			tintColor: !animationsDisabled && root.pressed ? pressColor : iconColor
		}

		GText {
			id: buttonText

			readonly property real effectiveWidth: implicitWidth + anchors.leftMargin + anchors.rightMargin

			visible: text !== ""
			anchors.left: parent.left
			anchors.leftMargin: buttonIcon.visible ? buttonIcon.width + Constants.text_spacing : 0
			anchors.right: parent.right
			anchors.verticalCenter: parent.verticalCenter

			Accessible.ignored: true

			font: root.font
			textStyle: root.textStyle
			color: !animationsDisabled && root.pressed ? root.textHighlightColor : root.textStyle.textColor
			text: root.text
			horizontalAlignment: buttonIcon.visible ? Text.AlignLeft : Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			maximumLineCount: 1
			elide: Text.ElideRight

			FocusFrame {
				scope: root
				marginFactor: 0.7
			}
		}
	}

	background: Rectangle {
		readonly property color pressColor: Qt.darker(buttonColor, Constants.highlightDarkerFactor)

		radius: Style.dimens.button_radius
		color: {
			if (!enabled || !enableButton) {
				return Style.color.button_disabled
			}

			return !animationsDisabled && pressed ? pressColor : buttonColor
		}
		border.width: Style.dimens.high_contrast_item_border
		border.color: Style.color.high_contrast_item_border
	}

	MouseArea {
		z: 2
		anchors.fill: parent

		onPressed: mouse.accepted = !enableButton
		cursorShape: enableButton ? root.cursorShape : Qt.ArrowCursor
	}

	GDropShadow {
		visible: !Constants.is_desktop && GraphicsInfo.api !== GraphicsInfo.Software
		anchors.fill: source

		source: root.background
	}
}
