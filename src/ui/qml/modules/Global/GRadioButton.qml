/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View

RadioButton {
	id: root

	property bool drawBottomCorners: false
	property bool drawTopCorners: false
	readonly property int indicatorHeight: Style.dimens.small_icon_size
	property bool tintIcon: false

	Accessible.name: text
	horizontalPadding: Constants.component_spacing
	indicator: null
	spacing: Constants.groupbox_spacing
	verticalPadding: Constants.component_spacing / 2

	background: RoundedRectangle {
		bottomLeftCorner: root.drawBottomCorners
		bottomRightCorner: root.drawBottomCorners
		color: colors.controlPreferredPaneBackground
		topLeftCorner: root.drawTopCorners
		topRightCorner: root.drawTopCorners
	}
	contentItem: RowLayout {
		spacing: root.spacing

		Rectangle {
			border.color: colors.controlBorder
			border.width: Style.dimens.border_width
			color: colors.controlBackground
			implicitHeight: root.indicatorHeight
			implicitWidth: root.indicatorHeight
			radius: height / 2

			Rectangle {
				anchors.centerIn: parent
				color: colors.controlContent
				height: parent.height / 2.5
				radius: height / 2
				visible: root.checked
				width: height
			}
		}
		TintableIcon {
			Layout.preferredHeight: sourceSize.height
			source: root.icon.source
			sourceSize.height: root.indicatorHeight
			tintColor: description.color
			tintEnabled: tintIcon
			visible: source.toString() !== ""
		}
		GText {
			id: description

			Accessible.ignored: true
			text: root.text
			visible: text !== ""
		}
		GSpacer {
			Layout.fillWidth: true
			visible: root.text !== ""
		}
	}

	HoverHandler {
		id: hoverHandler

	}
	StatefulColors {
		id: colors

		controlStyle: Style.color.controlRadiobutton
		groupMember: true
		hoveredCondition: hoverHandler.hovered
		statefulControl: root
	}
	FocusFrame {
		anchors {
			bottomMargin: root.bottomPadding / 2
			leftMargin: root.leftPadding / 2
			rightMargin: Math.max(0, contentItem.width - contentItem.implicitWidth) + root.rightPadding / 2
			topMargin: root.topPadding / 2
		}
	}
}
