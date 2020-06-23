/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0


SectionPage {
	id: root
	property var historyModelItem

	navigationAction: NavigationAction { state: "back"; onClicked: firePop() }
	title: historyModelItem ? historyModelItem.subject : ""
	titleBarColor: Category.displayColor(historyModelItem ? historyModelItem.providerCategory : "")

	content: Item {
		height: pane.height + 2 * Constants.component_spacing
		width: root.width

		Column {
			anchors.fill: parent
			anchors.margins: Constants.component_spacing

			Pane {
				id: pane
				//: LABEL ANDROID IOS
				title: qsTr("Provider Information") + SettingsModel.translationTrigger

				LabeledText {
					Accessible.onScrollDownAction: scrollPageDown()
					Accessible.onScrollUpAction: scrollPageUp()

					//: LABEL ANDROID IOS
					label: qsTr("Provider name") + SettingsModel.translationTrigger
					text: historyModelItem ? historyModelItem.subject : ""
					width: parent.width
				}

				LabeledText {
					visible: text !== ""

					Accessible.onScrollDownAction: scrollPageDown()
					Accessible.onScrollUpAction: scrollPageUp()

					//: LABEL ANDROID IOS
					label: qsTr("Purpose") + SettingsModel.translationTrigger
					text: historyModelItem ? historyModelItem.purpose : ""
					width: parent.width
				}

				LabeledText {
					Accessible.onScrollDownAction: scrollPageDown()
					Accessible.onScrollUpAction: scrollPageUp()

					//: LABEL ANDROID IOS
					label: qsTr("Date") + SettingsModel.translationTrigger
					text:{
						if (!historyModelItem) {
							return "";
						}
						return historyModelItem.dateTime.toLocaleString(Qt.locale(), qsTr("dd.MM.yyyy")) + SettingsModel.translationTrigger
					}
					width: parent.width
				}

				LabeledText {
					Accessible.onScrollDownAction: scrollPageDown()
					Accessible.onScrollUpAction: scrollPageUp()

					//: LABEL ANDROID IOS
					label: qsTr("Submitted data") + SettingsModel.translationTrigger
					text: historyModelItem ? historyModelItem.requestedData : ""
					width: parent.width
				}

				LabeledText {
					Accessible.onScrollDownAction: scrollPageDown()
					Accessible.onScrollUpAction: scrollPageUp()

					//: LABEL ANDROID IOS
					label: qsTr("Terms of usage") + SettingsModel.translationTrigger
					text: historyModelItem ? historyModelItem.termsOfUsage : ""
					width: parent.width
				}
			}
		}
	}
}
