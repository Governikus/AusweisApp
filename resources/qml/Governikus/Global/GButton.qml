/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

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

	padding: Constants.text_spacing

	font.pixelSize: textStyle.textSize

	ToolTip.visible: hovered && ToolTip.text !== ""
	ToolTip.text: enableButton ? enabledTooltipText : disabledTooltipText

	contentItem: RowLayout {
		z: 1

		spacing: root.icon.source != "" ? Constants.text_spacing : 0

		TintableIcon {
			readonly property color iconColor: root.textStyle.textColor
			readonly property color pressColor: Qt.darker(iconColor, Constants.highlightDarkerFactor)

			Layout.preferredHeight: sourceSize.height

			source: root.icon.source
			sourceSize.height: root.textStyle.textSize + root.horizontalPadding

			tintEnabled: tintIcon
			tintColor: !animationsDisabled && root.pressed ? pressColor : iconColor
		}

		GText {
			Accessible.ignored: true

			Layout.fillWidth: true

			font: root.font
			textStyle: root.textStyle
			color: !animationsDisabled && root.pressed ? root.textHighlightColor : root.textStyle.textColor
			text: root.text
			horizontalAlignment: root.icon.source == "" ? Text.AlignHCenter : Text.AlignLeft

			FocusFrame {
				scope: root
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

	DropShadow {
		visible: !Constants.is_desktop && Constants.is_layout_android && GraphicsInfo.api !== GraphicsInfo.Software
		anchors.fill: source

		source: root.background
		radius: 8
		samples: 8
		fast: true
		color: Style.color.shadow
		verticalOffset: 2
	}
}
