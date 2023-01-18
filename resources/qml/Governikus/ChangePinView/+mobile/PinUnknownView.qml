/*
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.ResultView 1.0
import Governikus.Type.PinResetInformationModel 1.0
import Governikus.Type.UiModule 1.0
import Governikus.Type.WorkflowModel 1.0

ResultView {
	signal close

	resultType: ResultView.Type.IsInfo

	//: LABEL ANDROID IOS
	title: qsTr("No PIN known")

	onCancelClicked: close()
	onContinueClicked: close()

	ColumnLayout {
		spacing: Constants.pane_spacing
		width: parent.width

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
			icon.source: "qrc:///images/material_open_in_new.svg"
			text: PinResetInformationModel.pinResetActionText
			visible: text !== ""

			onClicked: Qt.openUrlExternally(PinResetInformationModel.pinResetUrl)
		}
		PaneTitle {
			Layout.fillWidth: true
			Layout.topMargin: Constants.pane_spacing

			//: LABEL ANDROID IOS
			text: qsTr("Smart-eID")
			visible: WorkflowModel.isSmartSupported
		}
		GText {
			Layout.fillWidth: true

			//: LABEL ANDROID IOS A user wants to change but does not know their Smart-eID PIN.
			text: qsTr("If you have forgotten your Smart-eID PIN, you can easily set up Smart-eID again. This requires knowledge of your ID card PIN.")
			visible: WorkflowModel.isSmartSupported
		}
		GButton {
			Layout.alignment: Qt.AlignRight

			//: LABEL ANDROID IOS
			text: qsTr("Setup Smart-eID")
			visible: WorkflowModel.isSmartSupported

			onClicked: {
				popAll();
				show(UiModule.SMART);
			}
		}
	}
}
