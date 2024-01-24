/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type.SettingsModel

Button {
	id: numberPadButton

	property string a11yDisabledText: qsTr("Disabled")
	property string a11yText
	property bool visualPrivacy: SettingsModel.visualPrivacy

	Accessible.name: numberPadButton.enabled ? a11yText !== "" ? a11yText : text : a11yDisabledText
	Layout.maximumHeight: implicitHeight
	Layout.maximumWidth: implicitWidth
	Layout.minimumHeight: Layout.minimumWidth * d.aspectRatio
	Layout.minimumWidth: 36
	implicitHeight: implicitWidth * d.aspectRatio
	implicitWidth: 75

	background: Rectangle {
		anchors.fill: numberPadButton
		color: numberPadButton.enabled ? (numberPadButton.down && !visualPrivacy ? Style.color.control : Style.color.pane_sublevel) : Style.color.control_disabled
		radius: Style.dimens.control_radius

		TintableIcon {
			anchors.centerIn: parent
			fillMode: Image.Pad
			source: numberPadButton.icon.source
			sourceSize.height: contentItem.font.pixelSize * 1.5
			tintColor: numberPadButton.down && !visualPrivacy ? Style.color.control_pressed : contentText.textStyle.textColor
			tintEnabled: true
			visible: numberPadButton.icon.source != ""
		}
	}
	contentItem: GText {
		id: contentText

		Accessible.ignored: true
		color: numberPadButton.down && !visualPrivacy ? Style.color.control_pressed : textStyle.textColor
		horizontalAlignment: Text.AlignHCenter
		text: numberPadButton.text
		textStyle: Style.text.headline
		visible: numberPadButton.icon.source == ""
	}

	Accessible.onPressAction: if (numberPadButton.enabled)
		clicked()

	QtObject {
		id: d

		readonly property real aspectRatio: 0.7
	}
}
