/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style

FocusScope {
	property alias checked: entrySwitch.checked
	readonly property real contentMargin: Constants.is_desktop ? 0 : Constants.component_spacing
	property alias description: descriptionText.text
	property bool drawBottomCorners: false
	property bool drawTopCorners: false
	property alias title: titleText.text

	Accessible.checkable: enabled
	Accessible.checked: checked
	Accessible.name: title + ". " + description
	Accessible.role: Accessible.CheckBox
	Layout.fillWidth: true
	activeFocusOnTab: true
	height: implicitHeight
	implicitHeight: contentItem.implicitHeight + 2 * contentMargin
	implicitWidth: contentItem.implicitWidth

	Accessible.onPressAction: entrySwitch.toggle()
	Accessible.onToggleAction: entrySwitch.toggle()

	RoundedRectangle {
		anchors.fill: parent
		bottomLeftCorner: parent.drawBottomCorners
		bottomRightCorner: parent.drawBottomCorners
		color: mouseArea.pressed ? Style.color.pane_active : Style.color.transparent
		topLeftCorner: parent.drawTopCorners
		topRightCorner: parent.drawTopCorners

		RowLayout {
			id: contentItem

			anchors.fill: parent
			anchors.leftMargin: contentMargin
			anchors.rightMargin: contentMargin
			layoutDirection: Constants.is_desktop ? Qt.RightToLeft : Qt.LeftToRight
			spacing: Constants.component_spacing

			ColumnLayout {
				spacing: Constants.subtext_spacing

				GText {
					id: titleText

					Accessible.ignored: true
					Layout.maximumWidth: Number.POSITIVE_INFINITY
					Layout.preferredHeight: Constants.is_desktop ? Math.max(entrySwitch.implicitHeight, implicitHeight) : -1
					color: entrySwitch.enabled ? (mouseArea.pressed ? Style.color.text_subline_pressed : textStyle.textColor) : Style.color.text_subline_disabled
					textStyle: Style.text.subline
				}
				GText {
					id: descriptionText

					Accessible.ignored: true
					Layout.maximumWidth: Number.POSITIVE_INFINITY
					color: entrySwitch.enabled ? (mouseArea.pressed ? Style.color.text_pressed : textStyle.textColor) : Style.color.text_disabled
					visible: text !== ""
				}
			}
			GSwitch {
				id: entrySwitch

				Accessible.ignored: true
				Layout.alignment: Constants.is_desktop ? Qt.AlignTop : Qt.AlignVCenter
				activeFocusOnTab: false
				focus: true
				overwriteHovered: mouseArea.containsMouse
				overwritePressed: mouseArea.pressed
				text: titleText.text
			}
		}
		MouseArea {
			id: mouseArea

			anchors.fill: parent
			hoverEnabled: true

			onClicked: entrySwitch.toggle()
		}
	}
}
