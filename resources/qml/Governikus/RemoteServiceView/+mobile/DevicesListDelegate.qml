/*
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0

MouseArea {
	property alias title: titleText.text
	property alias description: descriptionText.text
	property alias linkQuality: linkQualityItem.percent
	property alias linkInactive: linkQualityItem.inactive

	height: content.implicitHeight
	width: content.implicitWidth

	Accessible.role: Accessible.ListItem
	Accessible.name: qsTr("Device %1").arg(title)
	Accessible.description: description
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

				Layout.fillWidth: true

				elide: Text.ElideRight
				maximumLineCount: 1
				textStyle: Style.text.normal_accent
			}

			GText {
				id: descriptionText

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
