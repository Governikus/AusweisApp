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
	sectionPageFlickable: smartMainView

	//: LABEL ANDROID IOS
	title: qsTr("Smart-eID")
	titleBarColor: Style.color.accent_smart

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: show(UiModule.DEFAULT)
	}

	PersonalizationController {
		id: controller
	}
	Component {
		id: setupStartView
		SmartSetupStartView {
		}
	}
	Component {
		id: updateStartView
		SmartUpdateStartView {
		}
	}
	Component {
		id: deleteProgressView
		ProgressView {
			progressValue: SmartModel.progress
			//: LABEL ANDROID IOS
			subText: qsTr("Please wait a moment.")
			title: qsTr("Smart-eID")
			titleBarColor: Style.color.accent_smart
		}
	}
	Component {
		id: deletePersonalizationStartview
		SmartDeleteStartView {
			//: LABEL ANDROID IOS
			buttonText: qsTr("Delete Smart-eID")

			//: LABEL ANDROID IOS
			deleteDescriptionText: qsTr("You are about to delete the Smart-eID data that is currently stored on your device.")

			onDeleteConfirmed: {
				setLockedAndHidden();
				//: LABEL ANDROID IOS
				push(deleteProgressView, {
						"text": qsTr("Deleting Smart-eID")
					});
				SmartModel.deletePersonalization();
			}

			Connections {
				function onFireDeletePersonalizationDone() {
					setLockedAndHidden(false);
					popAll();
				}

				target: SmartModel
			}
		}
	}
	Component {
		id: deleteStartview
		SmartDeleteStartView {
			//: LABEL ANDROID IOS
			buttonText: qsTr("Reset Smart-eID")

			//: LABEL ANDROID IOS
			deleteDescriptionText: qsTr("You are about to delete the Smart-eID data from your device and also remove the Smart-eID provisioning. This can a be used for troubleshooting as well.")

			onDeleteConfirmed: {
				setLockedAndHidden();
				push(deleteProgressView, {
						"text": qsTr("Resetting Smart-eID"),
						"progressBarVisible": true,
						"progressText": qsTr("Resetting Smart-eID")
					});
				SmartModel.deleteSmart();
			}

			Connections {
				function onFireDeleteSmartDone() {
					setLockedAndHidden(false);
					popAll();
				}

				target: SmartModel
			}
		}
	}
	Component {
		id: checkSmartView
		CheckSmartView {
			onCheckDevice: {
				show(UiModule.CHECK_ID_CARD);
				popAll();
			}
			onRunSmartSetup: push(setupStartView)
			onStartAuth: {
				show(UiModule.SELF_AUTHENTICATION);
				popAll();
			}
		}
	}
	SmartMainView {
		id: smartMainView
		anchors.fill: parent

		onCheckSmart: push(checkSmartView)
		onDeletePersonalization: push(deletePersonalizationStartview)
		onDeleteSmart: push(deleteStartview)
		onSetupSmart: push(setupStartView)
		onUpdateSmart: push(updateStartView)
	}
}
