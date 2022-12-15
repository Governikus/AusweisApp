/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.ProgressView 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Workflow 1.0
import Governikus.Type.CheckIDCardModel 1.0
import Governikus.Type.ApplicationModel 1.0

SectionPage {
	id: root
	signal cancel
	signal restartCheck
	signal startAuth

	//: LABEL ANDROID IOS
	title: qsTr("Check device and ID card")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: root.cancel()
	}

	Component.onCompleted: {
		checkIDCardModel.startScan();
	}

	CheckIDCardModel {
		id: checkIDCardModel
		onFireScanCompleted: timerHelper.start()
	}
	Timer {
		id: timerHelper
		interval: 1

		onTriggered: push(checkIDCardResultView, {
				"result": checkIDCardModel.result
			})
	}
	Component {
		id: checkIDCardResultView
		CheckIDCardResultView {
			onCancelClicked: root.cancel()
			onRestartCheck: root.restartCheck()
			onStartAuth: root.startAuth()
		}
	}
	NfcWorkflow {
		anchors.fill: parent
		visible: checkIDCardModel.result < CheckIDCardModel.ID_CARD_DETECTED

		onStartScanIfNecessary: checkIDCardModel.startScanIfNecessary()
	}
	ProgressView {
		anchors.fill: parent
		//: LABEL ANDROID IOS
		subText: qsTr("Please do not move the ID card.")

		//: LABEL ANDROID IOS
		text: qsTr("Checking ID card")
		visible: checkIDCardModel.result >= CheckIDCardModel.ID_CARD_DETECTED
	}
}
