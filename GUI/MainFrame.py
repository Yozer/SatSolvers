from tkinter import Tk, Frame, BOTH, Button, ttk, Text
from GUI.FileManager import FileManger
from GUI.GUI_Settings import GUI_Settings
from ClauseHelper import ClauseHelper
from GlucoseSolver import GlucoseSolver


class MainFrame(Frame):
    def __init__(self, parent):
        Frame.__init__(self, parent, background="white")

        self.parent = parent

        self.initUI()
        self.bind( "<Configure>", lambda e: print (e.width,e.height))


    def initUI(self):
        self.parent.title("SatSolvers")
        self.style = ttk.Style()
        self.style.theme_use("default")

        self.pack(fill=BOTH, expand=True)


        # Create and sets buttons
        buttonFrame = Frame(self)
        buttonFrame.grid(row = 0, column = 0, sticky = 'w')

        newClauseButton = Button(buttonFrame, text = "Open", command=self.updateClause)
        newClauseButton.pack(side = 'left', padx = 5, pady = 2)

        saveClauseButton = Button(buttonFrame, text = "Save", command=self.saveClause)
        saveClauseButton.pack(side = 'left', padx = 5)

        computeButton = Button(buttonFrame, text = "Calculate", command = self.solve)
        computeButton.pack(side = 'left', padx = 5)



        # Create frame, where is text with clause
        frame1 = Frame(self, height = 50)
        #frame1.pack(fill = BOTH, expand=True)
        frame1.grid(row = 2, column = 0)

        # ustawiam wysokosc i szerokość w oddzielnych funkcjach dla czytelności kodu
        self.clauseText = Text(self, height = GUI_Settings.clauseTextHeight,\
                            width = GUI_Settings.clauseTextWidth,\
                            font = GUI_Settings.clauseTextFont)

        #self.clauseText.pack(side = 'top', ipady = 1, fill = 'x')
        self.clauseText.grid(row = 1, sticky = 'w')

        self.resultText = Text(frame1, height = GUI_Settings.resultTextHeight,\
                          width = GUI_Settings.resultTextWidth,\
                          font = GUI_Settings.resultTextFont,\
                          state = 'disabled', \
                          cursor = 'arrow')
        self.resultText.pack(side = 'left', ipady = 1, fill = 'y')
        scroll = ttk.Scrollbar(frame1)
        scroll.pack(side = 'right', fill='y')
        scroll.config(command=self.resultText.yview)
        self.resultText.config(yscrollcommand = scroll.set)



    def printResult(self,model,variables):
        self.resultText.config(state = 'normal')
        self.resultText.delete(1.0, 'end')

        for v in variables:
            self.resultText.insert('current',"x" + str(v) + "=" + str(model[v]) + "\n")

        self.resultText.config(state = 'disabled')

    # Load new clause from file
    def updateClause(self):
        tmpClause = FileManger.openFile()
        self.clauseText.delete(1.0, 'end')
        self.clauseText.insert(1.0, tmpClause)


    # Save clause to file
    def saveClause(self):
        FileManger.saveFile(self.clauseText.get(1.0,'end'))

    # narazie do debugowania, poprawiania wyglądu
    def on_resize(event):
        print (event.width, event.height)


    def solve(self):
        text = self.clauseText.get(1.0,'end')
        if(ClauseHelper.checkClause(text)):

            (dimacs, variables) = ClauseHelper.parse_to_dimacs(text)

            solver = GlucoseSolver()
            model = solver.solve(dimacs)
            self.printResult(model,variables)
