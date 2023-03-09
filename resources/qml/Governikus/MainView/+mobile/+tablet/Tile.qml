/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0

GPaneBackground {
	id: root

	property alias image: image.source
	property alias title: text.text

	signal clicked

	Accessible.name: title
	Accessible.role: Accessible.Button
	color: Qt.darker(Style.color.accent, mouseArea.pressed ? Constants.highlightDarkerFactor : 1)

	Accessible.onPressAction: clicked()

	MouseArea {
		id: mouseArea
		anchors.fill: root

		onClicked: root.clicked()
	}
	ColumnLayout {
		anchors.fill: parent
		anchors.margins: Constants.pane_spacing
		spacing: Constants.text_spacing

		GSpacer {
			Layout.fillHeight: true
		}
		TintableIcon {
			id: image
			Accessible.ignored: true
			Layout.alignment: Qt.AlignHCenter
			Layout.fillHeight: true
			Layout.maximumHeight: Style.dimens.huge_icon_size
			Layout.preferredHeight: Style.dimens.huge_icon_size
			sourceSize.height: Style.dimens.huge_icon_size
			tintColor: text.color
		}
		GText {
			id: text
			Accessible.ignored: true
			Layout.fillHeight: true
			Layout.fillWidth: true
			Layout.minimumHeight: lineHeight
			color: Qt.darker(textStyle.textColor, mouseArea.pressed ? Constants.highlightDarkerFactor : 1)
			elide: Text.ElideRight
			horizontalAlignment: Text.AlignHCenter
			textStyle: Style.text.title_inverse_highlight
			verticalAlignment: Text.AlignVCenter
		}
		GSpacer {
			Layout.fillHeight: true
		}
	}
}
