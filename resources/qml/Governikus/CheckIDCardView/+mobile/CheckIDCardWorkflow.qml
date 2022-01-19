/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.ProgressView 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Workflow 1.0
import Governikus.Type.CheckIDCardModel 1.0
import Governikus.Type.ApplicationModel 1.0

SectionPage {
	id: root

	signal restartCheck()

	//: LABEL ANDROID IOS
	Accessible.name: qsTr("Check device and ID card")
	//: LABEL ANDROID IOS
	Accessible.description: qsTr("This is the device and ID card check of the AusweisApp2.")

	navigationAction: NavigationAction {
		state: "cancel"
		onClicked: {
			navBar.lockedAndHidden = false
			firePopAll()
		}
	}

	//: LABEL ANDROID IOS
	title: qsTr("Check device and ID card")

	Component.onCompleted: {
		navBar.lockedAndHidden = true

		checkIDCardModel.startScan()
	}

	CheckIDCardModel {
		id: checkIDCardModel

		onFireScanCompleted: timerHelper.start()
	}

	Timer {
		id: timerHelper
		interval: 1
		onTriggered: firePushWithProperties(checkIDCardResultView, { result: checkIDCardModel.result })
	}

	Component {
		id: checkIDCardResultView

		CheckIDCardResultView {
			onRestartCheck: root.restartCheck()
		}
	}

	NfcWorkflow {
		visible: checkIDCardModel.result < CheckIDCardModel.ID_CARD_DETECTED
		anchors.fill: parent

		onStartScanIfNecessary: checkIDCardModel.startScanIfNecessary()
	}

	ProgressView {
		visible: checkIDCardModel.result >= CheckIDCardModel.ID_CARD_DETECTED
		anchors.fill: parent

		text: qsTr("Checking ID card")
		subText: qsTr("Please do not move the ID card.")
	}
}
