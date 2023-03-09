/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
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
		columnSpacing: Constants.component_spacing
		flow: GridLayout.TopToBottom
		rowSpacing: Constants.component_spacing
		rows: Constants.is_tablet ? 2 : 4

		anchors {
			fill: parent
			margins: Constants.pane_padding
		}
		Tile {
			Layout.alignment: Qt.AlignHCenter
			Layout.fillHeight: true
			Layout.fillWidth: true
			image: "qrc:///images/mobile/device.svg"

			//: LABEL ANDROID IOS
			title: qsTr("Check device and ID card")

			onClicked: show(UiModule.CHECK_ID_CARD)
		}
		Tile {
			Layout.alignment: Qt.AlignHCenter
			Layout.fillHeight: true
			Layout.fillWidth: true
			image: "qrc:///images/material_lock.svg"

			//: LABEL ANDROID IOS
			title: qsTr("Change my (Transport) PIN")

			onClicked: show(UiModule.PINMANAGEMENT)
		}
		Tile {
			Layout.alignment: Qt.AlignHCenter
			Layout.fillHeight: true
			Layout.fillWidth: true
			image: "qrc:///images/mydata.svg"

			//: LABEL ANDROID IOS
			title: qsTr("See my personal data")

			onClicked: show(UiModule.SELF_AUTHENTICATION)
		}
		Tile {
			Layout.alignment: Qt.AlignHCenter
			Layout.fillHeight: true
			Layout.fillWidth: true
			image: "qrc:///images/identify.svg"

			//: LABEL ANDROID IOS
			title: qsTr("Smart-eID")
			visible: WorkflowModel.isSmartSupported

			onClicked: show(UiModule.SMART)
		}
		Tile {
			Layout.alignment: Qt.AlignHCenter
			Layout.fillHeight: true
			Layout.fillWidth: true
			image: "qrc:///images/provider.svg"

			//: LABEL ANDROID IOS
			title: qsTr("Provider")
			visible: !WorkflowModel.isSmartSupported

			onClicked: show(UiModule.PROVIDER)
		}
	}
}
