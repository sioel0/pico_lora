import serial
import curses
import time

lamps_list = "./lamps.txt"

class SerialInput:


    def __init__(self):
        
        self.stdscr = curses.initscr()
        curses.noecho()
        curses.cbreak()
        curses.curs_set(False)
        self.stdscr.nodelay(True)
        self.stdscr.keypad(True)

        self.ser = serial.Serial('/dev/ttyACM0')

        self.lamps = []
        with open(lamps_list) as f:
            lines = f.readlines()
            for l in lines:
                self.lamps.append(l.strip('\n'))
                
        self.title = "Lamps status"
        self.active_title = "Active"
        self.emerg_title = "Emergency"
        self.active = []
        self.emerg = []
        self.terminate = False
        for x in self.lamps:
            self.active.append(False)
            self.emerg.append(False)

        self.display_output()

    def display_output(self):
        start_check = []
        emerg_start = []
        t = time.time()
        for x in self.lamps:
            start_check.append(t)
            emerg_start.append(0)
        self.stdscr.clear()
        
        while True:
            if self.terminate:
                self.exit()
            inp = ""
            if self.ser.in_waiting > 0:
                inp = self.ser.read_until().decode().strip('\r\n')
            if inp != "":
                inp = [inp[i:i+2] for i in range(0, len(inp), 2)]
                if inp[1] == "50":
                    t = time.time()
                    start_check[int(inp[0], 16) - 1] = t
                    for i, st in enumerate(start_check):
                        if st - t > 15:
                            self.active[i] = False
                        else:
                            self.active[i] = True
                elif inp[1] == "52":
                    self.emerg[int(inp[0], 16) - 1] = True
                    emerg_start[int(inp[0], 16) - 1] = time.time()
            # reset emergency message
            for idx, x in enumerate(emerg_start):
                if x != 0:
                    if time.time() - x > 5:
                        x = 0
                        self.emerg[i] = False

            self.stdscr.addstr(0, 0, self.title, curses.A_REVERSE)
            self.stdscr.addstr(0, 19, self.active_title, curses.A_REVERSE)
            self.stdscr.addstr(0, 33, self.emerg_title, curses.A_REVERSE)
            self.stdscr.addstr(1, 0, "")

            line = 2
            for l in self.lamps:
                idx = int(l, 16) - 1
                self.stdscr.addstr(line, 0, "Lamp " + l, curses.A_BOLD)
                if self.active[idx]:
                    self.stdscr.addstr(line, 21, " ", curses.A_REVERSE)
                else:
                    self.stdscr.addstr(line, 21, "X")
                if self.emerg[idx]:
                    self.stdscr.addstr(line, 37, " ", curses.A_REVERSE)
                else:
                    self.stdscr.addstr(line, 37, "X")
                line += 1
            
            self.stdscr.refresh()
            c = self.stdscr.getch()
            if c == ord("q"):
                self.exit()


    def exit(self):
        curses.echo()
        curses.nocbreak()
        curses.curs_set(True)
        self.stdscr.keypad(False)
        self.stdscr.nodelay(False)
        curses.endwin()
        exit(0)


if __name__ == "__main__":
    SerialInput()
