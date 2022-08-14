import { Messages, EventEmitter } from "./event.js";
import { Hero, Enemy, Laser, intersectRect } from "./character.js";

window.onload = async () => {
    const runner = new GameRunner();
    await runner.loadAssets();
    runner.initGame();
    runner.run();
};

function loadTexture(path) {
    return new Promise((resolve) => {
        const img = new Image();
        img.src = path;
        img.onload = () => {
            resolve(img);
        }
    })
}

class GameRunner {
    constructor() {
        this.canvas = document.getElementById("myCanvas");
        this.ctx = this.canvas.getContext("2d");

        this.eventEmitter = new EventEmitter();
        this.gameObjects = undefined;
        this.gameLoop = undefined;

        this.heroImg = undefined;
        this.enemyImg = undefined;
        this.laserImg = undefined;
        this.lifeImg = undefined;
    }

    initGame() {
        this.gameObjects = [];
        this.clearCanvas();
        this.initEventEmitter();
        this.createHero();
        this.createEnemies();
    }

    run() {
        this.gameLoop = setInterval(() => {
            this.clearCanvas();
            this.drawPoints();
            this.drawLife();
            this.updateGameObjects();
            this.drawGameObjects();
        }, 100);
    }

    async loadAssets() {
        this.heroImg = await loadTexture("./assets/player.png");
        this.enemyImg = await loadTexture("./assets/enemyShip.png");
        this.laserImg = await loadTexture("./assets/laserRed.png");
        this.lifeImg = await loadTexture("./assets/life.png");
    }

    clearCanvas() {
        this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
        this.ctx.fillStyle = "black";
        this.ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);
    }

    drawPoints() {
        this.ctx.font = "30px Arial";
        this.ctx.fillStyle = "red";
        this.ctx.textAlign = "left";
        this.ctx.fillText("Points: " + this.hero.points, 10, this.canvas.height - 37);
    }

    drawLife() {
        const offsetX = this.canvas.width - 180;
        const offsetY = this.canvas.height - 37;
        for (let i = 0; i < this.hero.life; i++) {
            this.ctx.drawImage(this.lifeImg, offsetX + 45 * i, offsetY);
        }
    }

    updateGameObjects() {
        const enemies = this.gameObjects.filter((obj) => (obj.type === "Enemy"));
        const lasers = this.gameObjects.filter((obj) => (obj.type === "Laser"));
        enemies.forEach((enemy) => {
            const heroRect = this.hero.rectFromGameObject();
            if (intersectRect(heroRect, enemy.rectFromGameObject())) {
                this.eventEmitter.emit(Messages.COLLISION_ENEMY_HERO, { enemy });
            }
        });
        lasers.forEach((laser) => {
            const laserRect = laser.rectFromGameObject();
            enemies.forEach((enemy) => {
                if (intersectRect(laserRect, enemy.rectFromGameObject())) {
                    this.eventEmitter.emit(Messages.COLLISION_ENEMY_LASER, { laser: laser, enemy: enemy });
                }
            });
        });
        this.gameObjects = this.gameObjects.filter((obj) => (!obj.dead));
    }

    drawGameObjects() {
        this.gameObjects.forEach((obj) => {
            obj.draw(this.ctx);
        });
    }

    initEventEmitter() {
        this.eventEmitter.on(Messages.KEY_EVENT_UP, () => {
            this.hero.y -= 5;
        });
        this.eventEmitter.on(Messages.KEY_EVENT_DOWN, () => {
            this.hero.y += 5;
        });
        this.eventEmitter.on(Messages.KEY_EVENT_LEFT, () => {
            this.hero.x -= 20;
        });
        this.eventEmitter.on(Messages.KEY_EVENT_RIGHT, () => {
            this.hero.x += 20;
        });
        this.eventEmitter.on(Messages.KEY_EVENT_ENTER, () => {
            this.reset();
        });
        this.eventEmitter.on(Messages.KEY_EVENT_SPACE, () => {
            if (this.hero.canFire()) {
                const laser = this.hero.fire();
                laser.img = this.laserImg;
                laser.startMoveInRect({
                    top: 0,
                    left: 0,
                    bottom: this.canvas.height,
                    right: this.canvas.width,
                });
                this.gameObjects.push(laser);
            }
        });
        this.eventEmitter.on(Messages.COLLISION_ENEMY_HERO, (_, { enemy }) => {
            enemy.dead = true;
            this.hero.decreaseLife();
            if (this.isHeroDead()) {
                this.eventEmitter.emit(Messages.GAME_END_LOSS);
                return;
            }
            if (this.isEnemiesDead()) {
                this.eventEmitter.emit(Messages.GAME_END_WIN);
            }
        });
        this.eventEmitter.on(Messages.COLLISION_ENEMY_LASER, (_, { laser, enemy }) => {
            laser.dead = true;
            enemy.dead = true;
            this.hero.increasePoint();
            if (this.isEnemiesDead()) {
                this.eventEmitter.emit(Messages.GAME_END_WIN);
            }
        });
        this.eventEmitter.on(Messages.GAME_END_WIN, () => {
            this.endGame(true);
        });
        this.eventEmitter.on(Messages.GAME_END_LOSS, () => {
            this.endGame(false);
        });
        window.addEventListener("keyup", (evt) => {
            if (evt.key === "ArrowLeft") {
                this.eventEmitter.emit(Messages.KEY_EVENT_LEFT);
            } else if (evt.key === "ArrowRight") {
                this.eventEmitter.emit(Messages.KEY_EVENT_RIGHT);
            } else if (evt.key === "ArrowUp") {
                this.eventEmitter.emit(Messages.KEY_EVENT_UP);
            } else if (evt.key === "ArrowDown") {
                this.eventEmitter.emit(Messages.KEY_EVENT_DOWN);
            } else if (evt.key === "Enter") {
                this.eventEmitter.emit(Messages.KEY_EVENT_ENTER);
            } else if (evt.key === " ") {
                this.eventEmitter.emit(Messages.KEY_EVENT_SPACE);
            }
        });
        window.addEventListener("keydown", (evt) => {
            if (evt.key === "ArrowLeft" ||
                evt.key === "ArrowRight" ||
                evt.key === "ArrowUp" ||
                evt.key === "ArrowDown" ||
                evt.key === " ") {
                evt.preventDefault();
            }
        });
    }

    createHero() {
        this.hero = new Hero(this.canvas.width / 2 - 45, this.canvas.height * 3 / 4);
        this.hero.img = this.heroImg;
        this.gameObjects.push(this.hero);
    }

    createEnemies() {
        const enemyCount = 5, enemyWidth = 98, enemyHeight = 50;
        const totalWidth = enemyCount * enemyWidth;
        const offsetX = (this.canvas.width - totalWidth) / 2;
        for (let i = 0; i < enemyCount; i++) {
            const originX = offsetX + i * enemyWidth;
            for (let originY = 0; originY < enemyCount * enemyHeight; originY += enemyHeight) {
                const enemy = new Enemy(originX, originY);
                enemy.img = this.enemyImg;
                enemy.startMoveInRect({
                    top: 0,
                    left: 0,
                    bottom: this.canvas.height,
                    right: this.canvas.width,
                });
                this.gameObjects.push(enemy);
            }
        }
    }

    endGame(win) {
        if (this.gameLoop) {
            clearInterval(this.gameLoop);

            setTimeout(() => {
                this.clearCanvas();
                let message, color;
                if (win) {
                    message = "Victory!!! Pew Pew... - Press [Enter] to start a new game Captain Pew Pew";
                    color = "green";
                } else {
                    message = "You died !!! Press [Enter] to start a new game Captain Pew Pew";
                    color = "red";
                }
                this.ctx.font = '30px Arial';
                this.ctx.fillStyle = color;
                this.ctx.textAlign = 'center';
                this.ctx.fillText(message, this.canvas.width / 2, this.canvas.height / 2);
            }, 200);
        }
    }

    reset() {
        if (this.gameLoop) {
            clearInterval(this.gameLoop);
            this.initGame();
            this.run();
        }
    }

    isHeroDead() {
        return this.hero.life <= 0;
    }

    isEnemiesDead() {
        return this.gameObjects.filter((obj) => (obj.type === "Enemy" && !obj.dead)).length <= 0;
    }
};