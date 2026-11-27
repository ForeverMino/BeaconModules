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

    FileDialog {
        id: fileDialog
        onAccepted: root.attachAtCursor(selectedFile)
    }

    function attachAtCursor(fileUrl)
    {
        const html = NoteManager.attachFile(root.selectedNoteId, fileUrl)
        if (html.length > 0)
        {
            noteEditor.insert(noteEditor.cursorPosition, html)
            NoteManager.updateNote(root.selectedNoteId, noteEditor.text)
        }
    }

    Action
    {
        id: boldAction
        text: "B"
        shortcut: StandardKey.Bold
        enabled: root.selectedNoteId > 0
        checkable: true
        checked: noteEditor.cursorSelection.font.bold
        onTriggered: {
            noteEditor.cursorSelection.font.bold = !noteEditor.cursorSelection.font.bold
            NoteManager.updateNote(root.selectedNoteId, noteEditor.text)
        }
    }

    Action
    {
        id: italicAction
        text: "I"
        shortcut: StandardKey.Italic
        enabled: root.selectedNoteId > 0
        checkable: true
        checked: noteEditor.cursorSelection.font.italic
        onTriggered: {
            noteEditor.cursorSelection.font.italic = !noteEditor.cursorSelection.font.italic
            NoteManager.updateNote(root.selectedNoteId, noteEditor.text)
        }
    }

    Action
    {
        id: underlineAction
        text: "U"
        shortcut: StandardKey.Underline
        enabled: root.selectedNoteId > 0
        checkable: true
        checked: noteEditor.cursorSelection.font.underline
        onTriggered: {
            noteEditor.cursorSelection.font.underline = !noteEditor.cursorSelection.font.underline
            NoteManager.updateNote(root.selectedNoteId, noteEditor.text)
        }
    }

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
                    width: ListView.view.width
                    required property int noteId
                    required property string noteName
                    required property string noteContent
                    text: noteName
                    highlighted: root.selectedNoteId === noteId
                    onClicked: {
                        root.selectedNoteId = noteId
                        titleField.text = noteName
                        noteEditor.text = noteContent
                        NoteManager.selectNote(noteId)
                    }
                }
            }
        }

        ColumnLayout // note
        {
            enabled: root.selectedNoteId > 0
            Layout.fillHeight: true
            Layout.fillWidth: true

            RowLayout
            {
                Layout.fillWidth: true

                Button
                {
                    text: "Delete"
                    onClicked: {
                        if (NoteManager.deleteNote(root.selectedNoteId)) {
                            root.selectedNoteId = -1
                            titleField.text = ""
                            noteEditor.text = ""
                        }
                    }
                }

                Button
                {
                    text: "+ Attachment"
                    onClicked: fileDialog.open()
                }

                ToolButton { action: boldAction; font.bold: true }
                ToolButton { action: italicAction; font.italic: true }
                ToolButton { action: underlineAction; font.underline: true }

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
                        textFormat: TextEdit.RichText
                        persistentSelection: true
                        onLinkActivated: (link) => NoteManager.openAttachment(link)
                    }
                }

                DropArea
                {
                    anchors.fill: parent
                    onDropped: (drop) => {
                        if (drop.hasUrls) {
                            for (let i = 0; i < drop.urls.length; ++i) {
                                root.attachAtCursor(drop.urls[i])
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
        }
    }
}