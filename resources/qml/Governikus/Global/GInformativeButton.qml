/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Style
import Governikus.View

GButton {
	id: root

	property alias description: subText.text
	property real scaleIcon: 1.0

	Accessible.name: text + ". " + description
	Layout.maximumWidth: Number.POSITIVE_INFINITY
	horizontalPadding: Constants.component_spacing
	verticalPadding: Constants.text_spacing

	contentItem: RowLayout {
		readonly property color color: root.pressed ? Style.color.control_content_pressed : root.textStyle.textColor

		spacing: Constants.component_spacing
		z: 1

		TintableIcon {
			Layout.preferredHeight: Layout.preferredWidth
			Layout.preferredWidth: Style.dimens.icon_size
			fillMode: Image.Pad
			source: root.icon.source
			sourceSize.width: Layout.preferredWidth * scaleIcon
			tintColor: contentItem.color
			tintEnabled: root.tintIcon
		}
		Item {
			Layout.fillWidth: true
			implicitHeight: textColumn.implicitHeight
			implicitWidth: textColumn.implicitWidth

			ColumnLayout {
				id: textColumn

				anchors.fill: parent
				spacing: Constants.text_spacing / 2

				GText {
					Accessible.ignored: true
					Layout.alignment: Qt.AlignBottom
					color: contentItem.color
					elide: Text.ElideRight
					font.bold: true
					maximumLineCount: 1
					text: root.text
					textStyle: root.textStyle
				}
				GText {
					id: subText

					Accessible.ignored: true
					Layout.alignment: Qt.AlignTop
					color: contentItem.color
					elide: Text.ElideRight
					maximumLineCount: 2
					textStyle: root.textStyle
				}
			}
		}
	}
}
