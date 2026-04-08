/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type

FlickableSectionPage {
	id: root

	property alias dataText: dataPasswordText.text
	property var workflowModel: AuthModel

	signal rightsAccepted

	spacing: Style.dimens.pane_spacing

	//: MOBILE
	title: qsTr("Identify")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Cancel

		onClicked: root.workflowModel.cancelWorkflow()
	}

	ProviderInfoButton {
		Layout.fillWidth: true
		name: CertificateDescriptionModel.subjectName

		onClicked: root.push(certificateDescriptionView)

		Component {
			id: certificateDescriptionView

			CertificateDescriptionView {
				title: root.title

				navigationAction: NavigationAction {
					action: NavigationAction.Action.Back

					onClicked: root.pop()
				}
			}
		}
	}
	TransactionInfoButton {
		Layout.fillWidth: true
		showDataNotRequiredText: !requestedRights.visible
		transactionText: root.workflowModel.transactionInfo
		visible: transactionText !== "" || showDataNotRequiredText

		onClicked: root.push(transactionInfoViewComponent)

		Component {
			id: transactionInfoViewComponent

			TransactionInfoView {
				title: root.title
				transactionText: root.workflowModel.transactionInfo

				navigationAction: NavigationAction {
					action: NavigationAction.Action.Back

					onClicked: root.pop()
				}
			}
		}
	}
	GText {
		id: dataPasswordText

		text: NumberModel.isCanAllowedMode ?
		//: MOBILE
		qsTr("By entering the CAN, access to the following data of the ID card will be allowed to the mentioned provider:") :
		//: MOBILE
		qsTr("By entering your PIN, access to the following data of your ID card will be allowed to the mentioned provider:")
	}
	GButton {
		Layout.alignment: Qt.AlignHCenter
		icon.source: "qrc:///images/identify.svg"
		//: MOBILE %1 can be "CAN" or "PIN"
		text: qsTr("Proceed to %1 entry").arg((NumberModel.isCanAllowedMode ?
			//: IOS Inserted into "Proceed to %1 entry"
			qsTr("CAN") :
			//: IOS Inserted into "Proceed to %1 entry"
			qsTr("PIN")))
		tintIcon: true

		onClicked: root.rightsAccepted()
	}
	RequestedRights {
		id: requestedRights

		Layout.fillWidth: true
	}
}
