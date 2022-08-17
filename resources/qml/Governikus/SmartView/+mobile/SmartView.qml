/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.CheckSmartView 1.0
import Governikus.Global 1.0
import Governikus.ProgressView 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.SmartModel 1.0
import Governikus.Type.UiModule 1.0
import Governikus.View 1.0

SectionPage {
	id: root

	navigationAction: NavigationAction { action: NavigationAction.Action.Back; onClicked: show(UiModule.DEFAULT) }

	//: LABEL ANDROID IOS
	title: qsTr("Smart-eID")
	titleBarColor: Style.color.accent_smart

	PersonalizationController {
		id: controller
	}

	Component {
		id: setupStartView

		SmartSetupStartView {}
	}

	Component {
		id: updateStartView

		SmartUpdateStartView {}
	}

	Component {
		id: deleteProgressView

		ProgressView {
			titleBarColor: Style.color.accent_smart
			title: qsTr("Smart-eID")
			//: LABEL ANDROID IOS
			subText: qsTr("Please wait a moment.")
			progressValue: SmartModel.progress
		}
	}

	Component {
		id: deletePersonalizationStartview

		SmartDeleteStartView {
			Connections {
				target: SmartModel

				function onFireDeletePersonalizationDone() {
					setLockedAndHidden(false)
					popAll()
				}
			}

			//: LABEL ANDROID IOS
			deleteDescriptionText: qsTr("You are about to delete the Smart-eID data that is currently stored on your device.")
			//: LABEL ANDROID IOS
			buttonText: qsTr("Delete Smart-eID")

			onDeleteConfirmed: {
				setLockedAndHidden()
				//: LABEL ANDROID IOS
				push(deleteProgressView, {text: qsTr("Deleting Smart-eID")})
				SmartModel.deletePersonalization()
			}
		}
	}

	Component {
		id: deleteStartview

		SmartDeleteStartView {
			Connections {
				target: SmartModel

				function onFireDeleteSmartDone() {
					setLockedAndHidden(false)
					popAll()
				}
			}

			//: LABEL ANDROID IOS
			deleteDescriptionText: qsTr("You are about to delete the Smart-eID data from your device and also remove the Smart-eID provisioning. This can a be used for troubleshooting as well.")
			//: LABEL ANDROID IOS
			buttonText: qsTr("Reset Smart-eID")

			onDeleteConfirmed: {
				setLockedAndHidden()
				push(deleteProgressView, {
						 //: LABEL ANDROID IOS
						 text: qsTr("Resetting Smart-eID"),
						 progressBarVisible: true,
						 //: LABEL ANDROID IOS
						 progressText: qsTr("Resetting Smart-eID")
					 })
				SmartModel.deleteSmart()
			}
		}
	}

	Component {
		id: checkSmartView

		CheckSmartView {
			onStartAuth: {
				show(UiModule.SELF_AUTHENTICATION)
				popAll()
			}
			onCheckDevice: {
				show(UiModule.CHECK_ID_CARD)
				popAll()
			}
			onRunSmartSetup: push(setupStartView)
		}
	}

	sectionPageFlickable: smartMainView
	SmartMainView {
		id: smartMainView

		anchors.fill: parent

		onSetupSmart: push(setupStartView)
		onUpdateSmart: push(updateStartView)
		onDeletePersonalization: push(deletePersonalizationStartview)
		onDeleteSmart: push(deleteStartview)
		onCheckSmart: push(checkSmartView)
	}
}
