/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.TitleBar
import Governikus.Type

FlickableSectionPage {
	id: root

	signal rightsAccepted

	spacing: Style.dimens.pane_spacing

	ProviderInfoButton {
		Layout.fillWidth: true
		name: CertificateDescriptionModel.subjectName

		onClicked: root.push(certificateDescriptionView)

		Component {
			id: certificateDescriptionView

			CertificateDescriptionView {
				title: root.title

				titleBarSettings: TitleBarSettings {
					navigationAction: NavigationAction.Action.Back

					onNavigationActionClicked: root.pop()
				}
			}
		}
	}
	TransactionInfoButton {
		Layout.fillWidth: true
		showDataNotRequiredText: !requestedRights.visible
		transactionText: AuthModel.transactionInfo
		visible: transactionText !== "" || showDataNotRequiredText

		onClicked: root.push(transactionInfoViewComponent)
		onFocusChanged: if (focus)
			root.positionViewAtItem(this)

		Component {
			id: transactionInfoViewComponent

			TransactionInfoView {
				title: root.title
				transactionText: AuthModel.transactionInfo

				titleBarSettings: TitleBarSettings {
					navigationAction: NavigationAction.Action.Back

					onNavigationActionClicked: root.pop()
				}
			}
		}
	}
	GText {
		Layout.alignment: Qt.AlignLeft
		text: NumberModel.isCanAllowedMode ?
		//: DESKTOP
		qsTr("By entering the CAN, access to the following data of the ID card will be allowed to the mentioned provider:") :
		//: DESKTOP
		qsTr("By entering your PIN, access to the following data of your ID card will be allowed to the mentioned provider:")
	}
	GButton {
		Layout.alignment: Style.scanPatternAlignment
		icon.source: "qrc:/images/identify.svg"
		//: DESKTOP %1 can be "CAN" or "PIN"
		text: qsTr("Proceed to %1 entry").arg((NumberModel.isCanAllowedMode ?
			//: DESKTOP Inserted into "Proceed to %1 entry"
			qsTr("CAN") :
			//: DESKTOP Inserted into "Proceed to %1 entry"
			qsTr("PIN")))
		tintIcon: true

		onClicked: root.rightsAccepted()
	}
	RequestedRights {
		id: requestedRights

		Layout.fillWidth: true
	}
}
