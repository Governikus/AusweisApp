/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0

MouseArea {
	id: root

	property alias description: descriptionText.text
	property bool highlightTitle: false
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
				Layout.fillWidth: true
				elide: Text.ElideRight
				maximumLineCount: 1
				textStyle: root.highlightTitle ? Style.text.normal_accent_highlight : Style.text.normal_accent
			}
			GText {
				id: descriptionText
				Accessible.ignored: true
				Layout.fillWidth: true
				elide: Text.ElideRight
				maximumLineCount: 1
				textStyle: Style.text.hint_secondary
				visible: text !== ""
			}
		}
		LinkQuality {
			id: linkQualityItem
		}
	}
}
