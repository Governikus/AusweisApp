/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.SmartModel 1.0
import Governikus.Type.PersonalizationModel 1.0

Item {
	id: root

	readonly property int smartState: SmartModel.smartState

	signal checkSmart
	signal deletePersonalization
	signal deleteSmart
	signal setupSmart
	signal updateSmart

	implicitHeight: contentLayout.implicitHeight
	implicitWidth: contentLayout.implicitWidth

	ColumnLayout {
		id: contentLayout
		anchors.fill: parent
		spacing: Constants.component_spacing

		TitledSeparator {
			Layout.fillWidth: true
			contentMarginBottom: 0
			contentMarginLeft: 0
			contentMarginRight: 0
			contentMarginTop: 0

			//: LABEL ANDROID IOS
			title: qsTr("Smart-eID")
		}
		MenuItem {
			Layout.fillWidth: true
			contentMarginLeft: 0
			contentMarginRight: 0
			contentMarginVertical: 0
			//: LABEL ANDROID IOS
			description: qsTr("Check device compatibility and the current state of any present Smart-eID")

			//: LABEL ANDROID IOS
			title: qsTr("Check Smart-eID status")
			visible: smartState > SmartModel.SMART_UPDATING_STATUS

			onClicked: checkSmart()
		}
		MenuItem {
			Layout.fillWidth: true
			contentMarginLeft: 0
			contentMarginRight: 0
			contentMarginVertical: 0
			//: LABEL ANDROID IOS
			description: qsTr("Set up Smart-eID on this device")

			//: LABEL ANDROID IOS
			title: qsTr("Set up Smart-eID")
			visible: smartState > SmartModel.SMART_UNAVAILABLE && smartState < SmartModel.SMART_READY

			onClicked: setupSmart()
		}
		MenuItem {
			Layout.fillWidth: true
			contentMarginLeft: 0
			contentMarginRight: 0
			contentMarginVertical: 0
			//: LABEL ANDROID IOS
			description: qsTr("Renew your Smart-eID with current data")

			//: LABEL ANDROID IOS
			title: qsTr("Renew Smart-eID")
			visible: smartState === SmartModel.SMART_READY

			onClicked: updateSmart()
		}
		MenuItem {
			Layout.fillWidth: true
			contentMarginLeft: 0
			contentMarginRight: 0
			contentMarginVertical: 0
			//: LABEL ANDROID IOS
			description: qsTr("Remove Smart-eID data from your device")

			//: LABEL ANDROID IOS
			title: qsTr("Delete Smart-eID")
			visible: smartState === SmartModel.SMART_READY

			onClicked: deletePersonalization()
		}
		MenuItem {
			Layout.fillWidth: true
			contentMarginLeft: 0
			contentMarginRight: 0
			contentMarginVertical: 0
			//: LABEL ANDROID IOS
			description: qsTr("Remove Smart-eID data and provisioning from your device")

			//: LABEL ANDROID IOS
			title: qsTr("Reset Smart-eID")
			visible: smartState > SmartModel.SMART_UPDATING_STATUS

			onClicked: deleteSmart()
		}
	}
}
