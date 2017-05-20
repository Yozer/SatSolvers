from PyQt5.QtWidgets import (QMainWindow, QTextEdit,
                             QAction, QMenu, QColorDialog,QPlainTextEdit,QLabel,QFileDialog, QApplication,QDesktopWidget,QMessageBox,QComboBox,QVBoxLayout,QWidget,QHBoxLayout)
from PyQt5.QtGui import QIcon,QFont, QColor, QTextFormat, QPainter, QFontMetrics, QPalette, QTextCursor, QTextCharFormat
from PyQt5.QtCore import Qt, QRect
from PyQt5.QtCore import pyqtProperty, pyqtSignal, pyqtSlot

# TODO stworzenie ładnego edytora
# TODO naprawić zoomowanie, dodanie do menu konstekstowego actions (dodaj klauzule, wyszukaj)

class LineNumberArea(QWidget):
    def __init__(self, editor):
        super().__init__(editor)
        self.editor = editor


    def sizeHint(self):
        return Qsize(self.editor.lineNumberAreaWidth(), 0)


    def paintEvent(self, event):
        #print('LineNumberArea.paintEvent')
        self.editor.lineNumberAreaPaintEvent(event)


class CodeEditor(QPlainTextEdit):

    def setText(self,s):
        self.insertPlainText(s)


    def __init__(self,settings):
        super().__init__()

        self.settings = settings
        self.lineNumberArea = LineNumberArea(self)
        self.blockCountChanged.connect(self.updateLineNumberAreaWidth)
        self.updateRequest.connect(self.updateLineNumberArea)
        self.cursorPositionChanged.connect(self.highlightCurrentLine)
        self.textChanged.connect(lambda :print("text"))
        self.startedComment = False
        self.__addMenu()

        self.updateLineNumberAreaWidth(0)

    def comments(self):
        print ("change")

    def __addMenu(self):
        menu = self.createStandardContextMenu()
        menu.addSeparator()
        addMenu = QMenu("&Add",self)
        addClause = QAction('Add Clause',self)
        addClause.triggered.connect(self.textColorize)
        addMenu.addAction(addClause)
        menu.addMenu(addMenu)
        self.clause = addClause
        #self.setContextMenuPolicy(Qt.CustomContextMenu)
        #self.setContextMenuPolicy(Qt.ActionsContextMenu)
        self.menu = menu



    def contextMenuEvent(self, event):
        result = self.menu.exec_(self.mapToGlobal(event.pos()))
        if result == self.clause:
            print ("yes")

    def textColorize(self):
        col = self.settings.commentColor


        fmt = QTextCharFormat()
        fmt.setForeground(col)
        self.mergeFormatOnWordOrSelection(fmt)

    def mergeFormatOnWordOrSelection(self, format):
        cursor = self.textCursor()
        if not cursor.hasSelection():
            cursor.select(QTextCursor.WordUnderCursor)

        cursor.mergeCharFormat(format)
        self.mergeCurrentCharFormat(format)

    def lineNumberAreaWidth(self):
        digits = 1
        count = max(1, self.blockCount())
        while count >= 10:
            count /= 10
            digits += 1
        space = 3 + self.fontMetrics().width('9') * digits
        return space


    def updateLineNumberAreaWidth(self, _):
        #print('CodeEditor.updateLineNumberAreaWidth: margin = {}'.format(self.lineNumberAreaWidth()))
        self.setViewportMargins(self.lineNumberAreaWidth(), 0, 0, 0)


    def updateLineNumberArea(self, rect, dy):
        #print('CodeEditor.updateLineNumberArea: rect = {}, dy = {}'.format(rect, dy))

        if dy:
            self.lineNumberArea.scroll(0, dy)
        else:
            self.lineNumberArea.update(0, rect.y(), self.lineNumberArea.width(),
                                       rect.height())

        #print('CodeEditor.updateLineNumberArea: rect.contains(self.viewport().rect()) = {}'.format(rect.contains(self.viewport().rect())))
        if rect.contains(self.viewport().rect()):
            self.updateLineNumberAreaWidth(0)


    def resizeEvent(self, event):
        super().resizeEvent(event)

        cr = self.contentsRect();
        self.lineNumberArea.setGeometry(QRect(cr.left(), cr.top(),
                                        self.lineNumberAreaWidth(), cr.height()))

    def lineNumberAreaPaintEvent(self, event):
        #print('CodeEditor.lineNumberAreaPaintEvent')
        painter = QPainter(self.lineNumberArea)
        painter.fillRect(event.rect(), Qt.lightGray)

        block = self.firstVisibleBlock()
        blockNumber = block.blockNumber()
        top = self.blockBoundingGeometry(block).translated(self.contentOffset()).top()
        bottom = top + self.blockBoundingRect(block).height()

        # Just to make sure I use the right font
        height = QFontMetrics(self.font()).height()
        while block.isValid() and (top <= event.rect().bottom()):
            if block.isVisible() and (bottom >= event.rect().top()):
                number = str(blockNumber + 1)
                painter.setPen(Qt.black)
                painter.drawText(0, top, self.lineNumberArea.width(), height,
                                 Qt.AlignRight, number)

            block = block.next()
            top = bottom
            bottom = top + self.blockBoundingRect(block).height()
            blockNumber += 1


    def __startComment(self):
        pallete = self.palette()
        pallete.setColor(QPalette.Text , self.settings.commentColor)
        self.setPalette(pallete)


    def highlightCurrentLine(self):
        extraSelections = []



        if not self.isReadOnly():
            selection = QTextEdit.ExtraSelection()

            lineColor = QColor(self.settings.getHighlight())
            selection.format.setBackground(lineColor)
            selection.format.setProperty(QTextFormat.FullWidthSelection, True)
            selection.cursor = self.textCursor()
            selection.cursor.clearSelection()
            extraSelections.append(selection)
        self.setExtraSelections(extraSelections)