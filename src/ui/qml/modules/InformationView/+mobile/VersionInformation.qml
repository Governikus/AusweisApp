/**
 * Copyright (c) 2016-2026 Governikus Service GmbH, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Global
import Governikus.TitleBar
import Governikus.View
import Governikus.Style
import Governikus.Type

SectionPage {
	id: root

	property var versionInformationModel: VersionInformationModel

	contentIsScrolled: !listView.atYBeginning

	//: MOBILE %1 is replaced with the application name
	title: qsTr("%1 version").arg(Qt.application.name)

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.pop()
	}

	onActiveFocusChanged: if (activeFocus) {
		listView.highlightScrollbar();
		if (!ApplicationModel.screenReaderRunning)
			listView.forceActiveFocus(Qt.MouseFocusReason);
	}

	GListView {
		id: listView

		displayMarginBeginning: Style.dimens.pane_padding
		displayMarginEnd: Style.dimens.pane_padding
		keyNavigationChangesCurrentIndex: false
		model: root.versionInformationModel
		spacing: Style.dimens.pane_spacing

		delegate: LabeledText {
			id: delegateItem

			required property int index
			required property string key
			required property string value

			anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
			anchors.horizontalCenterOffset: -Style.dimens.pane_padding / 2
			focusFrameVisible: false
			label: key
			text: value
			width: Math.min(listView.width - Style.dimens.pane_padding, Style.dimens.max_text_width)

			Accessible.onScrollDownAction: listView.scrollPageDown()
			Accessible.onScrollUpAction: listView.scrollPageUp()
			onActiveFocusChanged: if (activeFocus) {
				listView.centerViewAtIndex(delegateItem.index);
			}
		}

		onActiveFocusChanged: if (activeFocus)
			highlightScrollbar()

		anchors {
			bottomMargin: Style.dimens.pane_padding
			fill: parent
			leftMargin: Style.dimens.pane_padding
			topMargin: Style.dimens.pane_padding
		}
		TapHandler {
			property int advancedSettingsCounter: 0

			onTapped: {
				advancedSettingsCounter += 1;
				switch (advancedSettingsCounter) {
				case 7:
				case 8:
				case 9:
					if (!ApplicationModel.screenReaderRunning) {
						//: MOBILE Used in notifications when the user taps the version information
						ApplicationModel.showFeedback(qsTr("%1 more taps to toggle the advanced settings.").arg(10 - advancedSettingsCounter), true);
					}
					break;
				case 10:
					SettingsModel.advancedSettings = !SettingsModel.advancedSettings;
					ApplicationModel.showFeedback((SettingsModel.advancedSettings ?
						//: MOBILE
						qsTr("Advanced settings activated.") :
						//: MOBILE
						qsTr("Advanced settings deactivated.")), true);
					advancedSettingsCounter = 0;
					break;
				}
			}
		}
	}
}
