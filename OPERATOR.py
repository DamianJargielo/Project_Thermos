import pygame
import socket
import time

# =========================
# USTAWIENIA
# =========================
ESP32_IP = "192.168.4.1"   # <-- wpisz IP swojej ESP32 z Serial Monitor
ESP32_PORT = 10000          # musi byc taki sam jak w ESP32

# =========================
# UDP
# =========================
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# =========================
# PYGAME / PAD
# =========================
pygame.init()
pygame.joystick.init()

if pygame.joystick.get_count() == 0:
    print("Nie wykryto pada")
    exit()

pad = pygame.joystick.Joystick(0)
pad.init()

print("Wykryto pad:", pad.get_name())
print("Liczba osi:", pad.get_numaxes())
print("Wysylanie danych do:", ESP32_IP, "port:", ESP32_PORT)

def deadzone(x, strefa=0.13):
    if abs(x) < strefa:
        return 0.0
    return x
pos = 1 #1- open 0-closed
while True:
    pygame.event.pump()

    # Typowe przypisanie osi dla pada:
    # axis 0 = lewy joystick lewo/prawo
    # axis 1 = lewy joystick przod/tyl
    # axis 2 lub 3 = prawy joystick lewo/prawo

    lewo_prawo = int(deadzone(pad.get_axis(0)) * 250)
    przod_tyl = int(deadzone(pad.get_axis(1)) * 250)
    obracanie = int(deadzone(pad.get_axis(3)) * 250)
    if pad.get_button(1): # red pressed
        pos = 1
    if pad.get_button(0): # green pressed
        pos = 0


    msg = f" {przod_tyl},{lewo_prawo},{obracanie},{pos}"
    sock.sendto(msg.encode("utf-8"), (ESP32_IP, ESP32_PORT))

    print("Wyslano:", msg)
    print(pos)
    time.sleep(0.05)    