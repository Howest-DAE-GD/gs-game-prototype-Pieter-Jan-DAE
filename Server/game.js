import { v4 as uuid } from 'uuid';

export class Game {
    constructor(boundaries) {

        // Game State
        this.boundaries = boundaries != undefined ? boundaries : { top: 1000, left: 0, right: 1000, bottom: 0 };
        this.players = {};
        this.attire = {};
        this.stateChanged = false;
        this.started = false;
        this.spawns = 0;
        this.typeToSpawn = 0;

        // Testing purposes
        const victim = new Player({ x: 0, y: 0 });
        this.players[victim.id] = victim;
        const a = new Attire({ x: 100, y: 100 }, Attire.TYPES.HAT);
        this.attire[a.id] = a;
    }

    start() {
        if(!this.started) {
            console.log(`The game has started!`);
            this.started = true;
            setInterval(() => {
                if (this.typeToSpawn <= Attire.TYPES.CANE) {
                    this.spawnAttire(this.typeToSpawn);
                    this.spawns += 1;
                    if (this.spawns > 4) {
                        this.spawns = 0;
                        this.typeToSpawn++;
                    }
                }
            }, 3000);
        }
    }

    spawnAttire(type)
    {
        const offset = 100;
        const pos = {
            x: Math.floor(Math.random() * (this.boundaries.right - 2*offset) + offset),
            y: Math.floor(Math.random() * (this.boundaries.top - 2*offset) + offset)
        };
        const a = new Attire(pos, type)
        this.attire[a.id] = a;
        this.stateChanged = true;
        return a.id;
    }

    addPlayer() {
        const p = new Player({ x: 300, y: 300 });
        this.players[p.id] = p;
        this.stateChanged = true;
        return p.id;
    }

    move(playerId, vector) {
        if (!(playerId in this.players)) {
            console.log(`Attempt to move with unknown playerId: ${playerId}`);
            return;
        }
        if (!vector || !('x' in vector) || !('y' in vector) || typeof vector.x !== 'number' || typeof vector.y !== 'number')
        {
            console.log(`Attempt to move with malformed vector: ${vector}`);
            return;
        }
        if (playerId in this.players) {
            const p = this.players[playerId];
            p.pos.x += vector.x;
            p.pos.y += vector.y;
            const b = this.boundaries;
            const epsilon = 0.001;
            if (p.pos.x < b.left) p.pos.x = b.left + epsilon;
            if (p.pos.x > b.right) p.pos.x = b.right - epsilon;
            if (p.pos.y < b.bottom) p.pos.y = b.bottom + epsilon;
            if (p.pos.y > b.top) p.pos.y = b.top - epsilon;
            this.stateChanged = true;
        }
    }

    take(playerId, attireId) {
        if (!(playerId in this.players)) {
            console.log(`Attempt to take with unknown playerId: ${playerId}`);
            return;
        }
        if (!(attireId in this.attire)) {
            console.log(`Attempt to take with unknown attireId: ${attireId}`);
            return;
        }
        const player = this.players[playerId];
        const attire = this.attire[attireId];
        if (player.add(attire.type)) {
            delete this.attire[attireId];
            this.stateChanged = true;
        }
    }

    drop(playerId, attireType) {
        if (!(playerId in this.players)) {
            console.log(`Attempt to drop with unknown playerId: ${playerId}`);
            return;
        }
        if (this.players[playerId].attire[attireType]) {
            const player = this.players[playerId];
            const pos = { x: player.pos.x, y: player.pos.y };
            const attire = new Attire(pos, attireType);
            this.attire[attire.id] = attire;
            player.remove(attireType);
            this.stateChanged = true;
        }
    }

    attack(aggressorId, victimId) {
        if (!(aggressorId in this.players)) {
            console.log(`Attempt to attack failed, unknown aggressorId: ${aggressorId}`);
            return;
        }
        if (!(victimId in this.players)) {
            console.log(`Attempt to attack failed, unknown victimId: ${victimId}`);
            return;
        }
        if (!this.players[aggressorId].isInSameCult(this.players[victimId])) {
            this.players[victimId].health -= 20;
            this.stateChanged = true;
        }
    }

    getState() {
        return { players: this.players, attire: this.attire };
    }

    hasStateChanged() {
        return this.stateChanged;
    }
}

export class GameObject {
    constructor(pos) {
        this.id = uuid();
        this.pos = pos;
    }
}

export class Player extends GameObject {
    constructor(pos) {
        super(pos);
        this.health = 100;
        this.maxHealth = 100;
        this.attire = {};
        for (let type of Object.values(Attire.TYPES)) {
            this.attire[type] = false;
        }
    }

    add(attireType) {
        if (this.attire[attireType]) return false;
        this.attire[attireType] = true;
        return true;
    }

    remove(attireType) {
        this.attire[attireType] = false;
    }

    isInSameCult(player) {
        for (let type of Object.values(Attire.TYPES)) {
            if (player.attire[type] !== this.attire[type]) return false;
        }
        return true;
    }
}

export class Attire extends GameObject {
    constructor(pos, type) {
        super(pos);
        this.type = type;
    }

    static TYPES = {
        HAT: 0,
        CANE: 1,
        OTHER: 2,
    }
}
