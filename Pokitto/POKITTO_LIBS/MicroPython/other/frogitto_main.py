import upygame as pygame
import framebuf
import urandom as random
import frogitto_data as spritedata
import sprite
import gc

pygame.display.init()
pygame.display.set_palette_16bit([
    0, 6438, 18917, 10825, 47398, 688, 41764, 17475,
    58225, 13598, 60486, 40179, 42596, 46845, 63245, 65535
]);

screen = pygame.display.set_mode() # full screen

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

#
all_sprites = sprite.Group()
all_frogittos = sprite.Group()
all_cars = sprite.Group()

# Create frogitto sprites
for i in range(1):
    frogittoGob = GameObject(
        [spritedata.frogittoSurf_f1, spritedata.frogittoSurf_f2, spritedata.frogittoSurf_f3],
        [[0,0],[0,0],[0,0]])

    # out of screen
    frogittoGob.rect.x = 50
    frogittoGob.rect.y = 70

    frogittoGob.animDur = 2;

    all_sprites.add(frogittoGob)
    all_frogittos.add(frogittoGob)

# Create blue and red car sprites
for i in range(10):
    carGob = GameObject(
        [spritedata.bluecarSurf_f1, spritedata.bluecarSurf_f2],
        [[0,0],[0,0]])

    # out of screen
    carGob.rect.x = -100
    carGob.rect.y = -100

    all_sprites.add(carGob)
    all_cars.add(carGob)

    carGob = GameObject(
        [spritedata.redcarSurf_f1, spritedata.redcarSurf_f2],
        [[0,0],[0,0]])

    # out of screen
    carGob.rect.x = -100
    carGob.rect.y = -100

    all_sprites.add(carGob)
    all_cars.add(carGob)

print('all_frogittos len=', len(all_frogittos))

vx = 0;
vy = 0;
frameNum = 0;
lastY = 0
while True:

    #print('frameNum=',frameNum)vx

    hit = sprite.spritecollideany(frogittoGob, all_cars)
    #print ('hit',hit.rect)
    #print ('frogitto',frogittoGob.rect)
    if hit != None:
        frogittoGob.rect.x = 50
        frogittoGob.rect.y = 70

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

    frogittoGob.setvel(vx,vy);


    # Launch new frogitto after n frames
#    if (frameNum % 20) == 0:
#        # Get first free frogitto
#        for s in all_frogittos:
#            if s.y < -8:
#                s.x = 40 + random.getrandbits(4)
#                s.y = 88
#                s.currentFrameNum = 0
#
#                break

    # Launch new car after n frames
    if (frameNum % 13) == 0:
        # Get first free car and set the starting position
        for s in all_cars:
            if s.rect.x < -16:
                s.rect.x = 110
                y = 10 + random.getrandbits(8) * 60 // 256
                if abs(y - lastY) < 7:
                    if y < lastY:
                        y -= 7
                    else:
                        y += 7
                s.rect.y = y
                lastY = y
                s.setvel(-1,0)
                s.currentFrameNum = 0

                break

    all_sprites.update()
    all_sprites.draw(screen)

    pygame.display.flip()

    frameNum += 1
    if frameNum > 1000000:
        frameNum = 0;
