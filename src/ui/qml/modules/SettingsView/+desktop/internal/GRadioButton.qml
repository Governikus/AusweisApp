/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Global
import Governikus.Style

GAbstractButton {
	id: root

	enum Position {
		Left,
		Middle,
		Right
	}

	required property string desc
	required property url img
	required property string name
	required property int position
	property alias tintIcon: icon.tintEnabled

	function getPosition(index: int, buttonCount: int): int {
		if (index === 0)
			return GRadioButton.Position.Left;
		else if (index === buttonCount - 1)
			return GRadioButton.Position.Right;
		else
			return GRadioButton.Position.Middle;
	}

	Accessible.description: desc
	Accessible.name: name
	Accessible.role: Accessible.RadioButton
	autoExclusive: true
	bottomPadding: backgroundRectangle.radius
	checkable: true
	icon.source: img
	leftPadding: root.position === GRadioButton.Position.Left ? backgroundRectangle.radius : backgroundRectangle.radius / 2
	rightPadding: root.position === GRadioButton.Position.Right ? backgroundRectangle.radius : backgroundRectangle.radius / 2
	topPadding: backgroundRectangle.radius
	z: activeFocus ? 1 : 0

	background: Rectangle {
		id: backgroundRectangle

		border.color: colors.controlBorder
		border.width: Style.dimens.border_width
		bottomLeftRadius: root.position === GRadioButton.Position.Left ? radius : 0
		bottomRightRadius: root.position === GRadioButton.Position.Right ? radius : 0
		color: colors.controlBackground
		radius: Style.dimens.pane_radius
		topLeftRadius: root.position === GRadioButton.Position.Left ? radius : 0
		topRightRadius: root.position === GRadioButton.Position.Right ? radius : 0

		Rectangle {
			// overdraw left border to hide it
			anchors.left: backgroundRectangle.left
			anchors.top: backgroundRectangle.top
			anchors.topMargin: border.width
			color: colors.controlBackground
			height: backgroundRectangle.height - 2 * backgroundRectangle.border.width
			visible: root.position !== GRadioButton.Position.Left
			width: backgroundRectangle.border.width
		}
		Rectangle {
			// overdraw right border of previous button when this button is checked to hide it
			anchors.right: backgroundRectangle.left
			anchors.top: backgroundRectangle.top
			color: colors.controlBorder
			height: backgroundRectangle.height
			visible: root.position !== GRadioButton.Position.Left && root.checked
			width: backgroundRectangle.border.width
		}
		GRadioButtonFocusFrame {
			bottomLeftRadius: backgroundRectangle.bottomLeftRadius
			bottomRightRadius: backgroundRectangle.bottomRightRadius
			checked: root.checked
			show: root.activeFocus
			topLeftRadius: backgroundRectangle.topLeftRadius
			topRightRadius: backgroundRectangle.topRightRadius
		}
	}
	contentItem: Row {
		spacing: Style.dimens.text_spacing

		TintableIcon {
			id: icon

			anchors.verticalCenter: parent.verticalCenter
			source: root.icon.source
			sourceSize.height: Style.dimens.small_icon_size
			tintColor: colors.controlContent
		}
		GText {
			id: label

			Accessible.ignored: true
			anchors.verticalCenter: parent.verticalCenter
			color: colors.controlContent
			text: root.name
		}
	}

	Accessible.onPressAction: toggled() // Windows
	Accessible.onToggleAction: toggled() // macOS

	Keys.onSpacePressed: event => {
		toggled();
		if (Qt.platform.os === "windows" && checked) {
			//: DESKTOP
			Accessible.announce(qsTr("checked"));
		}
		event.accepted = true;
	}

	StatefulColors {
		id: colors

		controlStyle: Style.color.controlRadiobutton
		groupMember: true
		statefulControl: root
	}
}
