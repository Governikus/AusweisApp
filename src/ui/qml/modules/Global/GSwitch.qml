/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Governikus.Style
import Governikus.View

Switch {
	id: control

	property alias description: descriptionText.text
	property bool drawBottomCorners: false
	property bool drawTopCorners: false

	Accessible.name: text + ". " + description
	horizontalPadding: Constants.component_spacing
	indicator: null
	verticalPadding: Constants.component_spacing

	background: RoundedRectangle {
		bottomLeftCorner: control.drawBottomCorners
		bottomRightCorner: control.drawBottomCorners
		color: colors.controlPreferredPaneBackground
		topLeftCorner: control.drawTopCorners
		topRightCorner: control.drawTopCorners
	}
	contentItem: RowLayout {
		readonly property int focusWidth: layoutDirection === Qt.LeftToRight ? width : implicitWidth

		layoutDirection: Constants.is_desktop ? Qt.RightToLeft : Qt.LeftToRight
		spacing: Constants.component_spacing

		ColumnLayout {
			spacing: Constants.subtext_spacing
			visible: titleText.text !== "" || descriptionText.text !== ""

			GText {
				id: titleText

				Accessible.ignored: true
				Layout.maximumWidth: Number.POSITIVE_INFINITY
				text: control.text
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
		Rectangle {
			border.color: colors.controlBorder
			color: colors.controlBackground
			implicitHeight: implicitWidth / 2
			implicitWidth: Style.dimens.switch_width
			radius: height / 2

			Rectangle {
				id: ball

				readonly property int distanceBallBorder: 3

				anchors.verticalCenter: parent.verticalCenter
				color: colors.controlContent
				height: parent.height - 2 * distanceBallBorder
				radius: height / 2
				width: height
				x: control.checked ? parent.width - width - distanceBallBorder : distanceBallBorder

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

	Accessible.onPressAction: toggle()

	HoverHandler {
		id: hoverHandler

	}
	StatefulColors {
		id: colors

		controlStyle: Style.color.controlSwitch
		hoveredCondition: hoverHandler.hovered
		statefulControl: control
	}
	FocusFrame {
		anchors {
			bottomMargin: control.bottomPadding / 2
			leftMargin: control.leftPadding / 2
			rightMargin: Math.max(0, contentItem.width - contentItem.focusWidth) + control.rightPadding / 2
			topMargin: control.topPadding / 2
		}
	}
}
