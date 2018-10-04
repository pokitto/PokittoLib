#include "Pokitto.h"
#include "USBSerial.h"

Pokitto::Core game;

USBSerial pc;

class PokSer : USBSerial {
public:
    bool isMessage();
    char* getMessage();
    bool discardAfterRead = true;

private:
    void update();
    bool _messageWaiting = false;
    int _currentMessage = 0;
    int _head=0;
    char _messages[2][30];


};

bool PokSer::isMessage() { return _messageWaiting; }

char* PokSer::getMessage() {
                if (!_messageWaiting) return 0;
                return &(_messages[1-_currentMessage][0]);
                if (discardAfterRead) {_messageWaiting = false;}
}

void PokSer::update() {
        while (readable()) {
                char t = getc();
                if (t=='\n' || t=='\r' || t == 0) {
                    _messageWaiting = true;
                    _messages[_currentMessage][_head] = 0;
                    _currentMessage = 1 - _currentMessage;
                    _head = 0;
                } else {
                    _messages[_currentMessage][_head++] = t;
                    if (_head == 30) {
                       _currentMessage = 1 - _currentMessage;
                       _head = 0;
                    }
                }
        }
    }


int main () {
game.display.palette[0]=COLOR_BLUE;
game.display.palette[1]=COLOR_WHITE;
game.begin();
game.display.persistence=1;
game.display.setFont(fontC64);

game.display.adjustCharStep = 0; //needed for the non-proportional C64 font (normal value=1)

game.display.print("Serial test");

while (game.isRunning()) {
        if (game.update()) {
			if (pc.readable()) {
			        game.display.print((char)pc.getc());
                    pc.printf("OK\n");
			}

        }
}

return 1;
}

