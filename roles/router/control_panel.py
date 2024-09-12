import curses
import serial
import libtmux
from threading import Thread

CMD_PACKET = b'\x51'

TRIGGER_RESET = b'\x01'
TURN_PANIC_ON = b'\x02'
TURN_PANIC_OFF = b'\x04'
TURN_LIGHT_ON = b'\x08'
TURN_LIGHT_OFF = b'\x10'
TURN_MVMT_ON = b'\x20'
TURN_MVMT_OFF = b'\x40'


lamps_list = "./lamps.txt"
ending = False


class Menu:


    def __init__(self):
        
        # setup default env
        self.stdscr = curses.initscr()
        curses.noecho()
        curses.cbreak()
        curses.curs_set(False)
        self.stdscr.nodelay(True)
        self.stdscr.keypad(True)

        # setup serial port
        self.ser = serial.Serial('/dev/ttyACM0')

        self.lamps = ["All"]
        with open(lamps_list) as f:
            lines = f.readlines()
            for l in lines:
                self.lamps.append(l.strip('\n'))
        self.lamps.append("Back")
        
        self.actions = ["Turn light on", "Turn light off", "Turn panic on", "Turn panic off", "Turn movement on", "Turn movement off", "Reset", "Exit"]
        
        self.action_title = "Select action:"
        self.lamp_sel_title = "Select light:"
        self.action = 0x00
        self.home_menu()


    def home_menu(self):
        self.stdscr.clear()
        current_selection = 0

        while not ending:
            self.stdscr.addstr(0, 0, self.action_title, curses.A_REVERSE)
            self.stdscr.addstr(1, 0, "")
            line = 2
            for s in self.actions:
                if line == current_selection + 2:
                    self.stdscr.addstr(line, 0, s, curses.A_UNDERLINE)
                else:
                    self.stdscr.addstr(line, 0, s)
                line += 1
            
            self.stdscr.refresh()
            c = self.stdscr.getch()
            if c == ord("j") or c == curses.KEY_DOWN:
                if current_selection < len(self.actions) - 1:
                    current_selection += 1
                else:
                    current_selection = 0
            elif c == ord("k") or c == curses.KEY_UP:
                if current_selection > 0:
                    current_selection -= 1
                else:
                    current_selection = len(self.actions) - 1
            elif c == ord("h") or c == curses.KEY_LEFT:
                self.exit()
            elif c == ord("l") or c == curses.KEY_RIGHT or c == ord("\n"):
                if self.actions[current_selection] == "Exit":
                    self.exit()
                else:
                    if self.actions[current_selection] == "Turn light on":
                        self.action = TURN_LIGHT_ON
                    elif self.actions[current_selection] == "Turn light off":
                        self.action = TURN_LIGHT_OFF
                    elif self.actions[current_selection] == "Turn panic on":
                        self.action = TURN_PANIC_ON
                    elif self.actions[current_selection] == "Turn panic off":
                        self.action = TURN_PANIC_OFF
                    elif self.actions[current_selection] == "Turn movement on":
                        self.action = TURN_MVMT_ON
                    elif self.actions[current_selection] == "Turn movement off":
                        self.action = TURN_MVMT_OFF
                    elif self.actions[current_selection] == "Reset":
                        self.action = TRIGGER_RESET
                    else:
                        pass
                    self.lamp_selection_menu()
            else:
                pass
        self.exit()


    def lamp_selection_menu(self):
        self.stdscr.clear()
        current_selection = 0

        while not ending:
            self.stdscr.addstr(0, 0, self.lamp_sel_title, curses.A_REVERSE)
            self.stdscr.addstr(1, 0, "")
            line = 2
            for s in self.lamps:
                ls = ""
                if s != "All" and s != "Back":
                    ls = "Lamp " + s
                else:
                    ls = s
                if line == current_selection + 2:
                    self.stdscr.addstr(line, 0, ls, curses.A_UNDERLINE)
                else:
                    self.stdscr.addstr(line, 0, ls)
                line += 1

            self.stdscr.refresh()
            c = self.stdscr.getch()
            if c == ord("l") or c == curses.KEY_RIGHT or c == ord('\n') or c == curses.KEY_ENTER:
                if self.lamps[current_selection] == "All":
                    for l in self.lamps[1:-1]:
                        self.ser.write(bytes.fromhex(l[2:]) + CMD_PACKET + self.action)
                elif self.lamps[current_selection] == "Back":
                    break
                else:
                    self.ser.write(bytes.fromhex(self.lamps[current_selection][2:]) + CMD_PACKET + self.action)
            elif c == ord("j") or c == curses.KEY_DOWN:
                if current_selection < len(self.lamps) - 1:
                    current_selection += 1
                else:
                    current_selection = 0
            elif c == ord("k") or c == curses.KEY_UP:
                if current_selection > 0:
                    current_selection -= 1
                else:
                    current_selection = len(self.lamps) - 1
            elif c == ord("h") or c == curses.KEY_LEFT:
                break
            else:
                pass
        if ending:
            self.exit()


    def exit(self):
        global ending
        curses.nocbreak()
        self.stdscr.keypad(False)
        self.stdscr.nodelay(False)
        curses.echo()
        curses.curs_set(True)
        curses.endwin()
        ending = True
        exit(0)


def launch_input_script() -> libtmux.Pane:
    tmux_win = libtmux.Server().windows[0]
    return tmux_win.split(direction = libtmux.constants.PaneDirection.Right, shell = "python status_panel.py")
    


if __name__ == "__main__":
    t = Thread(target = Menu)
    t.start()
    pane = launch_input_script()
    tmux_server = libtmux.Server()
    while not ending:
        if pane not in tmux_server.panes:
            ending = True

    if pane in tmux_server.panes:
        pane.kill()

    t.join()

