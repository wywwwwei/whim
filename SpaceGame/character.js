//set up the class GameObject
class GameObject {
    constructor(x, y) {
        this.x = x;
        this.y = y;
        this.width = 0;
        this.height = 0;
        this.dead = false;
        this.img = undefined;
        this.type = "";
    }

    draw(ctx) {
        ctx.drawImage(this.img, this.x, this.y, this.width, this.height);
    }

    rectFromGameObject() {
        return {
            top: this.y,
            left: this.x,
            bottom: this.y + this.height,
            right: this.x + this.width,
        };
    }
}

export function intersectRect(rect1, rect2) {
    return !(
        rect1.left > rect2.right ||
        rect1.right < rect2.left ||
        rect1.top > rect2.bottom ||
        rect1.bottom < rect2.top
    );
}

export class Hero extends GameObject {
    constructor(x, y) {
        super(x, y);
        this.width = 99;
        this.height = 75;
        this.type = "Hero";
        this.speed = { x: 0, y: 0 };
        this.cooldown = 0;
        this.life = 3;
        this.points = 0;
    }

    fire() {
        this.cooldown = 500;
        let cdInterval = setInterval(() => {
            this.cooldown = Math.max(0, this.cooldown - 100);
            if (this.cooldown === 0) {
                clearInterval(cdInterval);
            }
        }, 200);
        return new Laser(this.x + 45, this.y - 10);
    }

    canFire() {
        return this.cooldown === 0;
    }

    decreaseLife() {
        this.life--;
        if (this.life === 0) {
            this.dead = true;
        }
    }

    increasePoint() {
        this.points += 100;
    }
}

export class Enemy extends GameObject {
    constructor(x, y, moveMent) {
        super(x, y);
        this.width = 98;
        this.height = 50;
        this.type = "Enemy";
    }

    startMoveInRect(rect) {
        let movement = setInterval(() => {
            if (this.y < rect.bottom - this.height) {
                this.y += 5;
            } else {
                console.log("Enemy stopped at", this.y);
                clearInterval(movement);
            }
        }, 300);
    }
}

export class Laser extends GameObject {
    constructor(x, y, img) {
        super(x, y);
        this.width = 9;
        this.height = 33;
        this.type = "Laser";
        this.img = img;
    }

    startMoveInRect(rect) {
        let movement = setInterval(() => {
            if (this.y > rect.top) {
                this.y -= 15;
            } else {
                this.dead = true;
                clearInterval(movement);
            }
        }, 100);
    }
};