import QtQuick 2.5

// This proxy file is required since a Component loaded via the Loader.source URL
// does not know which platform selector is in charge.
ContentArea {
    id: contentArea
    anchors.fill: parent
}
