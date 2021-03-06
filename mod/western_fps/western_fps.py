
def check_map(app):
    res = ''
    for row in range(0, app.map.height):
        for col in range(0, app.map.width): 
            val = app.map.get(row, col)
            if val['object'] != 0 and val['monster'] != 0:
                res += f"Warning @({row},{col}) an object and a monster\n"
            if val['wall'] != 0 and val['height'] == 0:
                res += f"Warning @({row},{col}) set height=1 because of wall\n"
                app.map.set(1, row, col, 'height')
    if len(res) == 0:
        res = "No warning nor error found."
    else:
        app.refresh_map()
    app.display_info("Check map results", res)

exports = {
    'Check Map': check_map
}
