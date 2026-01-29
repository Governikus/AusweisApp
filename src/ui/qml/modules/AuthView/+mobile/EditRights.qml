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

	property alias actionText: actionText.text
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

	GText {
		id: actionText

		//: MOBILE
		text: qsTr("You are about to identify yourself towards the following provider:")
	}
	ProviderInfo {
		Layout.fillWidth: true
		name: CertificateDescriptionModel.subjectName

		onClicked: root.push(certificateDescriptionPage)

		Component {
			id: certificateDescriptionPage

			CertificateDescriptionPage {
				title: root.title
			}
		}
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
	GText {
		id: dataPasswordText

		horizontalAlignment: Text.AlignHCenter
		text: NumberModel.isCanAllowedMode ?
		//: MOBILE
		qsTr("By entering the CAN, access to the following data of the ID card will be allowed to the mentioned provider:") :
		//: MOBILE
		qsTr("By entering your PIN, access to the following data of your ID card will be allowed to the mentioned provider:")
	}
	GPane {
		Accessible.ignored: true
		Layout.fillWidth: true
		color: Style.color.paneSublevel.background.basic_unchecked
		drawShadow: false
		//: MOBILE
		title: qsTr("Transactional information")
		visible: !!root.workflowModel.transactionInfo || (!writeData.visible && !readData.visible)

		GText {
			objectName: "transactionText"
			text: root.workflowModel.transactionInfo
			textFormat: Text.StyledText
			visible: !!text
		}
		GText {
			//: MOBILE
			text: qsTr("The provider mentioned above does not require any data stored on your ID card, only confirmation of you possessing a valid ID card.")
			visible: !writeData.visible && !readData.visible
		}
	}
	GPane {
		Layout.fillWidth: true
		border.color: Style.color.warning
		border.width: Style.dimens.pane_border_highlight_width
		color: Style.color.paneSublevel.background.basic_unchecked
		drawShadow: false
		visible: writeData.count > 0

		DataGroup {
			id: writeData

			Layout.fillWidth: true
			chat: ChatModel.write

			//: MOBILE
			title: qsTr("Write access (update)")
			titleStyle: Style.text.headline
			writeAccess: true

			onScrollPageDown: root.scrollPageDown()
			onScrollPageUp: root.scrollPageUp()
		}
	}
	GPane {
		id: readData

		Layout.fillWidth: true
		color: Style.color.paneSublevel.background.basic_unchecked
		contentPadding: 0
		drawShadow: false
		visible: requiredData.count > 0 || optionalData.count > 0

		DataGroup {
			id: requiredData

			Layout.fillWidth: true
			chat: ChatModel.required

			//: MOBILE
			title: qsTr("Read access")

			onScrollPageDown: root.scrollPageDown()
			onScrollPageUp: root.scrollPageUp()
		}
		DataGroup {
			id: optionalData

			Layout.fillWidth: true
			chat: ChatModel.optional

			//: MOBILE
			title: qsTr("Read access (optional)")

			onScrollPageDown: root.scrollPageDown()
			onScrollPageUp: root.scrollPageUp()
		}
	}
}
