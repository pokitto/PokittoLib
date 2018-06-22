
// map header: winTile
//             mapWidth,mapHeight
//             map
//             playerX,playerY.....
//             MAPFLAGENEMY,speed,pointx,pointy,pointx,pointy,pointx,pointy.....
//             MAPFLAGEND
const byte gamemap1[] PROGMEM = {3,
                            18,6,
                            2,2,2,0,0,0,0,0,0,0,0,0,0,1,1,3,3,3,
                            2,2,2,0,1,1,1,1,1,1,1,1,1,1,0,3,3,3,
                            2,2,2,0,1,1,1,1,1,1,1,1,1,1,0,3,3,3,
                            2,2,2,0,1,1,1,1,1,1,1,1,1,1,0,3,3,3,
                            2,2,2,0,1,1,1,1,1,1,1,1,1,1,0,3,3,3,
                            2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,3,3,3,
                            
                            TILETOPX(1),TILETOPX(2) + (MAPTILESIZE / 2),
                            
                            //Enemy Data
                            MAPFLAGENEMY,2,TILETOPX(13),TILETOPX(1),TILETOPX(4),TILETOPX(1),
                            MAPFLAGENEMY,2,TILETOPX(4),TILETOPX(2),TILETOPX(13),TILETOPX(2),
                            MAPFLAGENEMY,2,TILETOPX(13),TILETOPX(3),TILETOPX(4),TILETOPX(3),
                            MAPFLAGENEMY,2,TILETOPX(4),TILETOPX(4),TILETOPX(13),TILETOPX(4),
                            
                            MAPFLAGEND};

const byte gamemap2[] PROGMEM = {3,
                            18,6,
                            0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
                            0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
                            2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,
                            2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,
                            0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
                            0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
                            
                            TILETOPX(1),TILETOPX(2) + (MAPTILESIZE / 2),
                            
                            MAPFLAGENEMY,2,TILETOPX(3),TILETOPX(0),TILETOPX(3),TILETOPX(5),
                            MAPFLAGENEMY,2,TILETOPX(5),TILETOPX(0),TILETOPX(5),TILETOPX(5),
                            MAPFLAGENEMY,2,TILETOPX(7),TILETOPX(0),TILETOPX(7),TILETOPX(5),
                            MAPFLAGENEMY,2,TILETOPX(9),TILETOPX(0),TILETOPX(9),TILETOPX(5),
                            MAPFLAGENEMY,2,TILETOPX(11),TILETOPX(0),TILETOPX(11),TILETOPX(5),
                            MAPFLAGENEMY,2,TILETOPX(13),TILETOPX(0),TILETOPX(13),TILETOPX(5),
                            MAPFLAGENEMY,2,TILETOPX(4),TILETOPX(5),TILETOPX(4),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(6),TILETOPX(5),TILETOPX(6),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(8),TILETOPX(5),TILETOPX(8),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(10),TILETOPX(5),TILETOPX(10),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(12),TILETOPX(5),TILETOPX(12),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(14),TILETOPX(5),TILETOPX(14),TILETOPX(0),
                            MAPFLAGCOIN,TILETOPX(8) + (MAPTILESIZE / 2),TILETOPX(2) + (MAPTILESIZE / 2),
                            MAPFLAGEND};

const byte gamemap3[] PROGMEM = {2,
                            4,5,
                            1,0,0,0,
                            1,1,1,1,
                            1,2,2,1,
                            1,2,2,1,
                            1,1,1,1,
                            
                            TILETOPX(1) + (MAPTILESIZE / 2),TILETOPX(2) + (MAPTILESIZE / 2),
                            
                            MAPFLAGENEMY,2,                          TILETOPX(0),TILETOPX(1),TILETOPX(3),TILETOPX(1),TILETOPX(3),TILETOPX(4),TILETOPX(0),TILETOPX(4),
                            MAPFLAGENEMY,2,TILETOPX(1),TILETOPX(1),  TILETOPX(3),TILETOPX(1),TILETOPX(3),TILETOPX(4),TILETOPX(0),TILETOPX(4),TILETOPX(0),TILETOPX(1),
                            MAPFLAGENEMY,2,TILETOPX(2)-2,TILETOPX(1),TILETOPX(3),TILETOPX(1),TILETOPX(3),TILETOPX(4),TILETOPX(0),TILETOPX(4),TILETOPX(0),TILETOPX(1),
                            
                            MAPFLAGENEMY,2,TILETOPX(3),TILETOPX(2),TILETOPX(3),TILETOPX(4),TILETOPX(0),TILETOPX(4),TILETOPX(0),TILETOPX(1),TILETOPX(3),TILETOPX(1),
                            MAPFLAGENEMY,2,TILETOPX(3),TILETOPX(3),TILETOPX(3),TILETOPX(4),TILETOPX(0),TILETOPX(4),TILETOPX(0),TILETOPX(1),TILETOPX(3),TILETOPX(1),
                            MAPFLAGENEMY,2,                        TILETOPX(3),TILETOPX(4),TILETOPX(0),TILETOPX(4),TILETOPX(0),TILETOPX(1),TILETOPX(3),TILETOPX(1),
                            
                            MAPFLAGENEMY,2,TILETOPX(2),TILETOPX(4),TILETOPX(0),TILETOPX(4),TILETOPX(0),TILETOPX(1),TILETOPX(3),TILETOPX(1),TILETOPX(3),TILETOPX(4),
                            MAPFLAGENEMY,2,TILETOPX(1),TILETOPX(4),TILETOPX(0),TILETOPX(4),TILETOPX(0),TILETOPX(1),TILETOPX(3),TILETOPX(1),TILETOPX(3),TILETOPX(4),
                            MAPFLAGENEMY,2,                        TILETOPX(0),TILETOPX(4),TILETOPX(0),TILETOPX(1),TILETOPX(3),TILETOPX(1),TILETOPX(3),TILETOPX(4),
                            
                            MAPFLAGENEMY,2,TILETOPX(0),TILETOPX(3),TILETOPX(0),TILETOPX(1),TILETOPX(3),TILETOPX(1),TILETOPX(3),TILETOPX(4),TILETOPX(0),TILETOPX(4),
                            MAPFLAGENEMY,2,TILETOPX(0),TILETOPX(2),TILETOPX(0),TILETOPX(1),TILETOPX(3),TILETOPX(1),TILETOPX(3),TILETOPX(4),TILETOPX(0),TILETOPX(4),
                            
                            MAPFLAGCOIN,TILETOPX(0),TILETOPX(0),
                            
                            MAPFLAGEND};

const byte gamemap4[] PROGMEM = {3,
                            18,8,
                            0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
                            0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
                            0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
                            2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,
                            2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,
                            0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
                            0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
                            0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
                            
                            TILETOPX(1),TILETOPX(3) + (MAPTILESIZE / 2),
                            
                            MAPFLAGENEMY,5,TILETOPX(3),TILETOPX(0),TILETOPX(3),TILETOPX(7),
                            MAPFLAGENEMY,5,TILETOPX(5),TILETOPX(0),TILETOPX(5),TILETOPX(7),
                            MAPFLAGENEMY,5,TILETOPX(7),TILETOPX(0),TILETOPX(7),TILETOPX(7),
                            MAPFLAGENEMY,5,TILETOPX(9),TILETOPX(0),TILETOPX(9),TILETOPX(7),
                            MAPFLAGENEMY,5,TILETOPX(11),TILETOPX(0),TILETOPX(11),TILETOPX(7),
                            MAPFLAGENEMY,5,TILETOPX(13),TILETOPX(0),TILETOPX(13),TILETOPX(7),
                            MAPFLAGENEMY,5,TILETOPX(4),TILETOPX(7),TILETOPX(4),TILETOPX(0),
                            MAPFLAGENEMY,5,TILETOPX(6),TILETOPX(7),TILETOPX(6),TILETOPX(0),
                            MAPFLAGENEMY,5,TILETOPX(8),TILETOPX(7),TILETOPX(8),TILETOPX(0),
                            MAPFLAGENEMY,5,TILETOPX(10),TILETOPX(7),TILETOPX(10),TILETOPX(0),
                            MAPFLAGENEMY,5,TILETOPX(12),TILETOPX(7),TILETOPX(12),TILETOPX(0),
                            MAPFLAGENEMY,5,TILETOPX(14),TILETOPX(7),TILETOPX(14),TILETOPX(0),
                            MAPFLAGCOIN,TILETOPX(3),TILETOPX(0),
                            MAPFLAGCOIN,TILETOPX(14),TILETOPX(0),
                            MAPFLAGCOIN,TILETOPX(3),TILETOPX(7),
                            MAPFLAGCOIN,TILETOPX(14),TILETOPX(7),
                            MAPFLAGEND};

const byte gamemap5[] PROGMEM = {3,
                            12,10,
                            1,1,1,1,0,0,1,1,1,1,0,0,
                            1,2,0,1,1,1,1,0,0,1,0,0,
                            1,0,0,1,0,0,1,0,0,1,0,0,
                            1,1,1,1,0,0,1,1,1,1,0,0,
                            1,0,0,1,0,0,1,0,0,1,3,3,
                            1,0,0,1,0,0,1,0,0,1,3,3,
                            1,1,1,1,0,0,1,1,1,1,0,0,
                            1,0,0,1,0,0,1,0,0,1,0,0,
                            1,0,0,1,1,1,1,0,0,1,0,0,
                            1,1,1,1,0,0,1,1,1,1,0,0,
                            
                            TILETOPX(1),TILETOPX(1),
                            
                            MAPFLAGENEMY,2,TILETOPX(3),TILETOPX(0),TILETOPX(3),TILETOPX(3),TILETOPX(0),TILETOPX(3),TILETOPX(0),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(3),TILETOPX(3),TILETOPX(3),TILETOPX(6),TILETOPX(0),TILETOPX(6),TILETOPX(0),TILETOPX(3),
                            MAPFLAGENEMY,2,TILETOPX(3),TILETOPX(6),TILETOPX(3),TILETOPX(9),TILETOPX(0),TILETOPX(9),TILETOPX(0),TILETOPX(6),
                            
                            MAPFLAGENEMY,2,TILETOPX(6),TILETOPX(0),TILETOPX(6),TILETOPX(3),TILETOPX(9),TILETOPX(3),TILETOPX(9),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(6),TILETOPX(3),TILETOPX(6),TILETOPX(6),TILETOPX(9),TILETOPX(6),TILETOPX(9),TILETOPX(3),
                            MAPFLAGENEMY,2,TILETOPX(6),TILETOPX(6),TILETOPX(6),TILETOPX(9),TILETOPX(9),TILETOPX(9),TILETOPX(9),TILETOPX(6),
                            
                            MAPFLAGENEMY,2,TILETOPX(3),TILETOPX(1),TILETOPX(6),TILETOPX(1),TILETOPX(6),TILETOPX(8),TILETOPX(3),TILETOPX(8),
                            
                            MAPFLAGCOIN,TILETOPX(0),TILETOPX(9),
                            MAPFLAGCOIN,TILETOPX(9),TILETOPX(0),
                            MAPFLAGCOIN,TILETOPX(9),TILETOPX(9),
                            MAPFLAGEND};

const byte gamemap6[] PROGMEM = {4,
                            18,10,
                            2,2,0,0,1,1,1,1,1,1,0,0,1,1,1,1,1,1,
                            2,2,0,0,1,1,1,1,1,1,0,0,1,1,1,1,1,1,
                            1,1,1,1,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
                            1,1,1,1,1,1,0,0,1,1,0,0,1,1,0,0,1,1,
                            1,1,0,0,0,0,0,0,1,1,0,0,1,1,0,0,4,4,
                            1,1,0,0,0,0,0,0,1,1,0,0,1,1,0,0,4,4,
                            1,1,0,0,1,1,1,1,3,3,1,1,1,1,0,0,0,0,
                            1,1,0,0,1,1,1,1,3,3,1,1,1,1,0,0,0,0,
                            1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,
                            1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,
                            
                            TILETOPX(0) + (MAPTILESIZE / 2),TILETOPX(0) + (MAPTILESIZE / 2), TILETOPX(8) + (MAPTILESIZE / 2),TILETOPX(6) + (MAPTILESIZE / 2),
                            
                            
                            MAPFLAGENEMY,2,TILETOPX(0),TILETOPX(2),TILETOPX(1),TILETOPX(2),TILETOPX(1),TILETOPX(3),TILETOPX(0),TILETOPX(3),
                            MAPFLAGENEMY,0,TILETOPX(1),TILETOPX(4) + (MAPTILESIZE / 2),
                            MAPFLAGENEMY,0,TILETOPX(0),TILETOPX(6) + (MAPTILESIZE / 2),
                            MAPFLAGENEMY,2,TILETOPX(1),TILETOPX(9),TILETOPX(0),TILETOPX(9),TILETOPX(0),TILETOPX(8),TILETOPX(1),TILETOPX(8),
                            
                            MAPFLAGENEMY,0,TILETOPX(2) + (MAPTILESIZE / 2),TILETOPX(2),
                            MAPFLAGENEMY,0,TILETOPX(2) + (MAPTILESIZE / 2),TILETOPX(8),
                            
                            MAPFLAGENEMY,0,TILETOPX(4),TILETOPX(0) + (MAPTILESIZE / 2),
                            MAPFLAGENEMY,2,TILETOPX(5),TILETOPX(3),TILETOPX(4),TILETOPX(3),TILETOPX(4),TILETOPX(2),TILETOPX(5),TILETOPX(2),
                            MAPFLAGENEMY,0,TILETOPX(4),TILETOPX(6) + (MAPTILESIZE / 2),
                            MAPFLAGENEMY,2,TILETOPX(5),TILETOPX(9),TILETOPX(4),TILETOPX(9),TILETOPX(4),TILETOPX(8),TILETOPX(5),TILETOPX(8),
                            
                            MAPFLAGENEMY,0,TILETOPX(6) + (MAPTILESIZE / 2),TILETOPX(1),
                            MAPFLAGENEMY,0,TILETOPX(5) + (MAPTILESIZE / 2),TILETOPX(6),
                            MAPFLAGENEMY,0,TILETOPX(7),TILETOPX(7),
                            
                            MAPFLAGENEMY,2,TILETOPX(9),TILETOPX(1),TILETOPX(8),TILETOPX(1),TILETOPX(8),TILETOPX(0),TILETOPX(9),TILETOPX(0),
                            MAPFLAGENEMY,0,TILETOPX(8),TILETOPX(2) + (MAPTILESIZE / 2),
                            MAPFLAGENEMY,2,TILETOPX(8),TILETOPX(5),TILETOPX(9),TILETOPX(5),TILETOPX(9),TILETOPX(2) + (MAPTILESIZE / 2),TILETOPX(9),TILETOPX(5),
                            
                            MAPFLAGENEMY,2,TILETOPX(12),TILETOPX(0),TILETOPX(13),TILETOPX(0),TILETOPX(13),TILETOPX(1),TILETOPX(12),TILETOPX(1),
                            MAPFLAGENEMY,0,TILETOPX(13),TILETOPX(2) + (MAPTILESIZE / 2),
                            MAPFLAGENEMY,0,TILETOPX(12),TILETOPX(4) + (MAPTILESIZE / 2),
                            MAPFLAGENEMY,2,TILETOPX(12),TILETOPX(6),TILETOPX(13),TILETOPX(6),TILETOPX(13),TILETOPX(7),TILETOPX(12),TILETOPX(7),
                            MAPFLAGENEMY,0,TILETOPX(12),TILETOPX(8) + (MAPTILESIZE / 2),
                            
                            MAPFLAGENEMY,0,TILETOPX(14) + (MAPTILESIZE / 2),TILETOPX(1),
                            MAPFLAGENEMY,2,TILETOPX(13),TILETOPX(8),TILETOPX(15) + (MAPTILESIZE / 2),TILETOPX(8),TILETOPX(15) + (MAPTILESIZE / 2),TILETOPX(9),TILETOPX(15) + (MAPTILESIZE / 2),TILETOPX(8),
                            
                            MAPFLAGENEMY,2,TILETOPX(17),TILETOPX(1),TILETOPX(16),TILETOPX(1),TILETOPX(16),TILETOPX(0),TILETOPX(17),TILETOPX(0),
                            MAPFLAGENEMY,0,TILETOPX(16),TILETOPX(2) + (MAPTILESIZE / 2),
                            
                            MAPFLAGCOIN,TILETOPX(16) + (MAPTILESIZE / 2),TILETOPX(8) + (MAPTILESIZE / 2),
                            
                            MAPFLAGEND};

const byte gamemap7[] PROGMEM = {3,
                            9,10,
                            0,0,2,2,2,0,3,3,3,
                            0,0,2,2,2,0,3,3,3,
                            0,0,1,0,0,0,1,0,0,
                            0,0,1,1,0,1,1,0,0,
                            0,0,1,1,0,1,1,0,0,
                            0,0,1,1,0,1,1,0,0,
                            1,1,1,1,0,1,1,1,1,
                            1,1,0,0,0,0,0,1,1,
                            1,1,1,1,1,1,1,1,1,
                            0,0,1,1,1,1,1,0,0,
                            
                            TILETOPX(3),TILETOPX(0) + (MAPTILESIZE / 2),
                            
                            MAPFLAGENEMY,1,TILETOPX(2),TILETOPX(3),TILETOPX(3)+1,TILETOPX(3),
                            MAPFLAGENEMY,1,TILETOPX(3)+1,TILETOPX(4),TILETOPX(2),TILETOPX(4),
                            MAPFLAGENEMY,1,TILETOPX(2),TILETOPX(5),TILETOPX(3)+1,TILETOPX(5),
                            MAPFLAGENEMY,1,TILETOPX(3)+1,TILETOPX(6),TILETOPX(2),TILETOPX(6),
                            
                            MAPFLAGENEMY,1,TILETOPX(1),TILETOPX(6),TILETOPX(0)-1,TILETOPX(6),
                            MAPFLAGENEMY,1,TILETOPX(0)-1,TILETOPX(7),TILETOPX(1),TILETOPX(7),
                            MAPFLAGENEMY,1,TILETOPX(1),TILETOPX(8),TILETOPX(0)-1,TILETOPX(8),
                            
                            MAPFLAGENEMY,1,TILETOPX(2),TILETOPX(8),TILETOPX(2),TILETOPX(9)+1,
                            MAPFLAGENEMY,1,TILETOPX(3),TILETOPX(9)+1,TILETOPX(3),TILETOPX(8),
                            MAPFLAGENEMY,1,TILETOPX(4),TILETOPX(8),TILETOPX(4),TILETOPX(9)+1,
                            MAPFLAGENEMY,1,TILETOPX(5),TILETOPX(9)+1,TILETOPX(5),TILETOPX(8),
                            MAPFLAGENEMY,1,TILETOPX(6),TILETOPX(8),TILETOPX(6),TILETOPX(9)+1,
                            
                            MAPFLAGENEMY,1,TILETOPX(7),TILETOPX(8),TILETOPX(8)+1,TILETOPX(8),
                            MAPFLAGENEMY,1,TILETOPX(8)+1,TILETOPX(7),TILETOPX(7),TILETOPX(7),
                            MAPFLAGENEMY,1,TILETOPX(7),TILETOPX(6),TILETOPX(8)+1,TILETOPX(6),
                            
                            
                            MAPFLAGENEMY,1,TILETOPX(5)-1,TILETOPX(6),TILETOPX(6),TILETOPX(6),
                            MAPFLAGENEMY,1,TILETOPX(6),TILETOPX(5),TILETOPX(5)-1,TILETOPX(5),
                            MAPFLAGENEMY,1,TILETOPX(5)-1,TILETOPX(4),TILETOPX(6),TILETOPX(4),
                            MAPFLAGENEMY,1,TILETOPX(6),TILETOPX(3),TILETOPX(5)-1,TILETOPX(3),
                            
                            MAPFLAGEND};

const byte gamemap8[] PROGMEM = {3,
                            10,10,
                            0,0,0,0,3,3,0,0,0,0,
                            0,0,0,0,3,3,0,0,0,0,
                            1,1,1,1,1,1,1,1,1,1,
                            1,1,1,1,1,1,1,1,1,1,
                            1,1,1,1,1,1,1,1,1,1,
                            1,1,1,1,1,1,1,1,1,1,
                            1,1,1,1,1,1,1,1,1,1,
                            1,1,1,1,1,1,1,1,1,1,
                            0,0,0,0,2,2,0,0,0,0,
                            0,0,0,0,2,2,0,0,0,0,
                            
                            TILETOPX(4) + (MAPTILESIZE / 2),TILETOPX(8) + (MAPTILESIZE / 2),
                            
                            MAPFLAGENEMY,3,TILETOPX(0),TILETOPX(2),TILETOPX(0),TILETOPX(7),
                            MAPFLAGENEMY,3,TILETOPX(1),TILETOPX(7),TILETOPX(1),TILETOPX(2),
                            MAPFLAGENEMY,3,TILETOPX(2),TILETOPX(2),TILETOPX(2),TILETOPX(7),
                            MAPFLAGENEMY,3,TILETOPX(3),TILETOPX(7),TILETOPX(3),TILETOPX(2),
                            MAPFLAGENEMY,3,TILETOPX(4),TILETOPX(2),TILETOPX(4),TILETOPX(7),
                            MAPFLAGENEMY,3,TILETOPX(5),TILETOPX(7),TILETOPX(5),TILETOPX(2),
                            MAPFLAGENEMY,3,TILETOPX(6),TILETOPX(2),TILETOPX(6),TILETOPX(7),
                            MAPFLAGENEMY,3,TILETOPX(7),TILETOPX(7),TILETOPX(7),TILETOPX(2),
                            MAPFLAGENEMY,3,TILETOPX(8),TILETOPX(2),TILETOPX(8),TILETOPX(7),
                            MAPFLAGENEMY,3,TILETOPX(9),TILETOPX(7),TILETOPX(9),TILETOPX(2),
                            
                            MAPFLAGENEMY,3,TILETOPX(0),TILETOPX(4),TILETOPX(9),TILETOPX(4),
                            MAPFLAGENEMY,3,TILETOPX(9),TILETOPX(5),TILETOPX(0),TILETOPX(5),
                            
                            MAPFLAGEND};

const byte gamemap9[] PROGMEM = {3,
                            20,10,
                            2,2,2,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,
                            2,2,2,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,
                            1,0,0,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,
                            1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,0,1,1,1,
                            1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,0,1,1,1,
                            1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,0,1,1,1,
                            1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,0,1,1,1,
                            1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,0,0,0,1,
                            1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,0,3,3,3,
                            1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,0,3,3,3,
                            
                            TILETOPX(1),TILETOPX(0) + (MAPTILESIZE / 2),
                            
                            MAPFLAGENEMY,3,TILETOPX(0),TILETOPX(9),TILETOPX(0),TILETOPX(3),
                            MAPFLAGENEMY,3,TILETOPX(1),TILETOPX(3),TILETOPX(1),TILETOPX(9),
                            MAPFLAGENEMY,3,TILETOPX(2),TILETOPX(9),TILETOPX(2),TILETOPX(3),
                            
                            MAPFLAGENEMY,4,TILETOPX(3),TILETOPX(9),TILETOPX(3),TILETOPX(7),
                            
                            MAPFLAGENEMY,3,TILETOPX(4),TILETOPX(9),TILETOPX(4),TILETOPX(0),
                            MAPFLAGENEMY,3,TILETOPX(5),TILETOPX(0),TILETOPX(5),TILETOPX(9),
                            MAPFLAGENEMY,3,TILETOPX(6),TILETOPX(9),TILETOPX(6),TILETOPX(0),
                            
                            MAPFLAGENEMY,4,TILETOPX(7),TILETOPX(0),TILETOPX(7),TILETOPX(2),
                            
                            MAPFLAGENEMY,3,TILETOPX(8),TILETOPX(9),TILETOPX(8),TILETOPX(0),
                            MAPFLAGENEMY,3,TILETOPX(9),TILETOPX(0),TILETOPX(9),TILETOPX(9),
                            MAPFLAGENEMY,3,TILETOPX(10),TILETOPX(9),TILETOPX(10),TILETOPX(0),
                            
                            MAPFLAGENEMY,4,TILETOPX(11),TILETOPX(9),TILETOPX(11),TILETOPX(7),
                            
                            MAPFLAGENEMY,3,TILETOPX(12),TILETOPX(9),TILETOPX(12),TILETOPX(0),
                            MAPFLAGENEMY,3,TILETOPX(13),TILETOPX(0),TILETOPX(13),TILETOPX(9),
                            MAPFLAGENEMY,3,TILETOPX(14),TILETOPX(9),TILETOPX(14),TILETOPX(0),
                            
                            MAPFLAGENEMY,4,TILETOPX(15),TILETOPX(0),TILETOPX(15),TILETOPX(2),
                            MAPFLAGENEMY,4,TILETOPX(16),TILETOPX(2),TILETOPX(16),TILETOPX(0),
                            
                            MAPFLAGENEMY,3,TILETOPX(17),TILETOPX(0),TILETOPX(17),TILETOPX(6),
                            MAPFLAGENEMY,3,TILETOPX(18),TILETOPX(6),TILETOPX(18),TILETOPX(0),
                            MAPFLAGENEMY,3,TILETOPX(19),TILETOPX(0),TILETOPX(19),TILETOPX(6),
                            
                            MAPFLAGEND};

const byte gamemap10[] PROGMEM = {3,
                            20,6,
                            0,0,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,0,0,
                            0,0,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,0,0,
                            2,2,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,3,3,
                            2,2,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,3,3,
                            0,0,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,0,0,
                            0,0,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,0,0,
                            
                            TILETOPX(0) + (MAPTILESIZE / 2),TILETOPX(2) + (MAPTILESIZE / 2),
                            
                            MAPFLAGENEMY,1,TILETOPX(2),TILETOPX(0),TILETOPX(3),TILETOPX(0),TILETOPX(3),TILETOPX(1),TILETOPX(2),TILETOPX(1),
                            MAPFLAGENEMY,1,TILETOPX(4),TILETOPX(0),TILETOPX(5),TILETOPX(0),TILETOPX(5),TILETOPX(1),TILETOPX(4),TILETOPX(1),
                            MAPFLAGENEMY,1,TILETOPX(2),TILETOPX(2),TILETOPX(3),TILETOPX(2),TILETOPX(3),TILETOPX(3),TILETOPX(2),TILETOPX(3),
                            MAPFLAGENEMY,1,TILETOPX(4),TILETOPX(2),TILETOPX(5),TILETOPX(2),TILETOPX(5),TILETOPX(3),TILETOPX(4),TILETOPX(3),
                            MAPFLAGENEMY,1,TILETOPX(2),TILETOPX(4),TILETOPX(3),TILETOPX(4),TILETOPX(3),TILETOPX(5),TILETOPX(2),TILETOPX(5),
                            MAPFLAGENEMY,1,TILETOPX(4),TILETOPX(4),TILETOPX(5),TILETOPX(4),TILETOPX(5),TILETOPX(5),TILETOPX(4),TILETOPX(5),
                            
                            MAPFLAGENEMY,1,TILETOPX(6),TILETOPX(4),TILETOPX(7),TILETOPX(4),TILETOPX(7),TILETOPX(5),TILETOPX(6),TILETOPX(5),
                            
                            MAPFLAGENEMY,1,TILETOPX(8),TILETOPX(0),TILETOPX(9),TILETOPX(0),TILETOPX(9),TILETOPX(1),TILETOPX(8),TILETOPX(1),
                            MAPFLAGENEMY,1,TILETOPX(10),TILETOPX(0),TILETOPX(11),TILETOPX(0),TILETOPX(11),TILETOPX(1),TILETOPX(10),TILETOPX(1),
                            MAPFLAGENEMY,1,TILETOPX(8),TILETOPX(2),TILETOPX(9),TILETOPX(2),TILETOPX(9),TILETOPX(3),TILETOPX(8),TILETOPX(3),
                            MAPFLAGENEMY,1,TILETOPX(10),TILETOPX(2),TILETOPX(11),TILETOPX(2),TILETOPX(11),TILETOPX(3),TILETOPX(10),TILETOPX(3),
                            MAPFLAGENEMY,1,TILETOPX(8),TILETOPX(4),TILETOPX(9),TILETOPX(4),TILETOPX(9),TILETOPX(5),TILETOPX(8),TILETOPX(5),
                            MAPFLAGENEMY,1,TILETOPX(10),TILETOPX(4),TILETOPX(11),TILETOPX(4),TILETOPX(11),TILETOPX(5),TILETOPX(10),TILETOPX(5),
                            
                            MAPFLAGENEMY,1,TILETOPX(12),TILETOPX(0),TILETOPX(13),TILETOPX(0),TILETOPX(13),TILETOPX(1),TILETOPX(12),TILETOPX(1),
                            
                            MAPFLAGENEMY,1,TILETOPX(14),TILETOPX(0),TILETOPX(15),TILETOPX(0),TILETOPX(15),TILETOPX(1),TILETOPX(14),TILETOPX(1),
                            MAPFLAGENEMY,1,TILETOPX(16),TILETOPX(0),TILETOPX(17),TILETOPX(0),TILETOPX(17),TILETOPX(1),TILETOPX(16),TILETOPX(1),
                            MAPFLAGENEMY,1,TILETOPX(14),TILETOPX(2),TILETOPX(15),TILETOPX(2),TILETOPX(15),TILETOPX(3),TILETOPX(14),TILETOPX(3),
                            MAPFLAGENEMY,1,TILETOPX(16),TILETOPX(2),TILETOPX(17),TILETOPX(2),TILETOPX(17),TILETOPX(3),TILETOPX(16),TILETOPX(3),
                            MAPFLAGENEMY,1,TILETOPX(14),TILETOPX(4),TILETOPX(15),TILETOPX(4),TILETOPX(15),TILETOPX(5),TILETOPX(14),TILETOPX(5),
                            MAPFLAGENEMY,1,TILETOPX(16),TILETOPX(4),TILETOPX(17),TILETOPX(4),TILETOPX(17),TILETOPX(5),TILETOPX(16),TILETOPX(5),
                            
                            MAPFLAGCOIN,TILETOPX(5),TILETOPX(0),
                            MAPFLAGCOIN,TILETOPX(8),TILETOPX(0),
                            MAPFLAGCOIN,TILETOPX(11),TILETOPX(5),
                            MAPFLAGCOIN,TILETOPX(14),TILETOPX(5),
                            
                            MAPFLAGEND};

const byte gamemap11[] PROGMEM = {3,
                            16,12,
                            0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                            0,0,1,0,0,0,1,0,0,0,0,0,0,1,0,0,
                            0,0,1,0,1,1,1,1,1,1,1,1,1,1,0,0,
                            0,0,1,0,0,0,1,0,0,0,0,0,0,1,0,0,
                            0,0,1,0,0,0,1,0,0,0,0,0,0,1,0,0,
                            0,0,1,0,1,1,1,1,1,1,1,3,1,1,0,0,
                            0,0,1,0,0,0,1,0,0,0,0,0,0,1,0,0,
                            1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,
                            0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,
                            
                            TILETOPX(1),TILETOPX(1),
                            
                            MAPFLAGENEMY,0,TILETOPX(3),TILETOPX(2),
                            MAPFLAGENEMY,0,TILETOPX(6),TILETOPX(3),
                            MAPFLAGENEMY,0,TILETOPX(6),TILETOPX(8),
                            MAPFLAGENEMY,0,TILETOPX(6),TILETOPX(10),
                            MAPFLAGENEMY,0,TILETOPX(12),TILETOPX(7),
                            
                            MAPFLAGENEMY,2,TILETOPX(4),TILETOPX(4),TILETOPX(13),TILETOPX(4),
                            MAPFLAGENEMY,2,TILETOPX(4),TILETOPX(7),TILETOPX(13),TILETOPX(7),
                            
                            MAPFLAGENEMY,2,TILETOPX(2),TILETOPX(0),TILETOPX(2),TILETOPX(9),
                            MAPFLAGENEMY,2,TILETOPX(13),TILETOPX(11),TILETOPX(13),TILETOPX(2),
                            
                            MAPFLAGENEMY,2,TILETOPX(0),TILETOPX(9),TILETOPX(13),TILETOPX(9),
                            MAPFLAGENEMY,2,TILETOPX(15),TILETOPX(2),TILETOPX(2),TILETOPX(2),
                            
                            MAPFLAGENEMY,2,TILETOPX(6),TILETOPX(9) + (MAPTILESIZE / 2),TILETOPX(6),TILETOPX(11),TILETOPX(6),TILETOPX(2),
                            
                            MAPFLAGEND};

const byte gamemap12[] PROGMEM = {3,
                            14,4,
                            0,0,1,1,1,1,1,1,1,1,1,1,0,0,
                            2,2,1,1,1,1,1,1,1,1,1,1,3,3,
                            2,2,1,1,1,1,1,1,1,1,1,1,3,3,
                            0,0,1,1,1,1,1,1,1,1,1,1,0,0,
                            
                            TILETOPX(0) + (MAPTILESIZE / 2),TILETOPX(1) + (MAPTILESIZE / 2),
                            
                            MAPFLAGENEMY,2,TILETOPX(2),TILETOPX(0),TILETOPX(2),TILETOPX(3),
                            MAPFLAGENEMY,2,TILETOPX(3),TILETOPX(3),TILETOPX(3),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(4),TILETOPX(0),TILETOPX(4),TILETOPX(3),
                            MAPFLAGENEMY,2,TILETOPX(5),TILETOPX(3),TILETOPX(5),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(6),TILETOPX(0),TILETOPX(6),TILETOPX(3),
                            MAPFLAGENEMY,2,TILETOPX(7),TILETOPX(3),TILETOPX(7),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(8),TILETOPX(0),TILETOPX(8),TILETOPX(3),
                            MAPFLAGENEMY,2,TILETOPX(9),TILETOPX(3),TILETOPX(9),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(10),TILETOPX(0),TILETOPX(10),TILETOPX(3),
                            MAPFLAGENEMY,2,TILETOPX(11),TILETOPX(3),TILETOPX(11),TILETOPX(0),
                            
                            MAPFLAGCOIN,TILETOPX( 2),TILETOPX(0),
                            MAPFLAGCOIN,TILETOPX( 3),TILETOPX(0),
                            MAPFLAGCOIN,TILETOPX( 4),TILETOPX(0),
                            MAPFLAGCOIN,TILETOPX( 5),TILETOPX(0),
                            MAPFLAGCOIN,TILETOPX( 6),TILETOPX(0),
                            MAPFLAGCOIN,TILETOPX( 7),TILETOPX(0),
                            MAPFLAGCOIN,TILETOPX( 8),TILETOPX(0),
                            MAPFLAGCOIN,TILETOPX( 9),TILETOPX(0),
                            MAPFLAGCOIN,TILETOPX(10),TILETOPX(0),
                            MAPFLAGCOIN,TILETOPX(11),TILETOPX(0),
                            
                            MAPFLAGCOIN,TILETOPX( 2),TILETOPX(1),
                            MAPFLAGCOIN,TILETOPX( 3),TILETOPX(1),
                            MAPFLAGCOIN,TILETOPX( 4),TILETOPX(1),
                            MAPFLAGCOIN,TILETOPX( 5),TILETOPX(1),
                            MAPFLAGCOIN,TILETOPX( 6),TILETOPX(1),
                            MAPFLAGCOIN,TILETOPX( 7),TILETOPX(1),
                            MAPFLAGCOIN,TILETOPX( 8),TILETOPX(1),
                            MAPFLAGCOIN,TILETOPX( 9),TILETOPX(1),
                            MAPFLAGCOIN,TILETOPX(10),TILETOPX(1),
                            MAPFLAGCOIN,TILETOPX(11),TILETOPX(1),
                            
                            MAPFLAGCOIN,TILETOPX( 2),TILETOPX(2),
                            MAPFLAGCOIN,TILETOPX( 3),TILETOPX(2),
                            MAPFLAGCOIN,TILETOPX( 4),TILETOPX(2),
                            MAPFLAGCOIN,TILETOPX( 5),TILETOPX(2),
                            MAPFLAGCOIN,TILETOPX( 6),TILETOPX(2),
                            MAPFLAGCOIN,TILETOPX( 7),TILETOPX(2),
                            MAPFLAGCOIN,TILETOPX( 8),TILETOPX(2),
                            MAPFLAGCOIN,TILETOPX( 9),TILETOPX(2),
                            MAPFLAGCOIN,TILETOPX(10),TILETOPX(2),
                            MAPFLAGCOIN,TILETOPX(11),TILETOPX(2),
                            
                            MAPFLAGCOIN,TILETOPX( 2),TILETOPX(3),
                            MAPFLAGCOIN,TILETOPX( 3),TILETOPX(3),
                            MAPFLAGCOIN,TILETOPX( 4),TILETOPX(3),
                            MAPFLAGCOIN,TILETOPX( 5),TILETOPX(3),
                            MAPFLAGCOIN,TILETOPX( 6),TILETOPX(3),
                            MAPFLAGCOIN,TILETOPX( 7),TILETOPX(3),
                            MAPFLAGCOIN,TILETOPX( 8),TILETOPX(3),
                            MAPFLAGCOIN,TILETOPX( 9),TILETOPX(3),
                            MAPFLAGCOIN,TILETOPX(10),TILETOPX(3),
                            MAPFLAGCOIN,TILETOPX(11),TILETOPX(3),
                            
                            MAPFLAGCOIN,TILETOPX( 2) + (MAPTILESIZE / 2),TILETOPX(0) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX( 3) + (MAPTILESIZE / 2),TILETOPX(0) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX( 4) + (MAPTILESIZE / 2),TILETOPX(0) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX( 5) + (MAPTILESIZE / 2),TILETOPX(0) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX( 6) + (MAPTILESIZE / 2),TILETOPX(0) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX( 7) + (MAPTILESIZE / 2),TILETOPX(0) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX( 8) + (MAPTILESIZE / 2),TILETOPX(0) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX( 9) + (MAPTILESIZE / 2),TILETOPX(0) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX(10) + (MAPTILESIZE / 2),TILETOPX(0) + (MAPTILESIZE / 2),
                            
                            MAPFLAGCOIN,TILETOPX( 2) + (MAPTILESIZE / 2),TILETOPX(1) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX( 3) + (MAPTILESIZE / 2),TILETOPX(1) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX( 4) + (MAPTILESIZE / 2),TILETOPX(1) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX( 5) + (MAPTILESIZE / 2),TILETOPX(1) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX( 6) + (MAPTILESIZE / 2),TILETOPX(1) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX( 7) + (MAPTILESIZE / 2),TILETOPX(1) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX( 8) + (MAPTILESIZE / 2),TILETOPX(1) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX( 9) + (MAPTILESIZE / 2),TILETOPX(1) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX(10) + (MAPTILESIZE / 2),TILETOPX(1) + (MAPTILESIZE / 2),
                            
                            MAPFLAGCOIN,TILETOPX( 2) + (MAPTILESIZE / 2),TILETOPX(2) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX( 3) + (MAPTILESIZE / 2),TILETOPX(2) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX( 4) + (MAPTILESIZE / 2),TILETOPX(2) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX( 5) + (MAPTILESIZE / 2),TILETOPX(2) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX( 6) + (MAPTILESIZE / 2),TILETOPX(2) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX( 7) + (MAPTILESIZE / 2),TILETOPX(2) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX( 8) + (MAPTILESIZE / 2),TILETOPX(2) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX( 9) + (MAPTILESIZE / 2),TILETOPX(2) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX(10) + (MAPTILESIZE / 2),TILETOPX(2) + (MAPTILESIZE / 2),
                            
                            MAPFLAGEND};

const byte gamemap13[] PROGMEM = {3,
                            16,6,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            2,2,1,1,1,1,1,1,1,1,1,1,1,1,3,3,
                            2,2,1,1,1,1,1,1,1,1,1,1,1,1,3,3,
                            2,2,1,1,1,1,1,1,1,1,1,1,1,1,3,3,
                            2,2,1,1,1,1,1,1,1,1,1,1,1,1,3,3,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            TILETOPX(0) + (MAPTILESIZE / 2),TILETOPX(2) + (MAPTILESIZE / 2),
                            
                            MAPFLAGENEMY,2,TILETOPX(2),TILETOPX(0), TILETOPX(7),TILETOPX(5), TILETOPX(12),TILETOPX(0), TILETOPX(13),TILETOPX(1), TILETOPX(9),TILETOPX(5), TILETOPX(4),TILETOPX(0), TILETOPX(2),TILETOPX(2), TILETOPX(5),TILETOPX(5), 
                                           TILETOPX(10),TILETOPX(0), TILETOPX(13),TILETOPX(3), TILETOPX(11),TILETOPX(5), TILETOPX(6),TILETOPX(0), TILETOPX(2),TILETOPX(4), TILETOPX(3),TILETOPX(5), TILETOPX(8),TILETOPX(0), TILETOPX(13),TILETOPX(5),
                                           
                                           TILETOPX(8),TILETOPX(0), TILETOPX(3),TILETOPX(5), TILETOPX(2),TILETOPX(4), TILETOPX(6),TILETOPX(0), TILETOPX(11),TILETOPX(5), TILETOPX(13),TILETOPX(3), TILETOPX(10),TILETOPX(0), TILETOPX(5),TILETOPX(5),
                                           TILETOPX(2),TILETOPX(2), TILETOPX(4),TILETOPX(0), TILETOPX(9),TILETOPX(5), TILETOPX(13),TILETOPX(1), TILETOPX(12),TILETOPX(0), TILETOPX(7),TILETOPX(5),
                            
                            MAPFLAGENEMY,2,TILETOPX(13),TILETOPX(5), TILETOPX(8),TILETOPX(0), TILETOPX(3),TILETOPX(5), TILETOPX(2),TILETOPX(4), TILETOPX(6),TILETOPX(0), TILETOPX(11),TILETOPX(5), TILETOPX(13),TILETOPX(3), TILETOPX(10),TILETOPX(0),
                                           TILETOPX(5),TILETOPX(5), TILETOPX(2),TILETOPX(2), TILETOPX(4),TILETOPX(0), TILETOPX(9),TILETOPX(5), TILETOPX(13),TILETOPX(1), TILETOPX(12),TILETOPX(0), TILETOPX(7),TILETOPX(5),
                                           
                                           TILETOPX(2),TILETOPX(0), TILETOPX(7),TILETOPX(5), TILETOPX(12),TILETOPX(0), TILETOPX(13),TILETOPX(1), TILETOPX(9),TILETOPX(5), TILETOPX(4),TILETOPX(0), TILETOPX(2),TILETOPX(2), TILETOPX(5),TILETOPX(5), 
                                           TILETOPX(10),TILETOPX(0), TILETOPX(13),TILETOPX(3), TILETOPX(11),TILETOPX(5), TILETOPX(6),TILETOPX(0), TILETOPX(2),TILETOPX(4), TILETOPX(3),TILETOPX(5), TILETOPX(8),TILETOPX(0),
                            
                            MAPFLAGENEMY,2,TILETOPX(2),TILETOPX(5), TILETOPX(7),TILETOPX(0), TILETOPX(12),TILETOPX(5), TILETOPX(13),TILETOPX(4), TILETOPX(9),TILETOPX(0), TILETOPX(4),TILETOPX(5), TILETOPX(2),TILETOPX(3), TILETOPX(5),TILETOPX(0), 
                                           TILETOPX(10),TILETOPX(5), TILETOPX(13),TILETOPX(2), TILETOPX(11),TILETOPX(0), TILETOPX(6),TILETOPX(5), TILETOPX(2),TILETOPX(1), TILETOPX(3),TILETOPX(0), TILETOPX(8),TILETOPX(5), TILETOPX(13),TILETOPX(0),
                                           
                                           TILETOPX(8),TILETOPX(5), TILETOPX(3),TILETOPX(0), TILETOPX(2),TILETOPX(1), TILETOPX(6),TILETOPX(5), TILETOPX(11),TILETOPX(0), TILETOPX(13),TILETOPX(2), TILETOPX(10),TILETOPX(5), TILETOPX(5),TILETOPX(0),
                                           TILETOPX(2),TILETOPX(3), TILETOPX(4),TILETOPX(5), TILETOPX(9),TILETOPX(0), TILETOPX(13),TILETOPX(4), TILETOPX(12),TILETOPX(5), TILETOPX(7),TILETOPX(0),
                            
                            MAPFLAGENEMY,2,TILETOPX(13),TILETOPX(0), TILETOPX(8),TILETOPX(5), TILETOPX(3),TILETOPX(0), TILETOPX(2),TILETOPX(1), TILETOPX(6),TILETOPX(5), TILETOPX(11),TILETOPX(0), TILETOPX(13),TILETOPX(2), TILETOPX(10),TILETOPX(5),
                                           TILETOPX(5),TILETOPX(0), TILETOPX(2),TILETOPX(3), TILETOPX(4),TILETOPX(5), TILETOPX(9),TILETOPX(0), TILETOPX(13),TILETOPX(4), TILETOPX(12),TILETOPX(5), TILETOPX(7),TILETOPX(0),
                                           
                                           TILETOPX(2),TILETOPX(5), TILETOPX(7),TILETOPX(0), TILETOPX(12),TILETOPX(5), TILETOPX(13),TILETOPX(4), TILETOPX(9),TILETOPX(0), TILETOPX(4),TILETOPX(5), TILETOPX(2),TILETOPX(3), TILETOPX(5),TILETOPX(0), 
                                           TILETOPX(10),TILETOPX(5), TILETOPX(13),TILETOPX(2), TILETOPX(11),TILETOPX(0), TILETOPX(6),TILETOPX(5), TILETOPX(2),TILETOPX(1), TILETOPX(3),TILETOPX(0), TILETOPX(8),TILETOPX(5),
                            
                            MAPFLAGENEMY,3,TILETOPX(2),TILETOPX(0),TILETOPX(2),TILETOPX(5),
                            MAPFLAGENEMY,3,TILETOPX(3),TILETOPX(5),TILETOPX(3),TILETOPX(0),
                            MAPFLAGENEMY,3,TILETOPX(4),TILETOPX(0),TILETOPX(4),TILETOPX(5),
                            MAPFLAGENEMY,3,TILETOPX(5),TILETOPX(5),TILETOPX(5),TILETOPX(0),
                            MAPFLAGENEMY,3,TILETOPX(6),TILETOPX(0),TILETOPX(6),TILETOPX(5),
                            MAPFLAGENEMY,3,TILETOPX(7),TILETOPX(5),TILETOPX(7),TILETOPX(0),
                            MAPFLAGENEMY,3,TILETOPX(8),TILETOPX(0),TILETOPX(8),TILETOPX(5),
                            MAPFLAGENEMY,3,TILETOPX(9),TILETOPX(5),TILETOPX(9),TILETOPX(0),
                            MAPFLAGENEMY,3,TILETOPX(10),TILETOPX(0),TILETOPX(10),TILETOPX(5),
                            MAPFLAGENEMY,3,TILETOPX(11),TILETOPX(5),TILETOPX(11),TILETOPX(0),
                            MAPFLAGENEMY,3,TILETOPX(12),TILETOPX(0),TILETOPX(12),TILETOPX(5),
                            MAPFLAGENEMY,3,TILETOPX(13),TILETOPX(5),TILETOPX(13),TILETOPX(0),
                            
                            MAPFLAGEND};

const byte gamemap14[] PROGMEM = {3,
                            20,8,
                            3,3,3,1,1,0,1,1,1,1,0,0,1,1,1,1,0,0,1,0,
                            3,3,3,0,1,0,1,0,1,1,0,0,1,0,1,1,0,0,1,0,
                            0,0,0,0,1,0,1,0,0,1,1,1,1,0,0,1,1,1,1,0,
                            0,0,0,0,1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,
                            0,0,0,0,1,0,1,0,1,1,0,0,1,0,1,1,0,0,1,0,
                            0,0,0,0,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,
                            
                            TILETOPX(18) + (MAPTILESIZE / 2),TILETOPX(6) + (MAPTILESIZE / 2),
                            
                            MAPFLAGENEMY,2,TILETOPX(6),TILETOPX(0),TILETOPX(6),TILETOPX(5),
                            MAPFLAGENEMY,2,TILETOPX(9),TILETOPX(0),TILETOPX(9),TILETOPX(5),
                            MAPFLAGENEMY,2,TILETOPX(12),TILETOPX(0),TILETOPX(12),TILETOPX(5),
                            MAPFLAGENEMY,2,TILETOPX(15),TILETOPX(0),TILETOPX(15),TILETOPX(5),
                            MAPFLAGENEMY,2,TILETOPX(18),TILETOPX(0),TILETOPX(18),TILETOPX(5),
                            
                            MAPFLAGENEMY,2,TILETOPX(9),TILETOPX(0),TILETOPX(6),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(9),TILETOPX(3),TILETOPX(6),TILETOPX(3),
                            MAPFLAGENEMY,2,TILETOPX(12),TILETOPX(2),TILETOPX(9),TILETOPX(2),
                            MAPFLAGENEMY,2,TILETOPX(12),TILETOPX(5),TILETOPX(9),TILETOPX(5),
                            MAPFLAGENEMY,2,TILETOPX(15),TILETOPX(0),TILETOPX(12),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(15),TILETOPX(3),TILETOPX(12),TILETOPX(3),
                            MAPFLAGENEMY,2,TILETOPX(18),TILETOPX(2),TILETOPX(15),TILETOPX(2),
                            MAPFLAGENEMY,2,TILETOPX(18),TILETOPX(5),TILETOPX(15),TILETOPX(5),
                            
                            MAPFLAGCOIN,TILETOPX(8),TILETOPX(1),
                            MAPFLAGCOIN,TILETOPX(8),TILETOPX(4),
                            MAPFLAGCOIN,TILETOPX(11),TILETOPX(3),
                            MAPFLAGCOIN,TILETOPX(14),TILETOPX(1),
                            MAPFLAGCOIN,TILETOPX(14),TILETOPX(4),
                            MAPFLAGCOIN,TILETOPX(17),TILETOPX(3),
                            MAPFLAGCOIN,TILETOPX(18),TILETOPX(1),
                            
                            MAPFLAGEND};

const byte gamemap15[] PROGMEM = {3,
                            18,8,
                            2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,
                            
                            TILETOPX(0) + (MAPTILESIZE / 2),TILETOPX(0) + (MAPTILESIZE / 2),
                            
                            MAPFLAGENEMY,4,TILETOPX( 2),TILETOPX(0),TILETOPX( 2),TILETOPX(7),
                            MAPFLAGENEMY,2,TILETOPX( 3),TILETOPX(0),TILETOPX( 3),TILETOPX(7),
                            MAPFLAGENEMY,1,TILETOPX( 4),TILETOPX(0),TILETOPX( 4),TILETOPX(7),
                            MAPFLAGENEMY,4,TILETOPX( 5),TILETOPX(0),TILETOPX( 5),TILETOPX(7),
                            MAPFLAGENEMY,2,TILETOPX( 6),TILETOPX(0),TILETOPX( 6),TILETOPX(7),
                            MAPFLAGENEMY,1,TILETOPX( 7),TILETOPX(0),TILETOPX( 7),TILETOPX(7),
                            MAPFLAGENEMY,4,TILETOPX( 8),TILETOPX(0),TILETOPX( 8),TILETOPX(7),
                            MAPFLAGENEMY,2,TILETOPX( 9),TILETOPX(0),TILETOPX( 9),TILETOPX(7),
                            MAPFLAGENEMY,1,TILETOPX(10),TILETOPX(0),TILETOPX(10),TILETOPX(7),
                            MAPFLAGENEMY,4,TILETOPX(11),TILETOPX(0),TILETOPX(11),TILETOPX(7),
                            MAPFLAGENEMY,2,TILETOPX(12),TILETOPX(0),TILETOPX(12),TILETOPX(7),
                            MAPFLAGENEMY,1,TILETOPX(13),TILETOPX(0),TILETOPX(13),TILETOPX(7),
                            MAPFLAGENEMY,4,TILETOPX(14),TILETOPX(0),TILETOPX(14),TILETOPX(7),
                            MAPFLAGENEMY,2,TILETOPX(15),TILETOPX(0),TILETOPX(15),TILETOPX(7),
                            
                            MAPFLAGCOIN,TILETOPX(2),TILETOPX(7),
                            MAPFLAGCOIN,TILETOPX(8) + (MAPTILESIZE / 2),TILETOPX(3) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX(15),TILETOPX(0),
                            
                            MAPFLAGEND};

const byte gamemap16[] PROGMEM = {3,
                            18,10,
                            1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,2,2,
                            1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,2,2,
                            1,1,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0,0,
                            1,1,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0,0,
                            1,1,0,0,0,0,1,1,0,0,1,1,1,1,1,1,0,0,
                            1,1,0,0,0,0,1,1,0,0,1,1,1,1,1,1,0,0,
                            1,1,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,
                            1,1,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,
                            1,1,3,3,0,0,1,1,1,1,1,1,1,1,1,1,0,0,
                            1,1,3,3,0,0,1,1,1,1,1,1,1,1,1,1,0,0,
                            
                            TILETOPX(16) + (MAPTILESIZE / 2),TILETOPX(0) + (MAPTILESIZE / 2),
                            
                            MAPFLAGENEMY,2,TILETOPX(15)+1,TILETOPX(1)+1,TILETOPX(10)-1,TILETOPX(1)+1,TILETOPX(10)-1,TILETOPX(0)-1,TILETOPX(15)+1,TILETOPX(0)-1,
                            MAPFLAGENEMY,2,TILETOPX(10)-1,TILETOPX(0)-1,TILETOPX(15)+1,TILETOPX(0)-1,TILETOPX(15)+1,TILETOPX(1)+1,TILETOPX(10)-1,TILETOPX(1)+1,
                            
                            MAPFLAGENEMY,2,TILETOPX(11)+1,TILETOPX(3)+1,TILETOPX(10)-1,TILETOPX(3)+1,TILETOPX(10)-1,TILETOPX(2)-1,TILETOPX(11)+1,TILETOPX(2)-1,
                            MAPFLAGENEMY,2,TILETOPX(10)-1,TILETOPX(2)-1,TILETOPX(11)+1,TILETOPX(2)-1,TILETOPX(11)+1,TILETOPX(3)+1,TILETOPX(10)-1,TILETOPX(3)+1,
                            
                            MAPFLAGENEMY,2,TILETOPX(15)+1,TILETOPX(5)+1,TILETOPX(10)-1,TILETOPX(5)+1,TILETOPX(10)-1,TILETOPX(4)-1,TILETOPX(15)+1,TILETOPX(4)-1,
                            MAPFLAGENEMY,2,TILETOPX(10)-1,TILETOPX(4)-1,TILETOPX(15)+1,TILETOPX(4)-1,TILETOPX(15)+1,TILETOPX(5)+1,TILETOPX(10)-1,TILETOPX(5)+1,
                            
                            MAPFLAGENEMY,2,TILETOPX(15)+1,TILETOPX(7)+1,TILETOPX(14)-1,TILETOPX(7)+1,TILETOPX(14)-1,TILETOPX(6)-1,TILETOPX(15)+1,TILETOPX(6)-1,
                            MAPFLAGENEMY,2,TILETOPX(14)-1,TILETOPX(6)-1,TILETOPX(15)+1,TILETOPX(6)-1,TILETOPX(15)+1,TILETOPX(7)+1,TILETOPX(14)-1,TILETOPX(7)+1,
                            
                            MAPFLAGENEMY,2,TILETOPX(15)+1,TILETOPX(9)+1,TILETOPX(10)-1,TILETOPX(9)+1,TILETOPX(10)-1,TILETOPX(8)-1,TILETOPX(15)+1,TILETOPX(8)-1,
                            MAPFLAGENEMY,2,TILETOPX(10)-1,TILETOPX(8)-1,TILETOPX(15)+1,TILETOPX(8)-1,TILETOPX(15)+1,TILETOPX(9)+1,TILETOPX(10)-1,TILETOPX(9)+1,
                            
                            MAPFLAGENEMY,2,TILETOPX(9)+1,TILETOPX(9)+1,TILETOPX(8)-1,TILETOPX(9)+1,TILETOPX(8)-1,TILETOPX(8)-1,TILETOPX(9)+1,TILETOPX(8)-1,
                            MAPFLAGENEMY,2,TILETOPX(8)-1,TILETOPX(8)-1,TILETOPX(9)+1,TILETOPX(8)-1,TILETOPX(9)+1,TILETOPX(9)+1,TILETOPX(8)-1,TILETOPX(9)+1,
                            
                            MAPFLAGENEMY,2,TILETOPX(7)+1,TILETOPX(9)+1,TILETOPX(6)-1,TILETOPX(9)+1,TILETOPX(6)-1,TILETOPX(4)-1,TILETOPX(7)+1,TILETOPX(4)-1,
                            MAPFLAGENEMY,2,TILETOPX(6)-1,TILETOPX(4)-1,TILETOPX(7)+1,TILETOPX(4)-1,TILETOPX(7)+1,TILETOPX(9)+1,TILETOPX(6)-1,TILETOPX(9)+1,
                            
                            MAPFLAGENEMY,2,TILETOPX(7)+1,TILETOPX(3)+1,TILETOPX(6)-1,TILETOPX(3)+1,TILETOPX(6)-1,TILETOPX(2)-1,TILETOPX(7)+1,TILETOPX(2)-1,
                            MAPFLAGENEMY,2,TILETOPX(6)-1,TILETOPX(2)-1,TILETOPX(7)+1,TILETOPX(2)-1,TILETOPX(7)+1,TILETOPX(3)+1,TILETOPX(6)-1,TILETOPX(3)+1,
                            
                            MAPFLAGENEMY,2,TILETOPX(7)+1,TILETOPX(1)+1,TILETOPX(2)-1,TILETOPX(1)+1,TILETOPX(2)-1,TILETOPX(0)-1,TILETOPX(7)+1,TILETOPX(0)-1,
                            MAPFLAGENEMY,2,TILETOPX(2)-1,TILETOPX(0)-1,TILETOPX(7)+1,TILETOPX(0)-1,TILETOPX(7)+1,TILETOPX(1)+1,TILETOPX(2)-1,TILETOPX(1)+1,
                            
                            MAPFLAGENEMY,2,TILETOPX(1)+1,TILETOPX(1)+1,TILETOPX(0)-1,TILETOPX(1)+1,TILETOPX(0)-1,TILETOPX(0)-1,TILETOPX(1)+1,TILETOPX(0)-1,
                            MAPFLAGENEMY,2,TILETOPX(0)-1,TILETOPX(0)-1,TILETOPX(1)+1,TILETOPX(0)-1,TILETOPX(1)+1,TILETOPX(1)+1,TILETOPX(0)-1,TILETOPX(1)+1,
                            
                            MAPFLAGENEMY,2,TILETOPX(1)+1,TILETOPX(7)+1,TILETOPX(1)+1,TILETOPX(2)-1,TILETOPX(0)-1,TILETOPX(2)-1,TILETOPX(0)-1,TILETOPX(7)+1,
                            MAPFLAGENEMY,2,TILETOPX(0)-1,TILETOPX(2)-1,TILETOPX(0)-1,TILETOPX(7)+1,TILETOPX(1)+1,TILETOPX(7)+1,TILETOPX(1)+1,TILETOPX(2)-1,
                            
                            MAPFLAGENEMY,2,TILETOPX(1)+1,TILETOPX(9)+1,TILETOPX(1)+1,TILETOPX(8)-1,TILETOPX(0)-1,TILETOPX(8)-1,TILETOPX(0)-1,TILETOPX(9)+1,
                            MAPFLAGENEMY,2,TILETOPX(0)-1,TILETOPX(8)-1,TILETOPX(0)-1,TILETOPX(9)+1,TILETOPX(1)+1,TILETOPX(9)+1,TILETOPX(1)+1,TILETOPX(8)-1,
                            
                            MAPFLAGEND};

const byte gamemap17[] PROGMEM = {3,
                            20,10,
                            0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,3,3,
                            0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,3,3,
                            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                            2,2,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,
                            2,2,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,
                            
                            TILETOPX(0) + (MAPTILESIZE / 2),TILETOPX(8) + (MAPTILESIZE / 2),
                            
                            MAPFLAGENEMY,3,TILETOPX( 0),TILETOPX(7),TILETOPX( 0),TILETOPX(2),
                            MAPFLAGENEMY,3,TILETOPX( 1),TILETOPX(2),TILETOPX( 1),TILETOPX(7),
                            MAPFLAGENEMY,3,TILETOPX( 2),TILETOPX(7),TILETOPX( 2),TILETOPX(2),
                            MAPFLAGENEMY,3,TILETOPX( 3),TILETOPX(2),TILETOPX( 3),TILETOPX(7),
                            MAPFLAGENEMY,3,TILETOPX( 4),TILETOPX(7),TILETOPX( 4),TILETOPX(2),
                            MAPFLAGENEMY,3,TILETOPX( 5),TILETOPX(2),TILETOPX( 5),TILETOPX(7),
                            MAPFLAGENEMY,3,TILETOPX( 6),TILETOPX(7),TILETOPX( 6),TILETOPX(2),
                            MAPFLAGENEMY,3,TILETOPX( 7),TILETOPX(2),TILETOPX( 7),TILETOPX(7),
                            MAPFLAGENEMY,3,TILETOPX( 8),TILETOPX(7),TILETOPX( 8),TILETOPX(2),
                            MAPFLAGENEMY,3,TILETOPX( 9),TILETOPX(2),TILETOPX( 9),TILETOPX(7),
                            MAPFLAGENEMY,3,TILETOPX(10),TILETOPX(7),TILETOPX(10),TILETOPX(2),
                            MAPFLAGENEMY,3,TILETOPX(11),TILETOPX(2),TILETOPX(11),TILETOPX(7),
                            MAPFLAGENEMY,3,TILETOPX(12),TILETOPX(7),TILETOPX(12),TILETOPX(2),
                            MAPFLAGENEMY,3,TILETOPX(13),TILETOPX(2),TILETOPX(13),TILETOPX(7),
                            MAPFLAGENEMY,3,TILETOPX(14),TILETOPX(7),TILETOPX(14),TILETOPX(2),
                            MAPFLAGENEMY,3,TILETOPX(15),TILETOPX(2),TILETOPX(15),TILETOPX(7),
                            MAPFLAGENEMY,3,TILETOPX(16),TILETOPX(7),TILETOPX(16),TILETOPX(2),
                            MAPFLAGENEMY,3,TILETOPX(17),TILETOPX(2),TILETOPX(17),TILETOPX(7),
                            MAPFLAGENEMY,3,TILETOPX(18),TILETOPX(7),TILETOPX(18),TILETOPX(2),
                            MAPFLAGENEMY,3,TILETOPX(19),TILETOPX(2),TILETOPX(19),TILETOPX(7),
                            
                            MAPFLAGENEMY,3,TILETOPX(19),TILETOPX(2),TILETOPX( 0),TILETOPX(2),
                            MAPFLAGENEMY,3,TILETOPX( 0),TILETOPX(3),TILETOPX(19),TILETOPX(3),
                            MAPFLAGENEMY,3,TILETOPX(19),TILETOPX(4),TILETOPX( 0),TILETOPX(4),
                            MAPFLAGENEMY,3,TILETOPX( 0),TILETOPX(5),TILETOPX(19),TILETOPX(5),
                            MAPFLAGENEMY,3,TILETOPX(19),TILETOPX(6),TILETOPX( 0),TILETOPX(6),
                            MAPFLAGENEMY,3,TILETOPX( 0),TILETOPX(7),TILETOPX(19),TILETOPX(7),
                            
                            MAPFLAGEND};

const byte gamemap18[] PROGMEM = {2,
                            20,10,
                            0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,
                            0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,
                            0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,
                            0,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,
                            0,0,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0,0,0,
                            0,0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,1,1,2,2,1,1,0,0,0,0,0,0,0,
                            1,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,1,
                            1,1,1,1,1,1,1,1,2,2,2,2,1,1,1,1,1,1,1,1,
                            1,1,1,1,1,1,1,1,2,2,2,2,1,1,1,1,1,1,1,1,
                            
                            TILETOPX(9) + (MAPTILESIZE / 2),TILETOPX(7) + (MAPTILESIZE / 2),
                            
                            MAPFLAGENEMY,2,TILETOPX(8),TILETOPX(7)+1,TILETOPX(8),TILETOPX(5)-1,
                            MAPFLAGENEMY,2,TILETOPX(7),TILETOPX(4)-1,TILETOPX(7),TILETOPX(6)+1,
                            MAPFLAGENEMY,2,TILETOPX(6),TILETOPX(5)+1,TILETOPX(6),TILETOPX(3)-1,
                            MAPFLAGENEMY,2,TILETOPX(5),TILETOPX(2)-1,TILETOPX(5),TILETOPX(4)+1,
                            MAPFLAGENEMY,2,TILETOPX(4),TILETOPX(3)+1,TILETOPX(4),TILETOPX(1)-1,
                            
                            MAPFLAGENEMY,2,TILETOPX(11),TILETOPX(5)-1,TILETOPX(11),TILETOPX(7)+1,
                            MAPFLAGENEMY,2,TILETOPX(12),TILETOPX(6)+1,TILETOPX(12),TILETOPX(4)-1,
                            MAPFLAGENEMY,2,TILETOPX(13),TILETOPX(3)-1,TILETOPX(13),TILETOPX(5)+1,
                            MAPFLAGENEMY,2,TILETOPX(14),TILETOPX(4)+1,TILETOPX(14),TILETOPX(2)-1,
                            MAPFLAGENEMY,2,TILETOPX(15),TILETOPX(1)-1,TILETOPX(15),TILETOPX(3)+1,
                            
                            MAPFLAGENEMY,2,TILETOPX( 0),TILETOPX(7),TILETOPX( 0),TILETOPX(9),
                            MAPFLAGENEMY,2,TILETOPX( 1),TILETOPX(9),TILETOPX( 1),TILETOPX(7),
                            MAPFLAGENEMY,2,TILETOPX( 2),TILETOPX(7),TILETOPX( 2),TILETOPX(9),
                            MAPFLAGENEMY,2,TILETOPX( 3),TILETOPX(9),TILETOPX( 3),TILETOPX(7),
                            MAPFLAGENEMY,2,TILETOPX( 4),TILETOPX(7),TILETOPX( 4),TILETOPX(9),
                            MAPFLAGENEMY,2,TILETOPX( 5),TILETOPX(9),TILETOPX( 5),TILETOPX(7),
                            MAPFLAGENEMY,2,TILETOPX( 6),TILETOPX(7),TILETOPX( 6),TILETOPX(9),
                            MAPFLAGENEMY,2,TILETOPX( 7),TILETOPX(9),TILETOPX( 7),TILETOPX(7),
                            
                            MAPFLAGENEMY,2,TILETOPX(12),TILETOPX(9),TILETOPX(12),TILETOPX(7),
                            MAPFLAGENEMY,2,TILETOPX(13),TILETOPX(7),TILETOPX(13),TILETOPX(9),
                            MAPFLAGENEMY,2,TILETOPX(14),TILETOPX(9),TILETOPX(14),TILETOPX(7),
                            MAPFLAGENEMY,2,TILETOPX(15),TILETOPX(7),TILETOPX(15),TILETOPX(9),
                            MAPFLAGENEMY,2,TILETOPX(16),TILETOPX(9),TILETOPX(16),TILETOPX(7),
                            MAPFLAGENEMY,2,TILETOPX(17),TILETOPX(7),TILETOPX(17),TILETOPX(9),
                            MAPFLAGENEMY,2,TILETOPX(18),TILETOPX(9),TILETOPX(18),TILETOPX(7),
                            MAPFLAGENEMY,2,TILETOPX(19),TILETOPX(7),TILETOPX(19),TILETOPX(9),
                            
                            MAPFLAGCOIN,TILETOPX(2) + (MAPTILESIZE / 2),TILETOPX(0) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX(16) + (MAPTILESIZE / 2),TILETOPX(0) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX(0),TILETOPX(8),
                            MAPFLAGCOIN,TILETOPX(19),TILETOPX(8),
                            
                            MAPFLAGEND};

const byte gamemap19[] PROGMEM = {4,
                            20,10,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            4,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            4,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,
                            2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            TILETOPX(0) + (MAPTILESIZE / 2),TILETOPX(6) + (MAPTILESIZE / 2),
                            
                            MAPFLAGENEMY,1,TILETOPX(2),TILETOPX(2),TILETOPX(2),TILETOPX(0),
                            MAPFLAGENEMY,1,TILETOPX(2),TILETOPX(3),TILETOPX(2),TILETOPX(1),
                            MAPFLAGENEMY,1,TILETOPX(2),TILETOPX(4),TILETOPX(2),TILETOPX(2),
                            MAPFLAGENEMY,1,TILETOPX(2),TILETOPX(7),TILETOPX(2),TILETOPX(5),
                            MAPFLAGENEMY,1,TILETOPX(2),TILETOPX(8),TILETOPX(2),TILETOPX(6),
                            MAPFLAGENEMY,1,TILETOPX(2),TILETOPX(9),TILETOPX(2),TILETOPX(7),
                            
                            MAPFLAGENEMY,2,TILETOPX(3),TILETOPX(7),TILETOPX(3),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(3),TILETOPX(8),TILETOPX(3),TILETOPX(1),
                            MAPFLAGENEMY,2,TILETOPX(3),TILETOPX(9),TILETOPX(3),TILETOPX(2),
                            
                            MAPFLAGENEMY,2,TILETOPX(4),TILETOPX(3),TILETOPX(4),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(4),TILETOPX(6),TILETOPX(4),TILETOPX(3),
                            MAPFLAGENEMY,2,TILETOPX(4),TILETOPX(9),TILETOPX(4),TILETOPX(6),
                            
                            MAPFLAGENEMY,1,TILETOPX(5),TILETOPX(0),TILETOPX(5),TILETOPX(2),
                            MAPFLAGENEMY,1,TILETOPX(5),TILETOPX(1),TILETOPX(5),TILETOPX(3),
                            MAPFLAGENEMY,1,TILETOPX(5),TILETOPX(2),TILETOPX(5),TILETOPX(4),
                            MAPFLAGENEMY,1,TILETOPX(5),TILETOPX(5),TILETOPX(5),TILETOPX(7),
                            MAPFLAGENEMY,1,TILETOPX(5),TILETOPX(6),TILETOPX(5),TILETOPX(8),
                            MAPFLAGENEMY,1,TILETOPX(5),TILETOPX(7),TILETOPX(5),TILETOPX(9),
                            
                            MAPFLAGENEMY,1,TILETOPX(6),TILETOPX(0),TILETOPX(6),TILETOPX(2),
                            MAPFLAGENEMY,1,TILETOPX(6),TILETOPX(1),TILETOPX(6),TILETOPX(3),
                            MAPFLAGENEMY,1,TILETOPX(6),TILETOPX(2),TILETOPX(6),TILETOPX(4),
                            MAPFLAGENEMY,1,TILETOPX(6),TILETOPX(5),TILETOPX(6),TILETOPX(7),
                            MAPFLAGENEMY,1,TILETOPX(6),TILETOPX(6),TILETOPX(6),TILETOPX(8),
                            MAPFLAGENEMY,1,TILETOPX(6),TILETOPX(7),TILETOPX(6),TILETOPX(9),
                            
                            MAPFLAGENEMY,2,TILETOPX(7),TILETOPX(7),TILETOPX(7),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(7),TILETOPX(8),TILETOPX(7),TILETOPX(1),
                            MAPFLAGENEMY,2,TILETOPX(7),TILETOPX(9),TILETOPX(7),TILETOPX(2),
                            
                            MAPFLAGENEMY,2,TILETOPX(8),TILETOPX(0),TILETOPX(8),TILETOPX(7),
                            MAPFLAGENEMY,2,TILETOPX(8),TILETOPX(1),TILETOPX(8),TILETOPX(8),
                            MAPFLAGENEMY,2,TILETOPX(8),TILETOPX(2),TILETOPX(8),TILETOPX(9),
                            
                            MAPFLAGENEMY,2,TILETOPX(9),TILETOPX(0),TILETOPX(9),TILETOPX(3),
                            MAPFLAGENEMY,2,TILETOPX(9),TILETOPX(3),TILETOPX(9),TILETOPX(6),
                            MAPFLAGENEMY,2,TILETOPX(9),TILETOPX(6),TILETOPX(9),TILETOPX(9),
                            
                            MAPFLAGENEMY,2,TILETOPX(10),TILETOPX(3),TILETOPX(10),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(10),TILETOPX(6),TILETOPX(10),TILETOPX(3),
                            MAPFLAGENEMY,2,TILETOPX(10),TILETOPX(9),TILETOPX(10),TILETOPX(6),
                            
                            MAPFLAGENEMY,1,TILETOPX(11),TILETOPX(0),TILETOPX(11),TILETOPX(2),
                            MAPFLAGENEMY,1,TILETOPX(11),TILETOPX(1),TILETOPX(11),TILETOPX(3),
                            MAPFLAGENEMY,1,TILETOPX(11),TILETOPX(2),TILETOPX(11),TILETOPX(4),
                            MAPFLAGENEMY,1,TILETOPX(11),TILETOPX(5),TILETOPX(11),TILETOPX(7),
                            MAPFLAGENEMY,1,TILETOPX(11),TILETOPX(6),TILETOPX(11),TILETOPX(8),
                            MAPFLAGENEMY,1,TILETOPX(11),TILETOPX(7),TILETOPX(11),TILETOPX(9),
                            
                            MAPFLAGENEMY,2,TILETOPX(12),TILETOPX(7),TILETOPX(12),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(12),TILETOPX(8),TILETOPX(12),TILETOPX(1),
                            MAPFLAGENEMY,2,TILETOPX(12),TILETOPX(9),TILETOPX(12),TILETOPX(2),
                            
                            MAPFLAGENEMY,1,TILETOPX(13),TILETOPX(0),TILETOPX(13),TILETOPX(2),
                            MAPFLAGENEMY,1,TILETOPX(13),TILETOPX(1),TILETOPX(13),TILETOPX(3),
                            MAPFLAGENEMY,1,TILETOPX(13),TILETOPX(2),TILETOPX(13),TILETOPX(4),
                            MAPFLAGENEMY,1,TILETOPX(13),TILETOPX(5),TILETOPX(13),TILETOPX(7),
                            MAPFLAGENEMY,1,TILETOPX(13),TILETOPX(6),TILETOPX(13),TILETOPX(8),
                            MAPFLAGENEMY,1,TILETOPX(13),TILETOPX(7),TILETOPX(13),TILETOPX(9),
                            
                            MAPFLAGENEMY,2,TILETOPX(14),TILETOPX(0),TILETOPX(14),TILETOPX(3),
                            MAPFLAGENEMY,2,TILETOPX(14),TILETOPX(3),TILETOPX(14),TILETOPX(6),
                            MAPFLAGENEMY,2,TILETOPX(14),TILETOPX(6),TILETOPX(14),TILETOPX(9),
                            
                            MAPFLAGENEMY,2,TILETOPX(15),TILETOPX(0),TILETOPX(15),TILETOPX(7),
                            MAPFLAGENEMY,2,TILETOPX(15),TILETOPX(1),TILETOPX(15),TILETOPX(8),
                            MAPFLAGENEMY,2,TILETOPX(15),TILETOPX(2),TILETOPX(15),TILETOPX(9),
                            
                            MAPFLAGENEMY,2,TILETOPX(16),TILETOPX(7),TILETOPX(16),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(16),TILETOPX(8),TILETOPX(16),TILETOPX(1),
                            MAPFLAGENEMY,2,TILETOPX(16),TILETOPX(9),TILETOPX(16),TILETOPX(2),
                            
                            MAPFLAGENEMY,1,TILETOPX(17),TILETOPX(0),TILETOPX(17),TILETOPX(2),
                            MAPFLAGENEMY,1,TILETOPX(17),TILETOPX(1),TILETOPX(17),TILETOPX(3),
                            MAPFLAGENEMY,1,TILETOPX(17),TILETOPX(2),TILETOPX(17),TILETOPX(4),
                            MAPFLAGENEMY,1,TILETOPX(17),TILETOPX(5),TILETOPX(17),TILETOPX(7),
                            MAPFLAGENEMY,1,TILETOPX(17),TILETOPX(6),TILETOPX(17),TILETOPX(8),
                            MAPFLAGENEMY,1,TILETOPX(17),TILETOPX(7),TILETOPX(17),TILETOPX(9),
                            
                            MAPFLAGCOIN,TILETOPX(18) + (MAPTILESIZE / 2),TILETOPX(4) + (MAPTILESIZE / 2),
                            
                            MAPFLAGEND};

const byte gamemap20[] PROGMEM = {4,
                            18,12,
                            4,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
                            4,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,
                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
                            2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
                            2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
                            TILETOPX(0) + (MAPTILESIZE / 2),TILETOPX(10) + (MAPTILESIZE / 2),
                            
                            MAPFLAGENEMY,2,TILETOPX( 2),TILETOPX(0),TILETOPX( 2),TILETOPX(4),
                            MAPFLAGENEMY,2,TILETOPX( 3),TILETOPX(4),TILETOPX( 3),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX( 4),TILETOPX(0),TILETOPX( 4),TILETOPX(4),
                            MAPFLAGENEMY,2,TILETOPX( 5),TILETOPX(4),TILETOPX( 5),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX( 6),TILETOPX(0),TILETOPX( 6),TILETOPX(4),
                            MAPFLAGENEMY,2,TILETOPX( 7),TILETOPX(4),TILETOPX( 7),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX( 8),TILETOPX(0),TILETOPX( 8),TILETOPX(4),
                            MAPFLAGENEMY,2,TILETOPX( 9),TILETOPX(4),TILETOPX( 9),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(10),TILETOPX(0),TILETOPX(10),TILETOPX(4),
                            MAPFLAGENEMY,2,TILETOPX(11),TILETOPX(4),TILETOPX(11),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(12),TILETOPX(0),TILETOPX(12),TILETOPX(4),
                            MAPFLAGENEMY,2,TILETOPX(13),TILETOPX(4),TILETOPX(13),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(14),TILETOPX(0),TILETOPX(14),TILETOPX(4),
                            MAPFLAGENEMY,2,TILETOPX(15),TILETOPX(4),TILETOPX(15),TILETOPX(0),
                            MAPFLAGENEMY,2,TILETOPX(16),TILETOPX(0),TILETOPX(16),TILETOPX(4),

                            MAPFLAGENEMY,2,TILETOPX( 2),TILETOPX(11),TILETOPX( 2),TILETOPX( 7),
                            MAPFLAGENEMY,2,TILETOPX( 3),TILETOPX( 7),TILETOPX( 3),TILETOPX(11),
                            MAPFLAGENEMY,2,TILETOPX( 4),TILETOPX(11),TILETOPX( 4),TILETOPX( 7),
                            MAPFLAGENEMY,2,TILETOPX( 5),TILETOPX( 7),TILETOPX( 5),TILETOPX(11),
                            MAPFLAGENEMY,2,TILETOPX( 6),TILETOPX(11),TILETOPX( 6),TILETOPX( 7),
                            MAPFLAGENEMY,2,TILETOPX( 7),TILETOPX( 7),TILETOPX( 7),TILETOPX(11),
                            MAPFLAGENEMY,2,TILETOPX( 8),TILETOPX(11),TILETOPX( 8),TILETOPX( 7),
                            MAPFLAGENEMY,2,TILETOPX( 9),TILETOPX( 7),TILETOPX( 9),TILETOPX(11),
                            MAPFLAGENEMY,2,TILETOPX(10),TILETOPX(11),TILETOPX(10),TILETOPX( 7),
                            MAPFLAGENEMY,2,TILETOPX(11),TILETOPX( 7),TILETOPX(11),TILETOPX(11),
                            MAPFLAGENEMY,2,TILETOPX(12),TILETOPX(11),TILETOPX(12),TILETOPX( 7),
                            MAPFLAGENEMY,2,TILETOPX(13),TILETOPX( 7),TILETOPX(13),TILETOPX(11),
                            MAPFLAGENEMY,2,TILETOPX(14),TILETOPX(11),TILETOPX(14),TILETOPX( 7),
                            MAPFLAGENEMY,2,TILETOPX(15),TILETOPX( 7),TILETOPX(15),TILETOPX(11),
                            MAPFLAGENEMY,2,TILETOPX(16),TILETOPX(11),TILETOPX(16),TILETOPX( 7),

                            MAPFLAGENEMY,2,TILETOPX( 6),TILETOPX( 8),TILETOPX( 2),TILETOPX( 8),
                            MAPFLAGENEMY,2,TILETOPX( 6),TILETOPX( 9),TILETOPX( 2),TILETOPX( 9),
                            MAPFLAGENEMY,2,TILETOPX( 6),TILETOPX(10),TILETOPX( 2),TILETOPX(10),
                            MAPFLAGENEMY,2,TILETOPX( 6),TILETOPX(11),TILETOPX( 2),TILETOPX(11),
                            
                            MAPFLAGENEMY,2,TILETOPX(11),TILETOPX( 7),TILETOPX( 7),TILETOPX( 7),
                            MAPFLAGENEMY,2,TILETOPX(11),TILETOPX( 8),TILETOPX( 7),TILETOPX( 8),
                            MAPFLAGENEMY,2,TILETOPX(11),TILETOPX( 9),TILETOPX( 7),TILETOPX( 9),
                            MAPFLAGENEMY,2,TILETOPX(11),TILETOPX(10),TILETOPX( 7),TILETOPX(10),
                            
                            MAPFLAGENEMY,2,TILETOPX(16),TILETOPX( 8),TILETOPX(12),TILETOPX( 8),
                            MAPFLAGENEMY,2,TILETOPX(16),TILETOPX( 9),TILETOPX(12),TILETOPX( 9),
                            MAPFLAGENEMY,2,TILETOPX(16),TILETOPX(10),TILETOPX(12),TILETOPX(10),
                            MAPFLAGENEMY,2,TILETOPX(16),TILETOPX(11),TILETOPX(12),TILETOPX(11),

                            MAPFLAGENEMY,2,TILETOPX( 2),TILETOPX( 0),TILETOPX( 6),TILETOPX( 0),
                            MAPFLAGENEMY,2,TILETOPX( 2),TILETOPX( 1),TILETOPX( 6),TILETOPX( 1),
                            MAPFLAGENEMY,2,TILETOPX( 2),TILETOPX( 2),TILETOPX( 6),TILETOPX( 2),
                            MAPFLAGENEMY,2,TILETOPX( 2),TILETOPX( 4),TILETOPX( 6),TILETOPX( 4),

                            MAPFLAGENEMY,2,TILETOPX( 7),TILETOPX( 0),TILETOPX(11),TILETOPX( 0),
                            MAPFLAGENEMY,2,TILETOPX( 7),TILETOPX( 2),TILETOPX(11),TILETOPX( 2),
                            MAPFLAGENEMY,2,TILETOPX( 7),TILETOPX( 3),TILETOPX(11),TILETOPX( 3),
                            MAPFLAGENEMY,2,TILETOPX( 7),TILETOPX( 4),TILETOPX(11),TILETOPX( 4),
                            
                            MAPFLAGENEMY,2,TILETOPX(12),TILETOPX( 0),TILETOPX(16),TILETOPX( 0),
                            MAPFLAGENEMY,2,TILETOPX(12),TILETOPX( 1),TILETOPX(16),TILETOPX( 1),
                            MAPFLAGENEMY,2,TILETOPX(12),TILETOPX( 2),TILETOPX(16),TILETOPX( 2),
                            MAPFLAGENEMY,2,TILETOPX(12),TILETOPX( 4),TILETOPX(16),TILETOPX( 4),
                            
                            MAPFLAGEND};

const byte gamemap21[] PROGMEM = {3,
                            20,10,
                            3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,
                            3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,
                            0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
                            TILETOPX(18),TILETOPX(0) + (MAPTILESIZE / 2),

                            MAPFLAGENEMY,3,TILETOPX( 2),TILETOPX(9),TILETOPX( 2),TILETOPX(3),
                            MAPFLAGENEMY,3,TILETOPX( 3),TILETOPX(3),TILETOPX( 3),TILETOPX(9),
                            MAPFLAGENEMY,3,TILETOPX( 4),TILETOPX(9),TILETOPX( 4),TILETOPX(3),
                            MAPFLAGENEMY,3,TILETOPX( 5),TILETOPX(3),TILETOPX( 5),TILETOPX(9),
                            MAPFLAGENEMY,3,TILETOPX( 6),TILETOPX(9),TILETOPX( 6),TILETOPX(3),
                            MAPFLAGENEMY,3,TILETOPX( 7),TILETOPX(3),TILETOPX( 7),TILETOPX(9),
                            MAPFLAGENEMY,3,TILETOPX( 8),TILETOPX(9),TILETOPX( 8),TILETOPX(3),
                            MAPFLAGENEMY,3,TILETOPX( 9),TILETOPX(3),TILETOPX( 9),TILETOPX(9),
                            MAPFLAGENEMY,3,TILETOPX(10),TILETOPX(9),TILETOPX(10),TILETOPX(3),
                            MAPFLAGENEMY,3,TILETOPX(11),TILETOPX(3),TILETOPX(11),TILETOPX(9),
                            MAPFLAGENEMY,3,TILETOPX(12),TILETOPX(9),TILETOPX(12),TILETOPX(3),
                            MAPFLAGENEMY,3,TILETOPX(13),TILETOPX(3),TILETOPX(13),TILETOPX(9),
                            MAPFLAGENEMY,3,TILETOPX(14),TILETOPX(9),TILETOPX(14),TILETOPX(3),
                            MAPFLAGENEMY,3,TILETOPX(15),TILETOPX(3),TILETOPX(15),TILETOPX(9),
                            MAPFLAGENEMY,3,TILETOPX(16),TILETOPX(9),TILETOPX(16),TILETOPX(3),
                            MAPFLAGENEMY,3,TILETOPX(17),TILETOPX(3),TILETOPX(17),TILETOPX(9),

                            MAPFLAGENEMY,3,TILETOPX(17),TILETOPX(3),TILETOPX( 2),TILETOPX(3),
                            MAPFLAGENEMY,3,TILETOPX( 2),TILETOPX(4),TILETOPX(17),TILETOPX(4),
                            MAPFLAGENEMY,3,TILETOPX(17),TILETOPX(5),TILETOPX( 2),TILETOPX(5),
                            MAPFLAGENEMY,3,TILETOPX( 2),TILETOPX(6),TILETOPX(17),TILETOPX(6),
                            MAPFLAGENEMY,3,TILETOPX(17),TILETOPX(7),TILETOPX( 2),TILETOPX(7),
                            MAPFLAGENEMY,3,TILETOPX( 2),TILETOPX(8),TILETOPX(17),TILETOPX(8),
                            MAPFLAGENEMY,3,TILETOPX(17),TILETOPX(9),TILETOPX( 2),TILETOPX(9),
                            
                            MAPFLAGCOIN,TILETOPX(2),TILETOPX(9),
                            MAPFLAGCOIN,TILETOPX(17),TILETOPX(9),
                            
                            MAPFLAGEND};

const byte gamemap22[] PROGMEM = {2,
                            20,12,
                            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                            2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                            TILETOPX(0) + (MAPTILESIZE / 2),TILETOPX(10) + (MAPTILESIZE / 2),

                            MAPFLAGENEMY,1,TILETOPX( 0),TILETOPX(9),TILETOPX( 0),TILETOPX(0),
                            MAPFLAGENEMY,1,TILETOPX( 1),TILETOPX(0),TILETOPX( 1),TILETOPX(9),
                            MAPFLAGENEMY,1,TILETOPX( 2),TILETOPX(9),TILETOPX( 2),TILETOPX(0),
                            MAPFLAGENEMY,1,TILETOPX( 3),TILETOPX(0),TILETOPX( 3),TILETOPX(9),
                            MAPFLAGENEMY,1,TILETOPX( 4),TILETOPX(9),TILETOPX( 4),TILETOPX(0),
                            MAPFLAGENEMY,1,TILETOPX( 5),TILETOPX(0),TILETOPX( 5),TILETOPX(9),
                            MAPFLAGENEMY,1,TILETOPX( 6),TILETOPX(9),TILETOPX( 6),TILETOPX(0),
                            MAPFLAGENEMY,1,TILETOPX( 7),TILETOPX(0),TILETOPX( 7),TILETOPX(9),
                            MAPFLAGENEMY,1,TILETOPX( 8),TILETOPX(9),TILETOPX( 8),TILETOPX(0),
                            MAPFLAGENEMY,1,TILETOPX( 9),TILETOPX(0),TILETOPX( 9),TILETOPX(9),
                            MAPFLAGENEMY,1,TILETOPX(10),TILETOPX(9),TILETOPX(10),TILETOPX(0),
                            MAPFLAGENEMY,1,TILETOPX(11),TILETOPX(0),TILETOPX(11),TILETOPX(9),
                            MAPFLAGENEMY,1,TILETOPX(12),TILETOPX(9),TILETOPX(12),TILETOPX(0),
                            MAPFLAGENEMY,1,TILETOPX(13),TILETOPX(0),TILETOPX(13),TILETOPX(9),
                            MAPFLAGENEMY,1,TILETOPX(14),TILETOPX(9),TILETOPX(14),TILETOPX(0),
                            MAPFLAGENEMY,1,TILETOPX(15),TILETOPX(0),TILETOPX(15),TILETOPX(9),
                            MAPFLAGENEMY,1,TILETOPX(16),TILETOPX(9),TILETOPX(16),TILETOPX(0),
                            MAPFLAGENEMY,1,TILETOPX(17),TILETOPX(0),TILETOPX(17),TILETOPX(9),
                            MAPFLAGENEMY,1,TILETOPX(18),TILETOPX(9),TILETOPX(18),TILETOPX(0),
                            MAPFLAGENEMY,1,TILETOPX(19),TILETOPX(0),TILETOPX(19),TILETOPX(9),

                            MAPFLAGENEMY,3,TILETOPX(0),TILETOPX(9), TILETOPX(0),TILETOPX(1), TILETOPX(18),TILETOPX(1), TILETOPX(18),TILETOPX(9),
                            MAPFLAGENEMY,3,TILETOPX(0),TILETOPX(8), TILETOPX(0),TILETOPX(0), TILETOPX(18),TILETOPX(0), TILETOPX(18),TILETOPX(8),
                            MAPFLAGENEMY,3,TILETOPX(1),TILETOPX(8), TILETOPX(1),TILETOPX(0), TILETOPX(19),TILETOPX(0), TILETOPX(19),TILETOPX(8),
                            MAPFLAGENEMY,3,TILETOPX(1),TILETOPX(9), TILETOPX(1),TILETOPX(1), TILETOPX(19),TILETOPX(1), TILETOPX(19),TILETOPX(9),
                            MAPFLAGENEMY,3,TILETOPX(0) + (MAPTILESIZE / 2),TILETOPX(8) + (MAPTILESIZE / 2), TILETOPX(0) + (MAPTILESIZE / 2),TILETOPX(0) + (MAPTILESIZE / 2), TILETOPX(18) + (MAPTILESIZE / 2),TILETOPX(0) + (MAPTILESIZE / 2), TILETOPX(18) + (MAPTILESIZE / 2),TILETOPX(8) + (MAPTILESIZE / 2),
                            
                            MAPFLAGENEMY,3,TILETOPX(18),TILETOPX(1), TILETOPX(18),TILETOPX(9), TILETOPX(0),TILETOPX(9), TILETOPX(0),TILETOPX(1),
                            MAPFLAGENEMY,3,TILETOPX(18),TILETOPX(0), TILETOPX(18),TILETOPX(8), TILETOPX(0),TILETOPX(8), TILETOPX(0),TILETOPX(0),
                            MAPFLAGENEMY,3,TILETOPX(19),TILETOPX(0), TILETOPX(19),TILETOPX(8), TILETOPX(1),TILETOPX(8), TILETOPX(1),TILETOPX(0),
                            MAPFLAGENEMY,3,TILETOPX(19),TILETOPX(1), TILETOPX(19),TILETOPX(9), TILETOPX(1),TILETOPX(9), TILETOPX(1),TILETOPX(1),
                            MAPFLAGENEMY,3,TILETOPX(18) + (MAPTILESIZE / 2),TILETOPX(0) + (MAPTILESIZE / 2), TILETOPX(18) + (MAPTILESIZE / 2),TILETOPX(8) + (MAPTILESIZE / 2), TILETOPX(0) + (MAPTILESIZE / 2),TILETOPX(8) + (MAPTILESIZE / 2), TILETOPX(0) + (MAPTILESIZE / 2),TILETOPX(0) + (MAPTILESIZE / 2),

                            MAPFLAGENEMY,3,TILETOPX(2),TILETOPX(7), TILETOPX(2),TILETOPX(3), TILETOPX(16),TILETOPX(3), TILETOPX(16),TILETOPX(7),
                            MAPFLAGENEMY,3,TILETOPX(2),TILETOPX(6), TILETOPX(2),TILETOPX(2), TILETOPX(16),TILETOPX(2), TILETOPX(16),TILETOPX(6),
                            MAPFLAGENEMY,3,TILETOPX(3),TILETOPX(6), TILETOPX(3),TILETOPX(2), TILETOPX(17),TILETOPX(2), TILETOPX(17),TILETOPX(6),
                            MAPFLAGENEMY,3,TILETOPX(3),TILETOPX(7), TILETOPX(3),TILETOPX(3), TILETOPX(17),TILETOPX(3), TILETOPX(17),TILETOPX(7),
                            MAPFLAGENEMY,3,TILETOPX(2) + (MAPTILESIZE / 2),TILETOPX(6) + (MAPTILESIZE / 2), TILETOPX(2) + (MAPTILESIZE / 2),TILETOPX(2) + (MAPTILESIZE / 2), TILETOPX(16) + (MAPTILESIZE / 2),TILETOPX(2) + (MAPTILESIZE / 2), TILETOPX(16) + (MAPTILESIZE / 2),TILETOPX(6) + (MAPTILESIZE / 2),
                            
                            MAPFLAGENEMY,3,TILETOPX(16),TILETOPX(3), TILETOPX(16),TILETOPX(7), TILETOPX(2),TILETOPX(7), TILETOPX(2),TILETOPX(3),
                            MAPFLAGENEMY,3,TILETOPX(16),TILETOPX(2), TILETOPX(16),TILETOPX(6), TILETOPX(2),TILETOPX(6), TILETOPX(2),TILETOPX(2),
                            MAPFLAGENEMY,3,TILETOPX(17),TILETOPX(2), TILETOPX(17),TILETOPX(6), TILETOPX(3),TILETOPX(6), TILETOPX(3),TILETOPX(2),
                            MAPFLAGENEMY,3,TILETOPX(17),TILETOPX(3), TILETOPX(17),TILETOPX(7), TILETOPX(3),TILETOPX(7), TILETOPX(3),TILETOPX(3),
                            MAPFLAGENEMY,3,TILETOPX(16) + (MAPTILESIZE / 2),TILETOPX(2) + (MAPTILESIZE / 2), TILETOPX(16) + (MAPTILESIZE / 2),TILETOPX(6) + (MAPTILESIZE / 2), TILETOPX(2) + (MAPTILESIZE / 2),TILETOPX(6) + (MAPTILESIZE / 2), TILETOPX(2) + (MAPTILESIZE / 2),TILETOPX(2) + (MAPTILESIZE / 2),
                            
                            MAPFLAGENEMY,3,TILETOPX(4),TILETOPX(5), TILETOPX(14),TILETOPX(5),
                            MAPFLAGENEMY,3,TILETOPX(4),TILETOPX(4), TILETOPX(14),TILETOPX(4),
                            MAPFLAGENEMY,3,TILETOPX(5),TILETOPX(4), TILETOPX(15),TILETOPX(4),
                            MAPFLAGENEMY,3,TILETOPX(5),TILETOPX(5), TILETOPX(15),TILETOPX(5),
                            MAPFLAGENEMY,3,TILETOPX(4) + (MAPTILESIZE / 2),TILETOPX(4) + (MAPTILESIZE / 2), TILETOPX(14) + (MAPTILESIZE / 2),TILETOPX(4) + (MAPTILESIZE / 2),
                            
                            MAPFLAGCOIN,TILETOPX(0) + (MAPTILESIZE / 2),TILETOPX(0) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX(18) + (MAPTILESIZE / 2),TILETOPX(0) + (MAPTILESIZE / 2),
                            MAPFLAGCOIN,TILETOPX(18) + (MAPTILESIZE / 2),TILETOPX(8) + (MAPTILESIZE / 2),
                            
                            MAPFLAGEND};

const byte *gamemaps[] = {gamemap1,gamemap2,gamemap3,gamemap4,gamemap5,gamemap6,gamemap7,gamemap8,gamemap9,gamemap10,gamemap11,gamemap12,gamemap13,gamemap14,gamemap15,gamemap16,gamemap17,gamemap18,gamemap19,gamemap20,gamemap21,gamemap22};

