/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.View 1.0


Item {
	id: baseItem

	property alias contactModel: repeater.model

	height: columnLayout.height

	Accessible.name: qsTr("Provider contact information")
	Accessible.description: qsTr("Contact information of the selected provider.")
	Accessible.role: Accessible.Grouping

	ColumnLayout {
		id: columnLayout

		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top

		spacing: Constants.text_spacing

		GText {
			Layout.bottomMargin: Constants.groupbox_spacing

			//: LABEL DESKTOP_QML
			text: qsTr("Contact")
			textStyle: Style.text.title_inverse

			FocusFrame {
				scope: baseItem
			}
		}

		Repeater {
			id: repeater

			ColumnLayout {
				Layout.fillWidth: true

				spacing: columnLayout.spacing

				GSeparator {
					visible: index !== 0
					Layout.fillWidth: true
				}

				ProviderContactInfoItem {
					id: contactItem

					Layout.fillWidth: true

					imageSource: Qt.resolvedUrl(model.iconSource)
					//: LABEL DESKTOP_QML
					itemText: (!!model.text ? model.text : qsTr("Unknown"))
					//: LABEL DESKTOP_QML
					accessibleText: (!!model.accessibleText ? model.accessibleText : qsTr("Unknown"))
					label: model.label
					link: model.link
				}
			}
		}
	}
}
