
class Header {
    constructor(file, data) {
        console.log(data.constructor.name);
        this.file = file;
        this.total_nb_bytes = data.byteLength; // 402
        this.nb_bytes = 0;
        this.magic = data.readUInt16LE(0, 2); // 43981
        this.nb_bytes += 2;
        this.ptr = [];
        for (let i = 0; i < 100; i++) {
            let p = data.readUInt16LE(2 + i * 4, 2 + (i + 1) * 4);
            this.nb_bytes += 4;
            if (p == 0xffff) // Avant on continuait jusqu'au max, 100 !
            {
                break;
            }
            this.ptr.push(p);
        }
    }

    toString() {
        const name = this.file.name;
        const ext = name.split('.').pop();
        const date = this.file.date;
        const size = this.file.size;
        let s = 'File name = ' + name + " (" + ext + ")\n" +
            'Created US Time = ' + date.toLocaleString('us-US', { timeZone: 'America/Chicago' }) + "\n" +
            'Created FR Time = ' + date.toLocaleString('fr-FR', { timeZone: 'Europe/Brussels' }) + "\n" +
            'File size = ' + size + "\n";
        s += 'Magix hex = ' + this.magic.toString(16).padStart(4, '0') + '\n';
        s += `Number of bytes read = ${this.nb_bytes} / ${this.total_nb_bytes}\n`;
        s += `Number of levels = ${this.ptr.length}\n`;
        for (let i = 0; i < this.ptr.length; i++) {
            s += `    - Lvl ${(i + 1).toString().padStart(2, ' ')} start at `;
            s += this.ptr[i].toString(16).padStart(4, '0') + '(';
            s += this.ptr[i] + ')\n';
        }
        return s;
    }
}

class LevelSet {
    constructor(data, header) {
        this.header = header;
        let dec = new TextDecoder();
        this.format = dec.decode(data.subarray(0, 8));
        this.levels = [];
        for (let i of this.header.ptr) {
            console.log(`Reading ${i}`);
            this.levels.push(this.read_level(data, i, this.header.magic));
        }
        if (this.levels.length > 0) {
            let lvl = this.levels[this.levels.length - 1];
            let end = lvl.planes[2].start + lvl.planes[2].length;
            console.log(">>>>>>> " + end);
            console.log(">>>>>>> " + data.byteLength);
            if (end < data.byteLength) {
                this.ending = dec.decode(data.slice(end, data.byteLength));
            }
        }
    }

    getNames() {
        let res = [];
        console.log(this.levels);
        for (let lvl of this.levels) {
            console.log(typeof (lvl), lvl);
            res.push(lvl.name);
        }
        return res;
    }

    read_level(data, startLevel, magic) {
        let level = new Level();
        let startPlane0 = data.readUInt32LE(startLevel, startLevel + 4);
        let startPlane1 = data.readUInt32LE(startLevel + 4, startLevel + 8);
        let startPlane2 = data.readUInt32LE(startLevel + 8, startLevel + 12);
        let lengthPlane0 = data.readUInt16LE(startLevel + 12, startLevel + 14);
        let lengthPlane1 = data.readUInt16LE(startLevel + 14, startLevel + 16);
        let lengthPlane2 = data.readUInt16LE(startLevel + 16, startLevel + 18);
        level.width =data.readUInt16LE(startLevel + 18, startLevel + 20);
        level.height = data.readUInt16LE(startLevel + 20, startLevel + 22);
        let dec = new TextDecoder();
        level.name = dec.decode(data.subarray(startLevel + 22, startLevel + 32));
        let plane0 = this.rwle_expand(magic, data.slice(startPlane0, startPlane0 + lengthPlane0));
        let plane1 = this.rwle_expand(magic, data.slice(startPlane1, startPlane1 + lengthPlane1));
        let plane2 = this.rwle_expand(magic, data.slice(startPlane2, startPlane2 + lengthPlane2));
        level.planes.push(new Plane(startPlane0, lengthPlane0, plane0));
        level.planes.push(new Plane(startPlane1, lengthPlane1, plane1));
        level.planes.push(new Plane(startPlane2, lengthPlane2, plane2));
        return level;
    }

    rwle_expand(magic, data) {
        let size = data.readUInt16LE(0, 2);
        console.log("RWLE EXPAND SIZE = " + size);
        let expanded = [];
        let i = 2;
        while (i < data.byteLength) {
            let v = data.readUInt16LE(i, i + 2);
            i += 2
            if (v == magic) {
                let count = data.readUInt16LE(i, i + 2);
                i += 2
                let value = data.readUInt16LE(i, i + 2);
                i += 2
                expanded = expanded.concat(Array(count).fill(value));
            } else {
                expanded.push(v);
            }
        }
        return expanded;
    }

    toString() {
        let s = "Format: " + this.format + "\n";
        let nb = 0;
        for (let i of Object.values(this.levels)) {
            s += `[Level ${nb}]\n` + i.toString() + "\n";
            nb += 1;
        }
        s += "Ending: " + this.ending + "\n";
        return s;
    }
}


class Plane {
    constructor(start, length, content) {
        this.start = start;
        this.length = length;
        this.content = content;
    }
    toString() {
        return `Plane @${this.start} #${this.length}`;
    }
}

class Level {
    constructor() {
        this.planes = [];
    }

    getPlane(index) {
        if (this.planes[index] === undefined) {
            throw new Error("Non existant plane at " + index);
        }
        return this.planes[index];
    }

    toString() {
        let s = this.name;
        if (this.planes.length === 3) {
            s = "[Starts]            : " + this.planes[0].start + ", " + this.planes[1].start + ", " + this.planes[2].start + "\n" +
                "[CompressedLenghts] : " + this.planes[0].length + ", " + this.planes[1].length + ", " + this.planes[2].length + "\n" +
                "[ReadLengths]       : " + this.planes[0].content.length + ", " + this.planes[1].content.length + ", " + this.planes[2].content.length + "\n" +
                "[Sizes]             : " + this.width + "x" + this.length + " (" + (64 * 64) + ")\n" +
                "[Name]              : " + this.name;
        }
        return s;
    }
}

export {Header, LevelSet};
