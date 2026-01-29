/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Governikus.Style
import Governikus.Type
import Governikus.View

AbstractButton {
	id: root

	property alias description: descriptionText.text
	property bool drawBottomCorners: false
	property bool drawTopCorners: false

	Accessible.description: description
	Accessible.name: text
	Accessible.role: {
		if ("Switch" in Accessible) {
			return Accessible.Switch; // qmllint disable missing-property
		}
		return Accessible.CheckBox;
	}
	checkable: true
	horizontalPadding: Style.dimens.pane_spacing
	verticalPadding: Style.dimens.pane_spacing

	background: RoundedRectangle {
		bottomLeftCorner: root.drawBottomCorners
		bottomRightCorner: root.drawBottomCorners
		color: colors.controlPreferredPaneBackground
		topLeftCorner: root.drawTopCorners
		topRightCorner: root.drawTopCorners
	}
	contentItem: RowLayout {
		spacing: Style.dimens.pane_spacing

		ColumnLayout {
			spacing: Style.dimens.subtext_spacing
			visible: titleText.text !== "" || descriptionText.text !== ""

			GText {
				id: titleText

				Accessible.ignored: true
				Layout.maximumWidth: Number.POSITIVE_INFINITY
				text: root.text
				textStyle: Style.text.subline
				visible: text !== ""
			}
			GText {
				id: descriptionText

				Accessible.ignored: true
				Layout.maximumWidth: Number.POSITIVE_INFINITY
				visible: text !== ""
			}
		}
		ToggleBox {
			Layout.alignment: Qt.AlignTop
		}
	}

	Accessible.onPressAction: if (enabled)
		toggle()
	Accessible.onScrollDownAction: Utils.scrollPageDownOnGFlickable(this)
	Accessible.onScrollUpAction: Utils.scrollPageUpOnGFlickable(this)
	onFocusChanged: if (focus)
		Utils.positionViewAtItem(this)

	HoverHandler {
		id: hoverHandler

	}
	StatefulColors {
		id: colors

		controlStyle: Style.color.controlSwitch
		hoveredCondition: hoverHandler.hovered
		statefulControl: root
	}
	FocusFrame {
		anchors {
			bottomMargin: root.bottomPadding / 2
			leftMargin: root.leftPadding / 2
			rightMargin: root.rightPadding / 2
			topMargin: root.topPadding / 2
		}
	}

	component ToggleBox: Rectangle {
		border.color: colors.controlBorder
		color: colors.controlBackground
		implicitHeight: implicitWidth / 2
		implicitWidth: Style.dimens.switch_width
		radius: height / 2

		TintableIcon {
			source: "qrc:///images/material_check.svg"
			sourceSize.height: parent.height * 0.65
			tintColor: ball.color
			visible: UiPluginModel.a11yOnOffSwitchLabelActive && root.checked

			anchors {
				left: parent.left
				leftMargin: 2 * ball.distanceBallBorder
				verticalCenter: parent.verticalCenter
			}
		}
		TintableIcon {
			source: "qrc:///images/material_close.svg"
			sourceSize.height: parent.height * 0.4
			tintColor: ball.color
			visible: UiPluginModel.a11yOnOffSwitchLabelActive && !root.checked

			anchors {
				right: parent.right
				rightMargin: 3 * ball.distanceBallBorder
				verticalCenter: parent.verticalCenter
			}
		}
		Rectangle {
			id: ball

			readonly property int distanceBallBorder: 3

			anchors.verticalCenter: parent.verticalCenter
			color: colors.controlContent
			height: parent.height - 2 * distanceBallBorder
			radius: height / 2
			width: height
			x: root.checked ? parent.width - width - distanceBallBorder : distanceBallBorder

			Behavior on x {
				enabled: hoverHandler.hovered

				NumberAnimation {
					duration: 200
					easing.type: Easing.InOutQuad
				}
			}
		}
	}
}
