import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import QtQuick.Dialogs
import BeaconModules


ApplicationWindow {

    id: root
    width: 640
    height: 480
    minimumWidth: 200
    minimumHeight: 250
    visible: true
    title: qsTr("Note Taking")
    property bool lightMode: Application.styleHints.colorScheme === Qt.Light
    property color reallyDark: "#1f1f1f"
    property color dark: "#262626"
    property color reallyLight: "#e7e7e7"
    property color light: "#e0e0e0"
    property int selectedNoteId: -1
    FileDialog { id: fileDialog; onAccepted: NoteManager.attachFile(root.selectedNoteId, selectedFile) }

    RowLayout
    {
        anchors.fill: parent

        ColumnLayout // left bar
        {
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width * 0.2

            Button
            {
                Layout.fillWidth: true
                text: "+ New Note"
                onClicked: NoteManager.createNote("")
            }

            ListView
            {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                model: NoteManager.noteListModel
                delegate: ItemDelegate
                {
                    width: ListView.view.width;
                    required property int noteId
                    required property string noteName
                    required property string noteContent
                    text: noteName
                    highlighted: root.selectedNoteId === noteId
                    onClicked: {root.selectedNoteId = noteId; titleField.text = noteName; noteEditor.text = noteContent;
                        NoteManager.selectNote(noteId)}

                }
            }
        }

        ColumnLayout // note
        {
            enabled: root.selectedNoteId > 0

            RowLayout
            {
                Layout.fillWidth: true

                Button
                {
                    text: "Delete"
                    onClicked: if(NoteManager.deleteNote(root.selectedNoteId)) {root.selectedNoteId = -1; titleField.text = "";
                                   noteEditor.text = ""}
                }

                Button
                {
                    text: "+ Attachment"
                    onClicked: fileDialog.open()
                }

                TextField
                {
                    id: titleField
                    Layout.fillWidth: true
                    onTextEdited: NoteManager.changeNoteName(root.selectedNoteId, text)
                }

            }

            Item
            {
                Layout.fillWidth: true
                Layout.fillHeight: true

                ScrollView
                {
                    anchors.fill: parent

                    TextArea
                    {
                        id: noteEditor
                        wrapMode: TextArea.Wrap
                        onTextEdited: NoteManager.updateNote(root.selectedNoteId, text)
                    }
                }

                DropArea
                {
                    anchors.fill: parent
                    onDropped: (drop) => {
                        if (drop.hasUrls) {
                            for (let i = 0; i < drop.urls.length; ++i) {
                                NoteManager.attachFile(root.selectedNoteId, drop.urls[i])
                            }
                        }
                    }
                }
            }

            ListView
            {
                Layout.fillWidth: true
                Layout.preferredHeight: 44
                orientation: ListView.Horizontal
                spacing: 8
                clip: true
                visible: count > 0
                model: NoteManager.attachmentListModel
                delegate: RowLayout
                {
                    required property int attachmentId
                    required property string fileName
                    required property string filePath
                    required property string fileType

                    Button
                    {
                        text: fileName
                        onClicked: NoteManager.openAttachment(filePath)
                    }

                    Button
                    {
                        text: "X"
                        onClicked: NoteManager.removeAttachment(attachmentId)
                    }
                }
            }

            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }

}
