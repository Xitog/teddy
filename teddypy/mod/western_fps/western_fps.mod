{
    "filetype" : "mod",
    "version"  : 1.0,
    "name"     : "Western FPS",
    "licence"  : "Creative Commons Zero (CC0)",
    "creator"  : "Created at Sparklin Labs by Pixel-boy",
    "websites" : {
        "Pixel-boy"     : "https://twitter.com/2pblog1",
        "Sparklin Labs" : "https://sparklinlabs.itch.io/superpowers",
        "Superpowers"   : "http://superpowers-html5.com/",
        "Github"        : "https://github.com/sparklinlabs/superpowers-asset-packs"
    },
    "layers" : {
        "ground"  : {
            "res"     : "textures",
            "default" : "void",
            "apply"   : "redbrick",
            "visible" : true,
            "icon"    : "ground"
        },
        "wall"    : {
            "res"     : "textures",
            "default" : 0,
            "apply"   : "greystone",
            "visible" : true,
            "icon"    : "wall"
        },
        "ceiling" : {
            "res"     : "textures",
            "default" : 0,
            "apply"   : "redbrick",
            "visible" : false,
            "icon"    : "ceiling"
        },
        "height"  : {
            "res"     : "numbers_HL",
            "default" : 0,
            "apply"   : "one",
            "visible" : false,
            "icon"    : "height"
        },
        "area"    : {
            "res"     : "numbers_BR",
            "default" : 0,
            "apply"   : "one",
            "visible" : false,
            "icon"    : "area"
        },
        "object"  : {
            "res"     : ["objects", "weapons"],
            "default" : 0,
            "apply"   : "ammo",
            "visible" : true,
            "icon"    : "object",
            "object"  : "Item"
        },
        "monster" : {
            "res"     : "monsters",
            "default" : 0,
            "apply"   : "enemy1",
            "visible" : true,
            "icon"    : "monster",
            "object"  : "Monster"
        }
    },
    "resources" : {
        "textures" : {
            "icon"    : "greystone",
            "content" : {
                "void"             : { "val" :  1, "file" : "0-placeholder.bmp"            },
                "greystone"        : { "val" :  2, "file" : "1-grey_stone_wall.bmp"        },
                "greystone1stone"  : { "val" :  3, "file" : "2-grey_stone_wall_stone.bmp"  },
                "lightstone"       : { "val" :  4, "file" : "3-light_stone_wall.bmp"       },
                "lightstone1stone" : { "val" :  5, "file" : "4-light_stone_wall_stone.bmp" },
                "lightstone2grass" : { "val" :  6, "file" : "5-light_stone_wall_grass.bmp" },
                "lightwood"        : { "val" :  7, "file" : "6-light_wood_wall.bmp"        },
                "lightwoodwindow"  : { "val" :  8, "file" : "7-light_wood_wall_window.bmp" },
                "darkwood"         : { "val" :  9, "file" : "8-dark_wood_wall.bmp"         },
                "redbrick"         : { "val" : 10, "file" : "9-red_brick_wall.bmp"         },
                "redbrick1light"   : { "val" : 11, "file" : "10-red_brick_wall_light.bmp"  },
                "redbrick2pillar"  : { "val" : 12, "file" : "11-red_brick_wall_pillar.bmp" },
                "redbrick3bars"    : { "val" : 13, "file" : "12-red_brick_wall_bars.bmp"   },
                "cream-wall"       : { "val" : 14, "file" : "texture-cream-wall.png"       },
                "yellow-wall"      : { "val" : 15, "file" : "texture-yellow-wall.png"      },
                "door-closed"      : { "val" : 16, "file" : "texture-door-closed.png"      }
            }
        },
        "objects" : {
            "icon"    : "goldkey",
            "content" : {
                "suppress"   : { "val" :   0, "file" : "suppress.png"                   },
                "start"      : { "val" :   1, "file" : "object-start-ico.png"           },
                "goldkey"    : { "val" :   2, "file" : "object-key-gold-ico.png"        },
                "silverkey"  : { "val" :   3, "file" : "object-key-silver-ico.png"      },
                "burger"     : { "val" :   4, "file" : "object-burger-ico.png"          },
                "coin"       : { "val" :   5, "file" : "object-coin-ico.png"            },
                "gold-lingo" : { "val" :   6, "file" : "object-gold-lingo-ico.png"      },
                "purse"      : { "val" :   7, "file" : "object-purse-ico.png"           },
                "lantern"    : { "val" :   8, "file" : "object-lantern-ico.png"         },
                "rose"       : { "val" :   9, "file" : "object-rose-ico.png"            },
                "rubis"      : { "val" :  10, "file" : "object-rubis-ico.png"           },
                "badge"      : { "val" :  11, "file" : "object-sheriff-badge-ico.png"   },
                "ammo"       : { "val" :  12, "file" : "object-ammo-ico.png"            }
            }
        },
        "weapons" : {
            "icon"    : "pistol",
            "content" : { 
                "suppress"   : { "val" :   0, "file" : "suppress.png"                   },
                "knife"      : { "val" : 100, "file" : "object-weapon-knife-ico.png"    },
                "pistol"     : { "val" : 101, "file" : "object-weapon-pistol-ico.png"   },
                "shotgun"    : { "val" : 102, "file" : "object-weapon-shotgun-ico.png"  },
                "rifle"      : { "val" : 103, "file" : "object-weapon-rifle-ico.png"    },
                "chaingun"   : { "val" : 104, "file" : "object-weapon-chaingun-ico.png" },
                "dynamite"   : { "val" : 105, "file" : "object-weapon-dynamite-ico.png" }
            }
        },
        "monsters" : {
            "icon"    : "enemy1",
            "content" : {
                "suppress"   : { "val" :   0, "file" : "suppress.png"                   },
                "enemy1"     : { "val" : 200, "file" : "enemy-cowboy-1-ico.png"         },
                "poncho"     : { "val" : 201, "file" : "enemy-cowboy-poncho-ico.png"    },
                "clerc"      : { "val" : 202, "file" : "enemy-cowboy-clerc-ico.png"     },
                "grizzly1"   : { "val" : 203, "file" : "enemy-grizzli-1-ico.png"        },
                "grizzly2"   : { "val" : 204, "file" : "enemy-grizzli-2-ico.png"        }
            }
        },
        "numbers_HL" : {
            "icon"    : "one",
            "content" : {
                "suppress": { "val" :  0, "file" : "suppress.png"  },
                "one"     : { "val" :  1, "file" : "num_hl_1.png"  },
                "two"     : { "val" :  2, "file" : "num_hl_2.png"  },
                "three"   : { "val" :  3, "file" : "num_hl_3.png"  },
                "four"    : { "val" :  4, "file" : "num_hl_4.png"  },
                "five"    : { "val" :  5, "file" : "num_hl_5.png"  },
                "six"     : { "val" :  6, "file" : "num_hl_6.png"  },
                "seven"   : { "val" :  7, "file" : "num_hl_7.png"  },
                "eight"   : { "val" :  8, "file" : "num_hl_8.png"  },
                "nine"    : { "val" :  9, "file" : "num_hl_9.png"  },
                "ten"     : { "val" : 10, "file" : "num_hl_10.png" }
            }
        },
        "numbers_BR" : {
            "icon"    : "one",
            "content" : {
                "suppress": { "val" :  0, "file" : "suppress.png" },
                "one"     : { "val" :  1, "file" : "num_br_1.png" },
                "two"     : { "val" :  2, "file" : "num_br_2.png" },
                "three"   : { "val" :  3, "file" : "num_br_3.png" },
                "four"    : { "val" :  4, "file" : "num_br_4.png" },
                "five"    : { "val" :  5, "file" : "num_br_5.png" },
                "six"     : { "val" :  6, "file" : "num_br_6.png" },
                "seven"   : { "val" :  7, "file" : "num_br_7.png"  },
                "eight"   : { "val" :  8, "file" : "num_br_8.png"  },
                "nine"    : { "val" :  9, "file" : "num_br_9.png"  },
                "ten"     : { "val" : 10, "file" : "num_br_10.png" }
            }
        },
        "icons" : {
            "icon"    : "wall",
            "content" : { 
                "ground"  : { "val" : 1, "file" : "layer-ground.png"  },
                "wall"    : { "val" : 2, "file" : "layer-wall.png"    },
                "ceiling" : { "val" : 3, "file" : "layer-ceiling.png" },
                "height"  : { "val" : 4, "file" : "layer-height.png"  },
                "area"    : { "val" : 5, "file" : "layer-area.png"    },
                "object"  : { "val" : 6, "file" : "layer-object.png"  },
                "monster" : { "val" : 7, "file" : "layer-monster.png" }
            }
        }
    },
    "stakeholders": {
        "Player 1": { "val" : 1, "color" : "blue"   },
        "Player 2": { "val" : 2, "color" : "red"    },
        "Player 3": { "val" : 3, "color" : "green"  },
        "Player 4": { "val" : 4, "color" : "yellow" }
    },
    "types" : {
        "Item": {
            "name": "Content_Name"
        },
        "Monster": {
            "name": "Content_Name",
            "player": "Meta_Player"
        }
    },
    "default_layer"  : "wall",
    "has_transition" : false
}
