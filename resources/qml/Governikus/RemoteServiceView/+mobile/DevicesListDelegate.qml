/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0

MouseArea {
	property alias description: descriptionText.text
	property alias linkInactive: linkQualityItem.inactive
	property alias linkQuality: linkQualityItem.percent
	property alias title: titleText.text

	Accessible.name: qsTr("Device %1. %2.").arg(title).arg(description)
	Accessible.role: Accessible.ListItem
	height: content.implicitHeight
	width: content.implicitWidth

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
				Layout.fillWidth: true
				elide: Text.ElideRight
				maximumLineCount: 1
				textStyle: Style.text.normal_accent
			}
			GText {
				id: descriptionText
				Accessible.ignored: true
				Layout.fillWidth: true
				elide: Text.ElideRight
				maximumLineCount: 1
				textStyle: Style.text.hint_secondary
			}
		}
		LinkQuality {
			id: linkQualityItem
		}
	}
}
