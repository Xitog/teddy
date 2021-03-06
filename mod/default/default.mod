{
    "filetype": "mod",
    "version": 1.0,
    "name": "Default",
    "licence": "Creative Commons Zero (CC0)",
    "creator": "Created by Damien Gouteux",
    "websites": {
        "Main": "https://xitog.github.io/dgx/"
    },
    "layers": {
        "ground": {
            "res": "textures",
            "default": "blue",
            "apply": "green",
            "visible": true,
            "icon": "ground"
        },
        "wall": {
            "res": "textures",
            "default": 0,
            "apply": "black",
            "visible": true,
            "icon": "wall"
        },
        "ceiling": {
            "res": "textures",
            "default": 0,
            "apply": "red",
            "visible": false,
            "icon": "ceiling"
        },
        "height": {
            "res": "numbers_HL",
            "default": "one",
            "apply": "two",
            "visible": false,
            "icon": "height"
        },
        "area": {
            "res": "numbers_BR",
            "default": 0,
            "apply": "one",
            "visible": false,
            "icon": "area"
        },
        "object": {
            "res": "objects",
            "default": 0,
            "apply": "heart",
            "visible": true,
            "icon": "object"
        }
    },
    "resources": {
        "textures": {
            "icon": "black",
            "content": {
                "black": {
                    "val": 1,
                    "file": "black.png"
                },
                "blue": {
                    "val": 2,
                    "file": "blue.png"
                },
                "green": {
                    "val": 3,
                    "file": "green.png"
                },
                "red": {
                    "val": 4,
                    "file": "red.png"
                }
            }
        },
        "objects": {
            "icon": "heart",
            "content": {
                "heart": {
                    "val": 1,
                    "file": "object-heart.png"
                },
                "goldkey": {
                    "val": 2,
                    "file": "object-goldkey.png"
                }
            }
        },
        "numbers_HL": {
            "icon": "one",
            "content": {
                "one": {
                    "val": 1,
                    "file": "num_hl_1.png"
                },
                "two": {
                    "val": 2,
                    "file": "num_hl_2.png"
                },
                "three": {
                    "val": 3,
                    "file": "num_hl_3.png"
                },
                "four": {
                    "val": 4,
                    "file": "num_hl_4.png"
                },
                "five": {
                    "val": 5,
                    "file": "num_hl_5.png"
                },
                "six": {
                    "val": 6,
                    "file": "num_hl_6.png"
                }
            }
        },
        "numbers_BR": {
            "icon": "one",
            "content": {
                "one": {
                    "val": 1,
                    "file": "num_br_1.png"
                },
                "two": {
                    "val": 2,
                    "file": "num_br_2.png"
                },
                "three": {
                    "val": 3,
                    "file": "num_br_3.png"
                },
                "four": {
                    "val": 4,
                    "file": "num_br_4.png"
                },
                "five": {
                    "val": 5,
                    "file": "num_br_5.png"
                },
                "six": {
                    "val": 6,
                    "file": "num_br_6.png"
                }
            }
        },
        "icons": {
            "icon": "ground",
            "content": {
                "ground": {
                    "val": 1,
                    "file": "layer-ground.png"
                },
                "wall": {
                    "val": 2,
                    "file": "layer-wall.png"
                },
                "ceiling": {
                    "val": 3,
                    "file": "layer-ceiling.png"
                },
                "height": {
                    "val": 4,
                    "file": "layer-height.png"
                },
                "area": {
                    "val": 5,
                    "file": "layer-area.png"
                },
                "object": {
                    "val": 6,
                    "file": "layer-object.png"
                }
            }
        }
    },
    "stakeholders": {
        "Player 1": {
            "val": 1,
            "color": "blue"
        },
        "Player 2": {
            "val": 2,
            "color": "red"
        },
        "Player 3": {
            "val": 3,
            "color": "green"
        },
        "Player 4": {
            "val": 4,
            "color": "yellow"
        }
    },
    "default_layer": "wall",
    "has_transition": true
}