import upygame as pygame
import framebuf
import urandom as random
import sprite
import gc
import umachine as pok
#import math # Not currently supported for HW build. That is because floats are not supported in HW ans they would be slow. 

# Inititalize display
pygame.display.init()
screen = pygame.display.set_mode() # full screen
screenRect = screen.get_rect()
screenW = screenRect.width
screenH = screenRect.height

# Create radient palette of 16 colors.
pal = [];
for cc in range(16):
    c = cc*32//16
    pal.append((c << 11) | c<<6 | c>>1 );
    print('index:',cc,' c=',c,' col=', pal[cc],' binary=', bin(c),'b=',bin(pal[cc]) )

# Boost lightest colors a bit
pal[13] = 0b1111011111111110
pal[14] = 0b1111011111111110
pal[15] = 0b1111111111111111

# Set aplette
pygame.display.set_palette_16bit(pal);

#create sine array
fpFactor = 256; # Fixed point factor
pseudoPi = 128; # PI as an array index

# Cannot use this on HW as math lib is not supported.
#fpSinCosArray = []
#for pseudoRad in range(3*pseudoPi):  # store array of 3*pi elements to contain full sin and cos circle
#    rad = pseudoRad * math.pi / pseudoPi
#    fpSin = int(round(math.sin(rad)*fpFactor))
#    fpSinCosArray.append(fpSin)
#    #print("pseudoRad=",pseudoRad,"rad=",rad,"fpSin=",fpSin)
#    print(",",fpSin,end='')

# Use precalculated Sine & Cosine array. Have a values of it 3 times the PI to have both Sine and Cosine. Note: a bit less would be enough
fpSinCosArray = [
    0, 6, 13, 19, 25, 31, 38, 44, 50, 56, 62, 68, 74, 80, 86, 92, 98, 104, 109, 115, 121, 126, 132, 137, 142,
    147, 152, 157, 162, 167, 172, 177, 181, 185, 190, 194, 198, 202, 206, 209, 213, 216, 220, 223, 226, 229,
    231, 234, 237, 239, 241, 243, 245, 247, 248, 250, 251, 252, 253, 254, 255, 255, 256, 256, 256, 256, 256,
    255, 255, 254, 253, 252, 251, 250, 248, 247, 245, 243, 241, 239, 237, 234, 231, 229, 226, 223, 220, 216,
    213, 209, 206, 202, 198, 194, 190, 185, 181, 177, 172, 167, 162, 157, 152, 147, 142, 137, 132, 126, 121,
    115, 109, 104, 98, 92, 86, 80, 74, 68, 62, 56, 50, 44, 38, 31, 25, 19, 13, 6, 0, -6, -13, -19, -25, -31,
    -38, -44, -50, -56, -62, -68, -74, -80, -86, -92, -98, -104, -109, -115, -121, -126, -132, -137, -142,
    -147, -152, -157, -162, -167, -172, -177, -181, -185, -190, -194, -198, -202, -206, -209, -213, -216,
    -220, -223, -226, -229, -231, -234, -237, -239, -241, -243, -245, -247, -248, -250, -251, -252, -253,
    -254, -255, -255, -256, -256, -256, -256, -256, -255, -255, -254, -253, -252, -251, -250, -248, -247,
    -245, -243, -241, -239, -237, -234, -231, -229, -226, -223, -220, -216, -213, -209, -206, -202, -198,
    -194, -190, -185, -181, -177, -172, -167, -162, -157, -152, -147, -142, -137, -132, -126, -121, -115,
    -109, -104, -98, -92, -86, -80, -74, -68, -62, -56, -50, -44, -38, -31, -25, -19, -13, -6, 0, 6, 13, 19,
    25, 31, 38, 44, 50, 56, 62, 68, 74, 80, 86, 92, 98, 104, 109, 115, 121, 126, 132, 137, 142, 147, 152, 157,
    162, 167, 172, 177, 181, 185, 190, 194, 198, 202, 206, 209, 213, 216, 220, 223, 226, 229, 231, 234, 237,
    239, 241, 243, 245, 247, 248, 250, 251, 252, 253, 254, 255, 255, 256, 256, 256, 256, 256, 255, 255, 254,
    253, 252, 251, 250, 248, 247, 245, 243, 241, 239, 237, 234, 231, 229, 226, 223, 220, 216, 213, 209, 206,
    202, 198, 194, 190, 185, 181, 177, 172, 167, 162, 157, 152, 147, 142, 137, 132, 126, 121, 115, 109, 104,
    98, 92, 86, 80, 74, 68, 62, 56, 50, 44, 38, 31, 25, 19, 13, 6
]

# Ship picture frame
ship_w = 8
ship_h = 7
shipPixels = [
0x00,0x0F,0x00,0x00,
0x00,0xF0,0xF0,0x00,
0x00,0xF0,0xF0,0x00,
0x0F,0x00,0x0F,0x00,
0x0F,0x00,0x0F,0x00,
0xF0,0x00,0x00,0xF0,
0xFF,0xFF,0xFF,0xF0,
]
shipBuf = bytearray(shipPixels)
shipSurf = pygame.surface.Surface(ship_w, ship_h, shipBuf)

# Small bullet picture frame
bullet2_w = 4
bullet2_h = 4
bullet2Pixels = [
0xBF,0xB0,
0xFF,0xF0,
0xBF,0xB0,
]
bullet2Buf = bytearray(bullet2Pixels)
bullet2Surf = pygame.surface.Surface(bullet2_w, bullet2_h, bullet2Buf)

print('display ready')

class GameObject(sprite.Sprite):
    def __init__(self, surfaces, frameOffsets):
        sprite.Sprite.__init__(self)
        self.frames = surfaces
        self.frameOffsets = frameOffsets
        self.currentFrameNum = 0;
        self.image = self.frames[self.currentFrameNum]  # current image
        self.animDur = 3;
        self.animDurCounter = self.animDur;
        self.vx = 0
        self.vy = 0
        self.rect = self.frames[0].get_rect()

    def setvel(self, vx, vy):
        self.vx = vx
        self.vy = vy

    def update(self):

        # Advance frame if animation is set
        if self.animDur > 0:

            # if animation duration has elapsed, advance frame
            if self.animDurCounter == 0:
                self.currentFrameNum += 1
                if self.currentFrameNum >= len(self.frames):
                    self.currentFrameNum = 0

                #
                self.animDurCounter = self.animDur

                # Set current image
                self.image = self.frames[self.currentFrameNum]
                self.rect.x += self.frameOffsets[self.currentFrameNum][0]
                self.rect.y += self.frameOffsets[self.currentFrameNum][1]
            else:
                self.animDurCounter -= 1

        # Advance position
        self.rect.x += self.vx
        self.rect.y += self.vy

# Create sprite groups
all_sprites = sprite.Group()
all_ships = sprite.Group()

# Create ship sprites. Note: only one in this demo.
for i in range(1):
    shipGob = GameObject([shipSurf], [[0,0]])

    # out of screen
    shipGob.rect.x = 50
    shipGob.rect.y = 70

    shipGob.animDur = 2;

    all_sprites.add(shipGob)
    all_ships.add(shipGob)

# Create dot object
dotGob = GameObject([bullet2Surf], [[0,0]])
dotGob.rect.x = -100
dotGob.rect.y = -100
all_sprites.add(dotGob)

# Create dot2 object
dot2Gob = GameObject([bullet2Surf], [[0,0]])
dot2Gob.rect.x = -100
dot2Gob.rect.y = 70
all_sprites.add(dot2Gob)

print('all_ships len=', len(all_ships))

vx = 0;
vy = 0;
frameNum = 0
lastY = 0
textX = 0
textY = screenH + 100
while True:

    #print('frameNum=',frameNum)vx

    # read keys
    eventtype = pygame.event.poll()
    if eventtype != pygame.NOEVENT:
        if eventtype.type == pygame.KEYDOWN:
            if eventtype.key == pygame.K_RIGHT:
                vx = 1
            if eventtype.key == pygame.K_LEFT:
                vx = -1
            if eventtype.key == pygame.K_UP:
                vy = -1
            if eventtype.key == pygame.K_DOWN:
                vy = 1
        if eventtype.type == pygame.KEYUP:
            if eventtype.key == pygame.K_RIGHT:
                vx = 0
            if eventtype.key == pygame.K_LEFT:
                vx = 0
            if eventtype.key == pygame.K_UP:
                vy = 0
            if eventtype.key == pygame.K_DOWN:
                vy = 0
                
    # Set ship velocity.
    shipGob.setvel(vx,vy);

    # Animate dot1 to go circle
    pseudoRad = 4 * (frameNum % (pseudoPi//2))
    size = 230
    #print("pseudoRad=",pseudoRad,"pseudoRad+pseudoPi//2",pseudoRad+pseudoPi//2)
    fpSin = fpSinCosArray[pseudoRad] // 2 # (256,-256)
    dotGob.rect.x = 5*fpSin // (256-size)
    dotGob.rect.x += 40
    fpCos = fpSinCosArray[pseudoRad+pseudoPi//2] // 2 #(256,-256)
    dotGob.rect.y = 5*fpCos // (256-size)
    dotGob.rect.y += 30
    screen.blit(dotGob.image, dotGob.rect.x, dotGob.rect.y)

    # Mirror dot1, 3-way
    screen.blit(dotGob.image, screenW-dotGob.rect.x, dotGob.rect.y)
    screen.blit(dotGob.image, screenW-dotGob.rect.x, screenH - dotGob.rect.y)
    screen.blit(dotGob.image, dotGob.rect.x, screenH - dotGob.rect.y)

    # Animate dot2 to go horizontally
    x = dot2Gob.rect.x
    x -= 1
    if(x<-10):
        x = 110 + 2
    dot2Gob.rect.x = x
    screen.blit(dot2Gob.image, dot2Gob.rect.x, dot2Gob.rect.y)

    all_ships.update()
    all_ships.draw(screen)

    # Animate text to go vertically
    if(frameNum%2==0):
        y = textY
        y += 1
        if(y>screenH-10):
            y = 0
        textY = y
    pok.draw_text(textX, textY, "PokittoScope", 14)
    
    pygame.display.flip()

    frameNum += 1
    if frameNum > 1000000:
        frameNum = 0;
