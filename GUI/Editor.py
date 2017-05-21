from PyQt5.QtWidgets import (QMainWindow, QTextEdit,
                             QAction, QMenu, QColorDialog,QPlainTextEdit,QLabel,QFileDialog, QApplication,QDesktopWidget,QMessageBox,QComboBox,QVBoxLayout,QWidget,QHBoxLayout)
from PyQt5.QtGui import QIcon,QFont, QColor, QTextFormat, QPainter, QFontMetrics, QPalette, QTextCursor, QTextCharFormat, QSyntaxHighlighter
from PyQt5.QtCore import Qt, QRect, QRegExp
from PyQt5.QtCore import pyqtProperty, pyqtSignal, pyqtSlot
import re

# TODO stworzenie ładnego edytora
# TODO naprawić zoomowanie, dodanie do menu konstekstowego actions (dodaj klauzule, wyszukaj)

def format(color, style=''):
    """Return a QTextCharFormat with the given attributes.
    """
    _color = QColor()
    _color.setNamedColor(color)

    _format = QTextCharFormat()
    _format.setForeground(_color)
    if 'bold' in style:
        _format.setFontWeight(QFont.Bold)
    if 'italic' in style:
        _format.setFontItalic(True)

    return _format


# Syntax styles that can be shared by all languages
STYLES = {
    'keyword': format('blue'),
    'operator': format('red'),
    'brace': format('darkGray'),
    'defclass': format('black', 'bold'),
    'string': format('magenta'),
    'string2': format('darkMagenta'),
    'comment': format('darkGreen', 'italic'),
    'self': format('black', 'italic'),
    'numbers': format('brown'),
}


class Highlighter (QSyntaxHighlighter):
    """Syntax highlighter for the Python language.
    """

    # Python keywords
    keywords = []

    # Python operators
    operators = []

    # Python braces
    braces = [
        '(', ')',
    ]
    def __init__(self, document,settings):
        super(Highlighter, self).__init__(document)
        self.settings = settings
        # Multi-line strings (expression, flag, style)
        # FIXME: The triple-quotes in these two lines will mess up the
        # syntax highlighting from this point onward
        self.tri_single = (QRegExp("'''"), 1, STYLES['string2'])
        self.tri_double = (QRegExp('"""'), 2, STYLES['string2'])

        self.operators = map(re.escape, self.settings.parser.operators())
        self.braces = map(re.escape, self.braces)
        rules = []
        #Highlighter.operators = self.operators
        # Keyword, operator, and brace rules
        rules += [(r'\b%s\b' % w, 0, STYLES['keyword'])
            for w in Highlighter.keywords]
        rules += [(r'%s' % o, 0, STYLES['operator'])
            for o in self.operators]
        rules += [(r'%s' % b, 0, STYLES['brace'])
            for b in self.braces]

        # All other rules

        rules += [
            # Double-quoted string, possibly containing escape sequences
            (r'"[^"\\]*(\\.[^"\\]*)*"', 0, STYLES['string']),
            # Single-quoted string, possibly containing escape sequences
            (r"'[^'\\]*(\\.[^'\\]*)*'", 0, STYLES['string']),

            # From '#' until a newline
            (r'#[^\n]*', 0, STYLES['comment']),

            # Numeric literals
            (r'\b[+-]?[0-9]+[lL]?\b', 0, STYLES['numbers']),
            (r'\b[+-]?0[xX][0-9A-Fa-f]+[lL]?\b', 0, STYLES['numbers']),
            (r'\b[+-]?[0-9]+(?:\.[0-9]+)?(?:[eE][+-]?[0-9]+)?\b', 0, STYLES['numbers']),
        ]

        # Build a QRegExp for each pattern
        self.rules = [(QRegExp(pat), index, fmt)
            for (pat, index, fmt) in rules]


    def highlightBlock(self, text):
        """Apply syntax highlighting to the given block of text.
        """
        # Do other syntax formatting
        for expression, nth, format in self.rules:
            index = expression.indexIn(text, 0)

            while index >= 0:
                # We actually want the index of the nth match
                index = expression.pos(nth)
                length = len(expression.cap(nth))
                self.setFormat(index, length, format)
                index = expression.indexIn(text, index + length)

        self.setCurrentBlockState(0)

        # Do multi-line strings
        in_multiline = self.match_multiline(text, *self.tri_single)
        if not in_multiline:
            in_multiline = self.match_multiline(text, *self.tri_double)


    def match_multiline(self, text, delimiter, in_state, style):
        """Do highlighting of multi-line strings. ``delimiter`` should be a
        ``QRegExp`` for triple-single-quotes or triple-double-quotes, and
        ``in_state`` should be a unique integer to represent the corresponding
        state changes when inside those strings. Returns True if we're still
        inside a multi-line string when this function is finished.
        """
        # If inside triple-single quotes, start at 0
        if self.previousBlockState() == in_state:
            start = 0
            add = 0
        # Otherwise, look for the delimiter on this line
        else:
            start = delimiter.indexIn(text)
            # Move past this match
            add = delimiter.matchedLength()

        # As long as there's a delimiter match on this line...
        while start >= 0:
            # Look for the ending delimiter
            end = delimiter.indexIn(text, start + add)
            # Ending delimiter on this line?
            if end >= add:
                length = end - start + add + delimiter.matchedLength()
                self.setCurrentBlockState(0)
            # No; multi-line string
            else:
                self.setCurrentBlockState(in_state)
                length = len(text) - start + add
            # Apply formatting
            self.setFormat(start, length, style)
            # Look for the next match
            start = delimiter.indexIn(text, start + length)

        # Return True if still inside a multi-line string, False otherwise
        if self.currentBlockState() == in_state:
            return True
        else:
            return False

class LineNumberArea(QWidget):
    def __init__(self, editor):
        super().__init__(editor)
        self.editor = editor


    def sizeHint(self):
        return Qsize(self.editor.lineNumberAreaWidth(), 0)


    def paintEvent(self, event):
        self.editor.lineNumberAreaPaintEvent(event)


class CodeEditor(QPlainTextEdit):
    text_was_changed = False

    def setText(self,s):
        self.clear()
        self.insertPlainText(s)
        self.text_was_changed = False

    def __init__(self, parent, settings):
        super().__init__(parent=parent)

        self.settings = settings
        self.lineNumberArea = LineNumberArea(self)
        self.blockCountChanged.connect(self.updateLineNumberAreaWidth)
        self.updateRequest.connect(self.updateLineNumberArea)
        self.cursorPositionChanged.connect(self.highlightCurrentLine)
        self.textChanged.connect(self.textHasChanged)
        self.highlight = Highlighter(self.document(),self.settings)
        self.updateLineNumberAreaWidth(0)
        font = self.font()
        font.setPointSize(12)
        self.setFont(font)
        self.__addMenu()



    def __addMenu(self):
        menu = self.createStandardContextMenu()
        menu.addSeparator()
        addMenu = QMenu("&Add",self)
        addClause = QAction('Add Clause',self)
        addClause.triggered.connect(lambda : print("x"))


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


    def lineNumberAreaWidth(self):
        digits = 1
        count = max(1, self.blockCount())
        while count >= 10:
            count /= 10
            digits += 1
        space = 3 + self.fontMetrics().width('9') * digits
        return space


    def updateLineNumberAreaWidth(self, _):
        self.setViewportMargins(self.lineNumberAreaWidth(), 0, 0, 0)


    def updateLineNumberArea(self, rect, dy):

        if dy:
            self.lineNumberArea.scroll(0, dy)
        else:
            self.lineNumberArea.update(0, rect.y(), self.lineNumberArea.width(),
                                       rect.height())

        if rect.contains(self.viewport().rect()):
            self.updateLineNumberAreaWidth(0)


    def resizeEvent(self, event):
        super().resizeEvent(event)

        cr = self.contentsRect();
        self.lineNumberArea.setGeometry(QRect(cr.left(), cr.top(),
                                        self.lineNumberAreaWidth(), cr.height()))

    def lineNumberAreaPaintEvent(self, event):
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

    def textHasChanged(self):
        self.text_was_changed = True


    def toPlainTextForParser(self):
        # remove comments
        text = self.toPlainText()
        text = re.sub(r'(?m)#.*\n?', '', text)
        text = re.sub(re.compile(r"'''.*'''", re.DOTALL), '', text)

        # assume that new line is a AND
        text = "\n".join([s for s in text.splitlines() if s])
        text = text.replace("\n", " " + self.settings.parser.And + " ")
        print(text)
        return text