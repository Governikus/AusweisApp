/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.UiModule 1.0
import Governikus.Type.WorkflowModel 1.0


SectionPage {
	id: sectionPage

	//: LABEL ANDROID IOS
	title: qsTr("Start page")

	GridLayout {
		anchors {
			fill: parent
			margins: Constants.pane_padding
		}

		flow: GridLayout.TopToBottom
		columnSpacing: Constants.component_spacing
		rowSpacing: Constants.component_spacing
		rows: Constants.is_tablet ? 2 : 4

		Tile {
			Layout.fillHeight: true
			Layout.fillWidth: true
			Layout.alignment: Qt.AlignHCenter

			//: LABEL ANDROID IOS
			title: qsTr("Check device and ID card")
			image: "qrc:///images/mobile/device.svg"

			onClicked: show(UiModule.CHECK_ID_CARD)
		}

		Tile {
			Layout.fillHeight: true
			Layout.fillWidth: true
			Layout.alignment: Qt.AlignHCenter

			//: LABEL ANDROID IOS
			title: qsTr("Change my (Transport) PIN")
			image: "qrc:///images/material_lock.svg"

			onClicked: show(UiModule.PINMANAGEMENT)
		}

		Tile {
			Layout.fillHeight: true
			Layout.fillWidth: true
			Layout.alignment: Qt.AlignHCenter

			//: LABEL ANDROID IOS
			title: qsTr("See my personal data")
			image: "qrc:///images/mydata.svg"

			onClicked: show(UiModule.SELF_AUTHENTICATION)
		}

		Tile {
			visible: WorkflowModel.isSmartSupported
			Layout.fillHeight: true
			Layout.fillWidth: true
			Layout.alignment: Qt.AlignHCenter

			//: LABEL ANDROID IOS
			title: qsTr("Smart-eID")
			image: "qrc:///images/identify.svg"

			onClicked: show(UiModule.SMART)
		}

		Tile {
			visible: !WorkflowModel.isSmartSupported
			Layout.fillHeight: true
			Layout.fillWidth: true
			Layout.alignment: Qt.AlignHCenter

			//: LABEL ANDROID IOS
			title: qsTr("Provider")
			image: "qrc:///images/provider.svg"

			onClicked: show(UiModule.PROVIDER)
		}
	}
}
