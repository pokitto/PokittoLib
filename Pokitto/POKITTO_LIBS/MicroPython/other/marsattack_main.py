import upygame as pygame
import urandom as random
import sprite
import marsattack_data as gamedata
import gc
import umachine as pok
#import math # not currently supported for HW build

pygame.display.init()
screen = pygame.display.set_mode() # full screen
screenRect = screen.get_rect()
screenW = screenRect.width
screenH = screenRect.height

#create and set palette
pal =[ (0,0,0),(255,255,255),(0,255,0),(24,24,177)]
pygame.display.set_palette(pal);

print('display ready')

class GameObject(sprite.Sprite):
    def __init__(self, surfaces, frameOffsets):
        sprite.Sprite.__init__(self)
        self.frames = surfaces
        self.frameOffsets = frameOffsets
        self.currentFrameNum = 0;
        self.image = self.frames[self.currentFrameNum]  # current image
        self.animDur = 0;
        self.animDurCounter = self.animDur;
        self.vx = 0
        self.vy = 0
        self.rect = self.frames[0].get_rect()
        self.numClones = 0

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

# Group
class RockPileGroup(sprite.Group):

    # Init
    def __init__(self, *sprites):
        sprite.Group.__init__(self, sprites)
        #self.refreshRate = refreshRate

    # Draw
    def draw(self, surface):

        sprites = self.sprites()
        surface_blit = surface.blit

        # draw sprite and pile of clones
        for spr in sprites:

            # draw on on refresh intervals
            self.spritedict[spr] = surface_blit(spr.image, spr.rect.x, spr.rect.y)
            for cl in range(spr.numClones):
                surface_blit(spr.image, spr.rect.x, spr.rect.y + (cl*spr.rect.height))
        self.lostsprites = []


#
all_sprites = sprite.Group()
all_rockpiles = RockPileGroup()

#Create ship
shipGob = GameObject([gamedata.shipSurf], [[0,0]])
shipGob.rect.x = 50
shipGob.rect.y = 70
shipGob.animDur = 2;
all_sprites.add(shipGob)

# Create rockpile sprites
numPiles = (screenW - (8*2)) // (gamedata.rockSurf_f0.get_rect().width-2+5)
for i in range(numPiles):
    gob = GameObject([gamedata.rockSurf_f0],[[0,0]])

    # out of screen
    gob.rect.x = 8 + i*(gob.rect.width+4)
    gob.numClones = random.getrandbits(3) + 2
    gob.rect.y = screenH - 5 - gob.numClones*gob.rect.height
    gob.animDur = 1

    all_sprites.add(gob)
    all_rockpiles.add(gob)

vx = 0;
vy = 0;
frameNum = 0
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
    shipGob.setvel(vx,vy);

    # update all gobs
    all_sprites.update()

    # draw rockpiles
    all_rockpiles.draw(screen)

    # draw ship
    screen.blit(shipGob.image, shipGob.rect.x, shipGob.rect.y)

    pygame.display.flip()

    frameNum += 1
    if frameNum > 1000000:
        frameNum = 0;
