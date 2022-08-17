/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.ResultView 1.0
import Governikus.Type.PinResetInformationModel 1.0
import Governikus.Type.UiModule 1.0
import Governikus.Type.WorkflowModel 1.0


ResultView {
	signal close()

	//: LABEL ANDROID IOS
	title: qsTr("No PIN known")
	resultType: ResultView.Type.IsInfo

	onCancelClicked: close()
	onContinueClicked: close()

	ColumnLayout {
		width: parent.width

		spacing: Constants.pane_spacing

		PaneTitle {
			Layout.fillWidth: true

			//: LABEL ANDROID IOS
			text: qsTr("ID card")
		}

		GText {
			Layout.fillWidth: true

			text: PinResetInformationModel.pinForgottenTutorialHint
		}

		GButton {
			Layout.alignment: Qt.AlignRight

			visible: text !== ""

			icon.source: "qrc:///images/material_open_in_new.svg"
			text: PinResetInformationModel.pinResetActionText

			onClicked: Qt.openUrlExternally(PinResetInformationModel.pinResetUrl)
		}

		PaneTitle {
			visible: WorkflowModel.isSmartSupported
			Layout.fillWidth: true
			Layout.topMargin: Constants.pane_spacing

			//: LABEL ANDROID IOS
			text: qsTr("Smart-eID")
		}

		GText {
			visible: WorkflowModel.isSmartSupported
			Layout.fillWidth: true

			//: LABEL ANDROID IOS A user wants to change but does not know their Smart-eID PIN.
			text: qsTr("If you have forgotten your Smart-eID PIN, you can easily set up Smart-eID again. This requires knowledge of your ID card PIN.")
		}

		GButton {
			visible: WorkflowModel.isSmartSupported
			Layout.alignment: Qt.AlignRight

			//: LABEL ANDROID IOS
			text: qsTr("Setup Smart-eID")

			onClicked: {
				popAll()
				show(UiModule.SMART)
			}
		}
	}
}
