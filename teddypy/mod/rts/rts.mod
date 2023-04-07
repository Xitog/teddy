{
    "filetype" : "mod",
    "version" : 1.0,
    "name" : "RTS",
    "licence" : "?",
    "creator" : "?",
    "websites" : {
    },
    "layers" : {
        "ground"  : {
            "res"     : "textures",
            "default" : "grass",
            "apply"   : "water",
            "visible" : true,
            "icon"    : "ground"
        },
        "trans" : {
            "res"     : [
                "darkgrass",
                "drymud",
                "grassmud",
                "mudwater",
                "waterdeep"],
            "default" : "grass",
            "apply"   : "water",
            "visible" : false,
            "icon"    : "trans"
        },
        "height"  : { 
            "res" : "numbers_HL",
            "default" : 0,
            "apply" : "one",
            "visible" : false,
            "icon" : "height"
        },
        "area"    : {
            "res"     : "numbers_BR",
            "default" : 0,
            "apply"   : "one",
            "visible" : false,
            "icon"    : "area"
        },
        "object"  : {
            "res"     : "objects",
            "default" : 0,
            "apply"   : "warrior",
            "visible" : true,
            "icon"    : "object"
        }
    },
    "resources" : {
        "textures" : {
            "icon"    : "grass",
            "content" : {
                "deep"  : { "val" : 1, "file" : "deep-1.png"  },
                "water" : { "val" : 2, "file" : "water-1.png" },
                "mud"   : { "val" : 3, "file" : "mud-1.png"   },
                "dry"   : { "val" : 4, "file" : "dry-1.png"   },
                "grass" : { "val" : 5, "file" : "grass-1.png" },
                "dark"  : { "val" : 6, "file" : "dark-1.png"  }
            }
        },
        "darkgrass" : {
            "icon"    : "dark",
            "content" : {
                "dark"    : { "val" :    6, "file" : "dark-1.png"           },
                "grass"   : { "val" :    5, "file" : "grass-1.png"          },
                "dgces1"  : { "val" : 6501, "file" : "dark-ces-grass-1.png" }
            }
        },
        "drymud" : {
            "icon"    : "dry",
            "content" : {
                "dry"   : { "val" : 4, "file" : "dry-1.png"   },
                "mud"   : { "val" : 3, "file" : "mud-1.png"   }
            }
        },
        "grassmud" : {
            "icon"    : "grass",
            "content" : {
                "grass" : { "val" : 5, "file" : "grass-1.png" },
                "mud"   : { "val" : 3, "file" : "mud-1.png"   }
            }
        },
        "mudwater" : {
            "icon"    : "mud",
            "content" : {
                "mud"   : { "val" : 3, "file" : "mud-1.png"   },
                "water" : { "val" : 2, "file" : "water-1.png" }
            }
        },
        "waterdeep" : {
            "icon"    : "water",
            "content" : {
                "water" : { "val" : 2, "file" : "water-1.png" },
                "grass" : { "val" : 5, "file" : "grass-1.png" }
            }
        },
        "objects" : {
            "icon"    : "warrior",
            "content" : {
                "warrior" : { "val" : 1, "file" : "obj_warrior.png" },
                "dwarf"   : { "val" : 2, "file" : "obj_dwarf.png"   }
            }
        },
        "numbers_HL" : {
            "icon"    : "one",
            "content" : {
                "one"     : { "val" :  1, "file" : "num_hl_1.png"  },
                "two"     : { "val" :  2, "file" : "num_hl_2.png"  },
                "three"   : { "val" :  3, "file" : "num_hl_3.png"  },
                "four"    : { "val" :  4, "file" : "num_hl_4.png"  },
                "five"    : { "val" :  5, "file" : "num_hl_5.png"  },
                "six"     : { "val" :  6, "file" : "num_hl_6.png"  },
                "sept"    : { "val" :  7, "file" : "num_hl_7.png"  },
                "huit"    : { "val" :  8, "file" : "num_hl_8.png"  },
                "neuf"    : { "val" :  9, "file" : "num_hl_9.png"  },
                "dix"     : { "val" : 10, "file" : "num_hl_10.png" }
            }
        },
        "numbers_BR" : {
            "icon"    : "one",
            "content" : {
                "one"     : { "val" :  1, "file" : "num_br_1.png"  },
                "two"     : { "val" :  2, "file" : "num_br_2.png"  },
                "three"   : { "val" :  3, "file" : "num_br_3.png"  },
                "four"    : { "val" :  4, "file" : "num_br_4.png"  },
                "five"    : { "val" :  5, "file" : "num_br_5.png"  },
                "six"     : { "val" :  6, "file" : "num_br_6.png"  },
                "sept"    : { "val" :  7, "file" : "num_br_7.png"  },
                "huit"    : { "val" :  8, "file" : "num_br_8.png"  },
                "neuf"    : { "val" :  9, "file" : "num_br_9.png"  },
                "dix"     : { "val" : 10, "file" : "num_br_10.png" }
            }
        },
        "icons" : {
            "icon"    : "ground",
            "content" : {
                "ground"  : { "val" : 1, "file" : "layer_ground.png" },
                "trans"   : { "val" : 2, "file" : "layer_trans.png"  },
                "height"  : { "val" : 3, "file" : "layer_height.png" },
                "area"    : { "val" : 4, "file" : "layer_area.png"   },
                "object"  : { "val" : 5, "file" : "layer_object.png" }
            }
        }
    },
    "stakeholders" : {
        "Player 1": { "val" : 1, "color" : "blue"   },
        "Player 2": { "val" : 2, "color" : "red"    },
        "Player 3": { "val" : 3, "color" : "green"  },
        "Player 4": { "val" : 4, "color" : "yellow" }
    },
    "default_layer" : "ground",
    "has_transition" : true
}