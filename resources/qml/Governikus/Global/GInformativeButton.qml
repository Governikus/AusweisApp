/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Style 1.0
import Governikus.View 1.0

GButton {
	id: root

	property alias description: subText.text
	property real scaleIcon: 1.0

	Accessible.name: text + ". " + description
	horizontalPadding: Constants.component_spacing
	textStyle: Style.text.button
	verticalPadding: Constants.text_spacing

	contentItem: RowLayout {
		spacing: Constants.component_spacing
		z: 1

		TintableIcon {
			Layout.preferredHeight: Layout.preferredWidth
			Layout.preferredWidth: Style.dimens.icon_size
			fillMode: Image.Pad
			source: root.icon.source
			sourceSize.width: Layout.preferredWidth * scaleIcon
			tintColor: root.textStyle.textColor
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
					Layout.fillWidth: true
					elide: Text.ElideRight
					maximumLineCount: 1
					text: root.text
					textStyle: Style.text.button_highlight
				}
				GText {
					id: subText
					Accessible.ignored: true
					Layout.alignment: Qt.AlignTop
					Layout.fillWidth: true
					elide: Text.ElideRight
					maximumLineCount: 2
					textStyle: root.textStyle
				}
			}
			FocusFrame {
				framee: textColumn
				isOnLightBackground: false
				marginFactor: 0.7
				scope: root
			}
		}
	}
}
