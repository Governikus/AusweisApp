/*
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Qt.labs.platform 1.1 as Labs

Labs.FileDialog {
	function selectFile(pDefaultFilename) {
		currentFile = folder + "/" + pDefaultFilename;
		open();
	}

	fileMode: Labs.FileDialog.SaveFile
	folder: Labs.StandardPaths.writableLocation(Labs.StandardPaths.DocumentsLocation)

	Component.onDestruction: reject()
}
