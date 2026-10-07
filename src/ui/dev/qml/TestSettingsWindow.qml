/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtCore

import Governikus.Global
import Governikus.Init
import Governikus.Style
import Governikus.Type
import Governikus.View

ApplicationWindow {
	id: root

	property App app: null
	property list<string> testFiles

	flags: Qt.Tool | Qt.CustomizeWindowHint | Qt.WindowTitleHint
	height: 420
	title: "Test settings"
	visible: true
	width: 360

	Component.onCompleted: {
		testFilesComboBox.model = testFiles;
		if (settings.lastTestIndex >= 0 && settings.lastTestIndex < testFilesComboBox.count)
			testFilesComboBox.currentIndex = settings.lastTestIndex;
		d.loadAndPushManualTestFromFile(Qt.resolvedUrl(testFilesComboBox.currentText));
		if (app) {
			app.x = settings.appX;
			app.y = settings.appY;
		}
	}
	onClosing: close => {
		close.accepted = false;
	}

	QtObject {
		id: d

		property ContentArea contentArea: null
		property ManualTest currentManualTest: null
		property StackView stackView: null

		function getContentArea(item) {
			if (item instanceof ContentArea) {
				return item;
			}
			for (var i = 0; i < item.children.length; i++) {
				var found = getContentArea(item.children[i]);
				if (found)
					return found;
			}
			return null;
		}
		function loadAndPushManualTestFromFile(fileUrl) {
			let component = Qt.createComponent(fileUrl);
			if (component.status !== Component.Ready) {
				console.warn("Failed to load", fileUrl, component.errorString());
				return;
			}

			if (d.currentManualTest) {
				d.currentManualTest.destroy();
			}

			d.currentManualTest = component.createObject(root);
			if (!d.currentManualTest) {
				console.warn("createObject failed for", fileUrl);
				return;
			}

			variantComboBox.model = d.currentManualTest.variants.map(v => v.label);

			d.pushTestWithVariant(0);
		}
		function pushTestWithVariant(index) {
			if (!root.app) {
				console.warn("app not set");
				return;
			}

			if (!d.currentManualTest) {
				return;
			}
			let variant = d.currentManualTest.variants[index] || d.currentManualTest.variants[0];

			if (!d.contentArea) {
				let item = d.getContentArea(root.app.contentItem);
				if (!item) {
					return;
				}
				d.contentArea = item;
			}

			// StackView is different for mobile (TabBarView) and desktop (ContentArea)
			d.stackView = d.contentArea.visibleItem ?? d.contentArea;

			d.stackView.pop();
			d.stackView.push(d.currentManualTest.view, variant.props || {});
		}
	}
	Settings {
		id: settings

		property real appX: 0
		property real appY: 0
		property int lastTestIndex: 0
		property alias x: root.x
		property alias y: root.y

		category: "UiPluginDev"
	}
	Connections {
		function onXChanged() {
			settings.appX = root.app.x;
		}
		function onYChanged() {
			settings.appY = root.app.y;
		}

		enabled: root.app !== null
		target: root.app
	}
	Connections {
		function onCurrentItemChanged() {
			console.log("currentItem", d.stackView.currentItem);
		}

		enabled: d.stackView
		target: d.stackView
	}
	Connections {
		function onActiveFocusItemChanged() {
			focusIndicator.parent = root.app.activeFocusItem;
			console.log("activeFocusItem:", root.app.activeFocusItem);
		}

		enabled: checkBoxHighlightActiveFocusItem.checked
		target: root.app
	}
	Rectangle {
		id: focusIndicator

		anchors.fill: parent
		color: "red"
		opacity: 0.2
		visible: checkBoxHighlightActiveFocusItem.checked
	}
	ColumnLayout {
		anchors.fill: parent
		anchors.margins: 5
		spacing: 5

		Label {
			text: "Test files"
		}
		ComboBox {
			id: testFilesComboBox

			Layout.fillWidth: true

			onActivated: {
				settings.lastTestIndex = currentIndex;
				d.loadAndPushManualTestFromFile(Qt.resolvedUrl(currentText));
			}
		}
		Label {
			text: "Variant"
		}
		ComboBox {
			id: variantComboBox

			Layout.fillWidth: true

			onActivated: d.pushTestWithVariant(currentIndex)
		}
		Label {
			text: "Theme"
		}
		GridLayout {
			columns: 2

			RadioButton {
				checked: SettingsModel.userDarkMode === SettingsModel.ModeOption.OFF
				text: "Light"

				onToggled: SettingsModel.userDarkMode = SettingsModel.ModeOption.OFF
			}
			RadioButton {
				checked: SettingsModel.userDarkMode === SettingsModel.ModeOption.ON
				text: "Dark"

				onToggled: SettingsModel.userDarkMode = SettingsModel.ModeOption.ON
			}
			RadioButton {
				checked: SettingsModel.userDarkMode === SettingsModel.ModeOption.AUTO
				text: "System"

				onToggled: SettingsModel.userDarkMode = SettingsModel.ModeOption.AUTO
			}
		}
		Label {
			text: "Language"
		}
		RowLayout {
			Repeater {
				model: ["de", "en", "ru", "uk"]

				RadioButton {
					checked: SettingsModel.language === text
					text: modelData

					onToggled: SettingsModel.language = text
				}
			}
		}
		Label {
			text: "Misc"
		}
		RowLayout {
			CheckBox {
				id: checkBoxHighlightActiveFocusItem

				checked: false
				text: "highlight activeFocusItem"
			}
		}
		Item {
			Layout.fillHeight: true
		}
	}
}
