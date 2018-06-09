//#include "sprites.h"
class Ent {
    public:
    int x,y;
    int id;
    Ent(int xx,int yy) {x=xx; y=yy;};
    void draw();
    Ent() {
        id=11;
        }
};


class Goblin : public Ent {
    public:

    int id;
    Goblin() {
        id=11;
        }
    
};

