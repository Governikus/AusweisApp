/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style

MouseArea {
	id: root

	property alias description: descriptionText.text
	property alias highlightTitle: titleText.font.bold
	property alias linkInactive: linkQualityItem.inactive
	property alias linkQuality: linkQualityItem.percent
	property alias linkQualityVisible: linkQualityItem.visible
	property alias title: titleText.text

	Accessible.name: qsTr("Device %1. %2.").arg(title).arg(description)
	Accessible.role: Accessible.ListItem
	implicitHeight: content.implicitHeight
	implicitWidth: content.implicitWidth

	Accessible.onPressAction: clicked(null)

	RowLayout {
		id: content

		anchors.fill: parent
		spacing: Constants.groupbox_spacing

		ColumnLayout {
			Layout.fillWidth: true
			spacing: 2

			GText {
				id: titleText

				Accessible.ignored: true
				elide: Text.ElideRight
				maximumLineCount: 1
				textStyle: Style.text.subline
			}
			GText {
				id: descriptionText

				Accessible.ignored: true
				elide: Text.ElideRight
				maximumLineCount: 1
				visible: text !== ""
			}
		}
		GSpacer {
			Layout.fillWidth: true
		}
		LinkQuality {
			id: linkQualityItem

		}
	}
}
