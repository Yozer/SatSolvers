import sys
from tkinter import Tk, Frame, BOTH, Button, ttk, Text
from FileManager import FileManger
from GUI_Settings import GUI_Settings
sys.path.append("../")
from ClauseHelper import ClauseHelper
from GlucoseSolver import GlucoseSolver


class MainFrame(Frame):
    def __init__(self, parent):
        Frame.__init__(self, parent, background="white")

        self.parent = parent

        self.initUI()
        self.bind("<Configure>", lambda e: print(e.width, e.height))

    def initUI(self):
        self.parent.title("SatSolvers")
        self.style = ttk.Style()
        self.style.theme_use("default")

        self.pack(fill=BOTH, expand=True)

        # Create and sets buttons
        buttonFrame = Frame(self)
        buttonFrame.grid(row=0, column=0, sticky='w')

        newClauseButton = Button(buttonFrame, text="Open", command=self.updateClause)
        newClauseButton.pack(side='left', padx=5, pady=2)

        saveClauseButton = Button(buttonFrame, text="Save", command=self.saveClause)
        saveClauseButton.pack(side='left', padx=5)

        computeButton = Button(buttonFrame, text="Calculate", command=self.solve)
        computeButton.pack(side='left', padx=5)

        frame1 = Frame(self, height=50)
        frame1.grid(row=1, column=0)

        # ustawiam wysokosc i szerokość w oddzielnych funkcjach dla czytelności kodu
        self.clauseText = Text(frame1, height=GUI_Settings.clauseTextHeight, \
                               width=GUI_Settings.clauseTextWidth, \
                               font=GUI_Settings.clauseTextFont)

        # self.clauseText.pack(side = 'top', ipady = 1, fill = 'x')
        # self.clauseText.grid(row = 1, sticky = 'w')
        self.clauseText.pack(side='left', ipady=1, fill='y')
        scroll = ttk.Scrollbar(frame1)
        scroll.pack(side='right', fill='y')
        scroll.config(command=self.clauseText.yview)
        self.clauseText.config(yscrollcommand=scroll.set)

        # Create frame, where is result text
        frame2 = Frame(self, height=50)
        # frame1.pack(fill = BOTH, expand=True)
        frame2.grid(row=2, column=0)

        self.resultText = Text(frame2, height=GUI_Settings.resultTextHeight, \
                               width=GUI_Settings.resultTextWidth, \
                               font=GUI_Settings.resultTextFont, \
                               state='disabled', \
                               cursor='arrow')
        self.resultText.pack(side='left', ipady=1, fill='y')
        scroll = ttk.Scrollbar(frame2)
        scroll.pack(side='right', fill='y')
        scroll.config(command=self.resultText.yview)
        self.resultText.config(yscrollcommand=scroll.set)

    def printResult(self, model, variables):
        self.resultText.config(state='normal')
        self.resultText.delete(1.0, 'end')

        for v in variables:
            self.resultText.insert('current', "x" + str(v) + "=" + str(model[v]) + "\n")

        self.resultText.config(state='disabled')

    # Load new clause from file
    def updateClause(self):
        tmpClause = FileManger.openFile()
        self.clauseText.delete(1.0, 'end')
        self.clauseText.insert(1.0, tmpClause)

    # Save clause to file
    def saveClause(self):
        FileManger.saveFile(self.clauseText.get(1.0, 'end'))


    def solve(self):
        text = self.clauseText.get(1.0, 'end')
        if (ClauseHelper.checkClause(text)):
            (dimacs, variables) = ClauseHelper.parse_to_dimacs(text)

            solver = GlucoseSolver()
            model = solver.solve(dimacs)
            self.printResult(model, variables)
