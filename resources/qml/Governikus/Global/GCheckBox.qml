/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View

CheckBox {
	id: control

	property alias focusFrameVisible: focusFrame.visible
	property alias layoutDirection: contentLayout.layoutDirection
	property alias maximumLineCount: description.maximumLineCount
	property alias overwriteHovered: colors.overwriteHovered
	property alias overwritePressed: colors.overwritePressed
	readonly property color preferredBackgroundColor: colors.control_preferredPaneBackground
	property alias textStyle: description.textStyle

	horizontalPadding: Constants.component_spacing
	indicator: null
	verticalPadding: Constants.component_spacing / 2

	contentItem: RowLayout {
		id: contentLayout

		readonly property int focusWidth: layoutDirection === Qt.RightToLeft ? width : implicitWidth

		spacing: control.spacing

		Rectangle {
			border.color: colors.control_border
			border.width: Style.dimens.border_width
			color: colors.control
			implicitHeight: Style.dimens.small_icon_size
			implicitWidth: implicitHeight
			radius: Math.max(plugin.scaleFactor * 4, 1)

			TintableIcon {
				anchors.fill: parent
				anchors.margins: Math.max(plugin.scaleFactor * 4, 1)
				fillMode: Image.PreserveAspectFit
				source: "qrc:///images/checkbox_indicator.svg"
				tintColor: colors.control_content
				visible: control.checked
			}
		}
		GText {
			id: description

			Accessible.ignored: true
			Layout.maximumWidth: Number.POSITIVE_INFINITY
			elide: Text.ElideRight
			maximumLineCount: 1
			text: control.text
			textStyle: Style.text.normal
			visible: text !== ""
		}
	}

	Accessible.onPressAction: {
		toggle();
		toggled();
	}

	StatefulColors {
		id: colors

		control_content: Style.color.control_content_graphics
		statefulControl: control
	}
	FocusFrame {
		id: focusFrame

		anchors {
			bottomMargin: control.bottomPadding / 2
			leftMargin: control.leftPadding / 2
			rightMargin: Math.max(0, contentItem.width - contentItem.focusWidth) + control.rightPadding / 2
			topMargin: control.topPadding / 2
		}
	}
}
