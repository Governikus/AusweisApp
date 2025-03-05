/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.MultiInfoView
import Governikus.ResultView
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type

ResultErrorView {
	id: root

	animationSymbol: PersonalizationModel.error ? Symbol.Type.ERROR : Symbol.Type.CHECK
	animationType: AnimationLoader.Type.STATUS
	buttonText: PersonalizationModel.error ?
	//: LABEL ANDROID IOS
	qsTr("Back to start page") :
	//: LABEL ANDROID IOS
	qsTr("Continue")
	errorCode: PersonalizationModel.error ? PersonalizationModel.statusCodeString : ""
	errorDescription: PersonalizationModel.error ? PersonalizationModel.errorText : ""
	//: LABEL ANDROID IOS
	header: qsTr("Personalization failed")
	subheader: PersonalizationModel.error ? PersonalizationModel.errorHeader : ""
	//: INFO ANDROID IOS Success message that the Smart-eID was created.
	text: PersonalizationModel.error ? PersonalizationModel.resultString : qsTr("You have successfully set up your Smart-eID.")

	//: LABEL ANDROID IOS
	title: qsTr("Set up Smart-eID")

	ColumnLayout {
		spacing: Style.dimens.pane_spacing
		visible: !PersonalizationModel.error
		width: parent.width

		GText {
			//: INFO ANDROID IOS Explanation text of the Smart-eID blocking code
			text: qsTr("Please write down your blocking code:")
		}
		GText {
			Layout.alignment: Qt.AlignHCenter
			horizontalAlignment: Text.AlignHCenter
			text: PersonalizationModel.blockingCode
			textStyle: Style.text.headline
			visible: text !== ""
		}
		GText {
			//: LABEL ANDROID IOS
			text: qsTr("You will shortly receive the blocking password and further information about your Smart-eID by letter.")
			visible: PersonalizationModel.blockingCode !== ""
		}
		GText {
			font.weight: Font.Bold
			//: INFO ANDROID IOS Placeholder (error) text if the Smart-eID setup finished successfully but for some reason no blocking code was retrieved
			text: qsTr("The Smart-eID setup finished successfully but no blocking code was retrieved. For security reasons, you should delete your Smart-eID and restart the setup.")
			visible: PersonalizationModel.blockingCode === ""
		}
		MoreInformationLink {
			Layout.alignment: Qt.AlignHCenter
			text: infoData.linkText

			onClicked: root.push(multiInfoView)
		}
		GText {
			text: {
				switch (PersonalizationModel.remainingAttempts) {
				case 0:
					return PersonalizationModel.blockingPeriodMessage;
				case 1:
					//: LABEL ANDROID IOS
					return qsTr("Attention: you may only set up <b><u>one</u></b> more Smart-eID with your ID card. Further setups may be carried out on %1.").arg(PersonalizationModel.restrictionDate);
				default:
					return "";
				}
			}
			visible: text !== ""
		}
	}
	MultiInfoData {
		id: infoData

		contentType: MultiInfoData.Type.SMART_BLOCKING_CODE
	}
	Component {
		id: multiInfoView

		MultiInfoView {
			infoContent: infoData
			smartEidUsed: root.smartEidUsed

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back

				onClicked: root.pop()
			}
		}
	}
}
