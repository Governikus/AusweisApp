import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2

import AusweisApp2.Global 1.0
import AusweisApp2.Views.Provider 1.0

Window {
	id: baseItem
	title: qsTr("Provider details")
	height: Utils.dp(900)
	minimumHeight: Utils.dp(700)
	width: Utils.dp(1200)
	minimumWidth: Utils.dp(800)
	modality: Qt.ApplicationModal
	color: Constants.background_color

	property var providerModelItem: null

	ProviderDetailView {
		anchors.centerIn: parent
		height: parent.height
		width: parent.width
		providerModelItem: baseItem.providerModelItem
	}

	Connections {
		target: applicationModel
		onCurrentWorkflowChanged: if (!!applicationModel.currentWorkflow) {baseItem.close()}
	}
}
