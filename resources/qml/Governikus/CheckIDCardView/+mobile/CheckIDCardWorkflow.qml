/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.ProgressView
import Governikus.TitleBar
import Governikus.View
import Governikus.Workflow
import Governikus.Type.CheckIDCardModel
import Governikus.Type.ApplicationModel

SectionPage {
	id: root

	signal cancel
	signal restartCheck
	signal startAuth

	contentIsScrolled: nfcWorkflow.visible && !nfcWorkflow.atYBeginning || progressView.visible && progressView.contentIsScrolled

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
			title: root.title

			onCancelClicked: root.cancel()
			onRestartCheck: root.restartCheck()
			onStartAuth: root.startAuth()
		}
	}
	NfcWorkflow {
		id: nfcWorkflow

		anchors.fill: parent
		visible: checkIDCardModel.result < CheckIDCardModel.ID_CARD_DETECTED

		onStartScanIfNecessary: checkIDCardModel.startScanIfNecessary()
	}
	ProgressView {
		id: progressView

		anchors.fill: parent
		//: LABEL ANDROID IOS
		subText: qsTr("Please do not move the ID card.")

		//: LABEL ANDROID IOS
		text: qsTr("Checking ID card")
		title: root.title
		visible: checkIDCardModel.result >= CheckIDCardModel.ID_CARD_DETECTED
	}
}
