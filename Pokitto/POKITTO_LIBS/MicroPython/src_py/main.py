print('*** start python skript')
import gc
import upygame as pygame
import urandom as random
import sprite
import marsattack_data as gamedata
import marsattack_classes as gameclass
import umachine as pok
#import math # not currently supported for HW build

pygame.display.init()
screen = pygame.display.set_mode() # full screen
screenRect = screen.get_rect()
screenW = screenRect.width
screenH = screenRect.height

# Set palette
pygame.display.set_palette_16bit([60683,32963,49572,65502]);

print('*** display ready')

# Callback which is called when the gob anim cycle is finished
def HideGobAfterAnimCycle(gob):
    # Note: only move y so that the dirty rect for a sprite is optimum.
    gob.vy = 999

#
def InitRockPile(gob, numClones):
    gob.frames[0] = gob.frames[1] # head rock is the same as body rock
    gob.image = gob.frames[1]
    gob.numClones = numClones
    gob.rect.height = gob.image.get_rect().height
    gob.rect.width = gob.image.get_rect().width
    gob.rect.y = screenH - 5 - (gob.numClones*gob.frames[1].get_rect().height) - gob.frames[0].get_rect().height

#
all_sprites = sprite.Group()
all_rockpiles = gameclass.RockPileGroup()
all_gobs = gameclass.GobGroup()

# Create rockpile sprites
#numPiles = (screenW - (8*2)) // (gamedata.rockSurf_f0.get_rect().width-2+5)
pileIndexList = [0,1,2,3,4,5,6,7]
towerSurfaceList = [
    gamedata.rock1Surf, gamedata.rock2Surf, gamedata.rock3Surf, gamedata.rock4Surf,
    gamedata.rock5Surf, gamedata.rock6Surf, gamedata.rock7Surf, gamedata.rock7Surf ]
towerHeadSurfaceList = [
    gamedata.rockHead1Surf, gamedata.rockHead2Surf, gamedata.rockHead3Surf, gamedata.rockHead4Surf,
    gamedata.rockHead5Surf, gamedata.rockHead6Surf, gamedata.rockHead7Surf, gamedata.rock7Surf ]
numPiles = len(pileIndexList)
for i in range(numPiles):
    rockSF = towerSurfaceList[pileIndexList[i]]

    # Only put head to some towers
    #if(random.getrandbits(3) > 3 ):
    if True:
        rockHeadSF = towerHeadSurfaceList[pileIndexList[i]]
    else:
       rockHeadSF = rockSF

    # Create gob
    gob = gameclass.GameObject([rockHeadSF, rockSF],[[0,0], [0,0]], screen, screenW, screenH)

    # Set pos
    gob.rect.x = 30 + i*(gob.rect.width+4)
    gob.numClones = random.getrandbits(2) + 2
    gob.rect.y = screenH - 5 - (gob.numClones*gob.frames[1].get_rect().height) - gob.frames[0].get_rect().height

    all_sprites.add(gob)
    all_rockpiles.add(gob)



# draw screen once.
screen.fill(0)
all_rockpiles.draw(screen)
pygame.display.update(False)
pygame.display.update(False)

# Init sprites

# Create ship
shipGob = gameclass.GameObject([gamedata.shipF1Surf], [[0,0]], screen, screenW, screenH)
shipGob.rect.x = 50
shipGob.rect.y = 3
shipGob.animDur = 0;
shipGob.disableAfterOutOfScreen = False
all_sprites.add(shipGob)
all_gobs.add(shipGob)
shipGob.setSprite(0, [[60683,32963,49572,65502]])

# Create bomb
bombGob = gameclass.GameObject([gamedata.bombSurf], [[0,0]], screen, screenW, screenH)
bombGob.rect.x = 999
bombGob.rect.y = 999
all_sprites.add(bombGob)
all_gobs.add(bombGob)
bombGob.setSprite(1, [gamedata.bombColorMap])

# Create explosion
explosionGob = gameclass.GameObject(
    [gamedata.explosion1f1Surf, gamedata.explosion1f2Surf, gamedata.explosion1f3Surf, gamedata.explosion1f4Surf],
    [[0,0],[0,0],[0,0],[0,0]], screen, screenW, screenH
)
explosionGob.rect.x = 999
explosionGob.rect.y = 999
all_sprites.add(explosionGob)
all_gobs.add(explosionGob)
explosionGob.setSprite(2, [gamedata.explosion1ColorMap, gamedata.explosion1ColorMap, gamedata.explosion1ColorMap, gamedata.explosion1ColorMap])
explosionGob.shouldCycleAnim = False;
explosionGob.callAfterAnimCycle = HideGobAfterAnimCycle
explosionGob.visible = False

# Create explosion 2
explosionGob2 = gameclass.GameObject(
    [gamedata.explosion1f1Surf, gamedata.explosion1f2Surf, gamedata.explosion1f3Surf, gamedata.explosion1f4Surf],
    [[0,0],[0,0],[0,0],[0,0]], screen, screenW, screenH
)
explosionGob2.rect.x = 999
explosionGob2.rect.y = 999
all_sprites.add(explosionGob2)
all_gobs.add(explosionGob2)
explosionGob2.setSprite(3, [gamedata.explosion1ColorMap, gamedata.explosion1ColorMap, gamedata.explosion1ColorMap, gamedata.explosion1ColorMap])
explosionGob2.shouldCycleAnim = False;
explosionGob2.callAfterAnimCycle = HideGobAfterAnimCycle
explosionGob2.visible = False

# Create explosion 3
explosionGob3 = gameclass.GameObject(
    [gamedata.explosion1f1Surf, gamedata.explosion1f2Surf, gamedata.explosion1f3Surf, gamedata.explosion1f4Surf],
    [[0,0],[0,0],[0,0],[0,0]], screen, screenW, screenH
)
explosionGob3.rect.x = 999
explosionGob3.rect.y = 999
all_sprites.add(explosionGob3)
all_gobs.add(explosionGob3)
explosionGob3.setSprite(4, [gamedata.explosion1ColorMap, gamedata.explosion1ColorMap, gamedata.explosion1ColorMap, gamedata.explosion1ColorMap])
explosionGob3.shouldCycleAnim = False;
explosionGob3.callAfterAnimCycle = HideGobAfterAnimCycle
explosionGob3.visible = False

vx = 3;
vy = 0;
while True:

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
                bombGob.rect.x = shipGob.rect.x + ((shipGob.rect.width- bombGob.rect.width)//2)
                bombGob.rect.y = shipGob.rect.y + shipGob.rect.height
                bombGob.setvel(0,2)
                bombGob.visible = True

        if eventtype.type == pygame.KEYUP:
            if eventtype.key == pygame.K_RIGHT:
                vx = 0
            if eventtype.key == pygame.K_LEFT:
                vx = 0
            if eventtype.key == pygame.K_UP:
                vy = 0
            if eventtype.key == pygame.K_DOWN:
                vy = 0
    #vx = 3
    shipGob.setvel(vx,vy)

    # Check bomb collision
    hitSprite = sprite.spritecollideany(bombGob, all_rockpiles)
    if hitSprite != None:
        # setup explosion
        explosionGob.rect.x = hitSprite.rect.x
        explosionGob.rect.y = hitSprite.rect.y
        explosionGob.vx = 0
        explosionGob.vy = 0
        explosionGob.animDur = 1
        explosionGob.visible = True
        explosionGob.currentFrameNum = 0;
        explosionGob.setSpriteFrame()

        # Calc dirty rect
        totalH =  hitSprite.rect.height + (hitSprite.numClones * hitSprite.frames[1].get_rect().height)
        dirtyRect = pygame.Rect(hitSprite.rect.x, hitSprite.rect.y, hitSprite.rect.width, totalH)

        if hitSprite.numClones > 1:
            # Reduce pile height by one rock
            InitRockPile(hitSprite, hitSprite.numClones-1)
        else:
            # Hide pile as there is only one rock left
            hitSprite.rect.y = 999

        # Redraw background and piles
        screen.fill(0, dirtyRect)
        all_rockpiles.update()
        screen.set_clip(dirtyRect);
        all_rockpiles.draw(screen)
        screen.set_clip();

        # Update dirty rect
        #print('rect=', updateRect)
        pygame.display.update(False, dirtyRect)

    # Check ship collision
    hitSprite = sprite.spritecollideany(shipGob, all_rockpiles)
    if hitSprite != None:
        # setup explosion 2
        explosionGob2.rect.x = shipGob.rect.x
        explosionGob2.rect.y = shipGob.rect.y
        explosionGob2.animDur = 3
        explosionGob2.visible = True
        explosionGob2.currentFrameNum = 0;
        explosionGob2.setSpriteFrame()

        # setup explosion 3
        explosionGob3.rect.x = shipGob.rect.x + explosionGob2.rect.width
        explosionGob3.rect.y = shipGob.rect.y
        explosionGob3.animDur = 3
        explosionGob3.visible = True
        explosionGob3.currentFrameNum = 0;
        explosionGob3.setSpriteFrame()

        # Hide ship
        shipGob.vy = 999
        shipGob.disableAfterOutOfScreen = True

    ### Update and Draw

    # update all gobs
    all_sprites.update()

    # go round
    if shipGob.rect.x > screenW:
        shipGob.rect.x = -shipGob.rect.width
        shipGob.rect.y += 14

    # Win ?
    if (shipGob.rect.y > (screenH-25)
            and shipGob.rect.x > 100):
        vx = 0  # land here!

    # Draw gobs
    all_gobs.draw(screen)

    # Draw screen
    pygame.display.update(True)
