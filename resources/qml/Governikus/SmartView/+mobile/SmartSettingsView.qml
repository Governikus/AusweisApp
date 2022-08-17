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

	signal setupSmart()
	signal updateSmart()
	signal deletePersonalization()
	signal deleteSmart()
	signal checkSmart()

	readonly property int smartState: SmartModel.smartState

	implicitWidth: contentLayout.implicitWidth
	implicitHeight: contentLayout.implicitHeight

	ColumnLayout {
		id: contentLayout

		anchors.fill: parent

		spacing: Constants.component_spacing

		TitledSeparator {
			Layout.fillWidth: true
			contentMarginLeft: 0
			contentMarginRight: 0
			contentMarginBottom: 0
			contentMarginTop: 0

			//: LABEL ANDROID IOS
			title: qsTr("Smart-eID")
		}

		MenuItem {
			Layout.fillWidth: true

			visible: smartState > SmartModel.SMART_UPDATING_STATUS

			contentMarginLeft: 0
			contentMarginRight: 0
			contentMarginVertical: 0

			//: LABEL ANDROID IOS
			title: qsTr("Check Smart-eID status")
			//: LABEL ANDROID IOS
			description: qsTr("Check device compatibility and the current state of any present Smart-eID")

			onClicked: checkSmart()
		}


		MenuItem {
			Layout.fillWidth: true

			visible: smartState > SmartModel.SMART_UNAVAILABLE && smartState < SmartModel.SMART_READY

			contentMarginLeft: 0
			contentMarginRight: 0
			contentMarginVertical: 0

			//: LABEL ANDROID IOS
			title: qsTr("Set up Smart-eID")
			//: LABEL ANDROID IOS
			description: qsTr("Set up Smart-eID on this device")

			onClicked: setupSmart()
		}

		MenuItem {
			Layout.fillWidth: true

			visible: smartState === SmartModel.SMART_READY

			contentMarginLeft: 0
			contentMarginRight: 0
			contentMarginVertical: 0

			//: LABEL ANDROID IOS
			title: qsTr("Renew Smart-eID")
			//: LABEL ANDROID IOS
			description: qsTr("Renew your Smart-eID with current data")

			onClicked: updateSmart()
		}

		MenuItem {
			Layout.fillWidth: true

			visible: smartState === SmartModel.SMART_READY

			contentMarginLeft: 0
			contentMarginRight: 0
			contentMarginVertical: 0

			//: LABEL ANDROID IOS
			title: qsTr("Delete Smart-eID")
			//: LABEL ANDROID IOS
			description: qsTr("Remove Smart-eID data from your device")

			onClicked: deletePersonalization()
		}

		MenuItem {
			Layout.fillWidth: true

			visible: smartState > SmartModel.SMART_UPDATING_STATUS

			contentMarginLeft: 0
			contentMarginRight: 0
			contentMarginVertical: 0

			//: LABEL ANDROID IOS
			title: qsTr("Reset Smart-eID")
			//: LABEL ANDROID IOS
			description: qsTr("Remove Smart-eID data and provisioning from your device")

			onClicked: deleteSmart()
		}
	}
}
