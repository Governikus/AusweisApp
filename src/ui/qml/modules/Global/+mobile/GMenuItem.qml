/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View

AbstractButton {
	id: root

	property alias description: descriptionText.text
	property bool drawBottomCorners: false
	property bool drawTopCorners: false
	property alias tintIcon: iconItem.tintEnabled
	property alias title: titleText.text

	Accessible.name: title + ". " + description
	Accessible.role: Accessible.Button
	icon.source: "qrc:///images/material_arrow_right.svg"
	padding: Constants.pane_padding

	background: RoundedRectangle {
		bottomLeftCorner: root.drawBottomCorners
		bottomRightCorner: root.drawBottomCorners
		color: colors.paneBackground
		topLeftCorner: root.drawTopCorners
		topRightCorner: root.drawTopCorners

		FocusFrame {
			anchors.margins: Constants.component_spacing / 2
			scope: root
		}
	}
	contentItem: RowLayout {
		id: layout

		anchors.margins: Constants.pane_padding
		spacing: 0

		ColumnLayout {
			Layout.maximumWidth: Number.POSITIVE_INFINITY

			GText {
				id: titleText

				Accessible.ignored: true
				textStyle: Style.text.subline
				visible: text !== ""
			}
			GText {
				id: descriptionText

				Accessible.ignored: true
				visible: text !== ""
			}
		}
		TintableIcon {
			id: iconItem

			Accessible.ignored: true
			Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
			source: root.icon.source
			sourceSize.height: Style.dimens.small_icon_size
		}
	}

	StatefulColors {
		id: colors

		statefulControl: root
	}
}
