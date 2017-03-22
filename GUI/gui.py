from tkinter import Tk


from MainFrame import MainFrame
from GUI.GUI_Settings import GUI_Settings

root = Tk()


#root.geometry("250x150+300+300")
screen_width = root.winfo_screenwidth()
screen_height = root.winfo_screenheight()
height = root.winfo_screenmmheight()
width = 226
#width = root.winfo_screenmmwidth()
x = (screen_width/2) - (width/2)
y = (screen_height/2) - (height/2)
root.geometry('%dx%d+%d+%d' % (width, height, x, y))

app = MainFrame(root)

root.mainloop()