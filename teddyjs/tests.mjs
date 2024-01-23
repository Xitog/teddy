import { readFileSync, statSync } from "fs";
import { basename } from "path";

import { Header, LevelSet } from "./wolf.mjs";

const HEADER_PATH = "D:/Perso/Projets/git/teddy/data/Wolfenstein 3D/Shareware maps/1.0/MAPHEAD.WL1";
const LEVELS_PATH = "D:/Perso/Projets/git/teddy/data/Wolfenstein 3D/Shareware maps/1.0/MAPTEMP.WL1";

class File {
    constructor(path) {
        this.path = path;
        let stats = statSync(this.path);
        this.name = basename(path);
        this.size = stats.size;
        this.date = stats.mtime;
        console.log(stats);
    }
    toString() {
        return this.name;
    }
}

console.log('Wolf');

let headerFile = new File(HEADER_PATH);
let levelsFile = new File(LEVELS_PATH);

let data = readFileSync(headerFile.path);
console.log('Data:         ', data.constructor.name, data);
console.log('Date:         ', headerFile.date.constructor.name, headerFile.date);
console.log('Date US:      ', headerFile.date.toLocaleString('us-US', { timeZone: 'America/Chicago' }));
let sub = data.subarray(0, 2);
console.log('Subarray 0, 2:', sub.constructor.name, '(', sub.length, ')', sub)
console.log('Reading:');
for (const [index, element] of data.subarray(0, 2).entries()) {
    console.log('   ', index, element.toString(16).padStart(4, '0'));
}
let magic = data.readUInt16LE(0, 2); // 43981
console.log(magic, magic.toString(16));

// Reading with library
let header = new Header(headerFile, data);
console.log(header.toString());

let data2 = readFileSync(levelsFile.path);
console.log('Data:         ', data2.constructor.name, data2);

let levels = new LevelSet(data2, header);
console.log(levels.toString());
